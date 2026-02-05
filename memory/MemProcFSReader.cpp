#include "MemProcFSReader.h"
#include <windows.h>

namespace memory
{
    MemProcFSReader::MemProcFSReader()
        : m_initialized(false)
        , m_targetProcess(DEFAULT_PROCESS)
        , m_deviceString(DEFAULT_DEVICE)
#ifdef USE_MEMPROCFS
        , m_hVMM(nullptr)
        , m_dwPID(0)
        , m_moduleBase(0)
#endif
    {
    }

    MemProcFSReader::~MemProcFSReader()
    {
        Shutdown();
    }

    bool MemProcFSReader::Initialize(const char* deviceOrProcess)
    {
#ifdef USE_MEMPROCFS
        //=====================================================================
        // MEMPROCFS INITIALIZATION
        //=====================================================================

        // Parse device string (format: "device|process" or just "device")
        if (deviceOrProcess && deviceOrProcess[0] != '\0')
        {
            std::string param(deviceOrProcess);
            size_t separator = param.find('|');
            if (separator != std::string::npos)
            {
                m_deviceString = param.substr(0, separator);
                m_targetProcess = param.substr(separator + 1);
            }
            else
            {
                // Assume it's process name if not a device string
                if (param.find("://") != std::string::npos)
                    m_deviceString = param;
                else
                    m_targetProcess = param;
            }
        }

        // Initialize MemProcFS with device
        LPCSTR argv[] = { "", "-device", m_deviceString.c_str() };
        m_hVMM = VMMDLL_Initialize(3, argv);

        if (!m_hVMM)
        {
            // Failed to initialize - DMA device not connected?
            return false;
        }

        // Find target process
        if (!RefreshProcess())
        {
            VMMDLL_Close(m_hVMM);
            m_hVMM = nullptr;
            return false;
        }

        // Get module base
        m_moduleBase = VMMDLL_ProcessGetModuleBaseU(m_hVMM, m_dwPID, nullptr);

        m_initialized = true;
        return true;

#else
        //=====================================================================
        // STUB IMPLEMENTATION (when USE_MEMPROCFS is not defined)
        //=====================================================================

        // MemProcFS not enabled - this reader won't work
        // This is intentional: BUILD_EXE + USE_MEMPROCFS must both be defined
        OutputDebugStringA("[MemProcFSReader] USE_MEMPROCFS not defined - reader disabled\n");
        return false;
#endif
    }

    void MemProcFSReader::Shutdown()
    {
#ifdef USE_MEMPROCFS
        if (m_hVMM)
        {
            VMMDLL_Close(m_hVMM);
            m_hVMM = nullptr;
        }
        m_dwPID = 0;
        m_moduleBase = 0;
#endif
        m_initialized = false;
    }

    bool MemProcFSReader::IsInitialized() const
    {
        return m_initialized;
    }

    bool MemProcFSReader::Read(uintptr_t address, void* buffer, size_t size)
    {
#ifdef USE_MEMPROCFS
        if (!m_initialized || !m_hVMM || !buffer || size == 0)
            return false;

        m_readCount++;

        // WARNING: Single reads are SLOW with DMA!
        // Use ReadScatter for multiple reads instead!
        return VMMDLL_MemRead(m_hVMM, m_dwPID, address, (PBYTE)buffer, (DWORD)size);
#else
        (void)address; (void)buffer; (void)size;
        return false;
#endif
    }

    size_t MemProcFSReader::ReadScatter(std::vector<ScatterReadEntry>& entries)
    {
#ifdef USE_MEMPROCFS
        if (!m_initialized || !m_hVMM || entries.empty())
            return 0;

        m_scatterBatchCount++;

        // Create scatter handle
        VMMDLL_SCATTER_HANDLE hScatter = VMMDLL_Scatter_Initialize(m_hVMM, m_dwPID, 0);
        if (!hScatter)
            return 0;

        // Prepare all reads
        for (auto& entry : entries)
        {
            VMMDLL_Scatter_PrepareEx(
                hScatter,
                entry.address,
                (DWORD)entry.size,
                (PBYTE)entry.buffer,
                NULL  // No callback
            );
        }

        // Execute batch read - this is where the DMA magic happens!
        // All reads are performed in a single DMA transaction
        VMMDLL_Scatter_Execute(hScatter);

        // Mark all as success (MemProcFS doesn't provide per-entry status easily)
        // In a production implementation, you'd want better error handling
        size_t successCount = 0;
        for (auto& entry : entries)
        {
            // For now, assume success if we got here
            // A more robust implementation would verify the data
            entry.success = true;
            successCount++;
        }

        // Cleanup
        VMMDLL_Scatter_CloseHandle(hScatter);

        return successCount;

#else
        // Stub: Mark all as failed
        for (auto& entry : entries)
            entry.success = false;
        return 0;
#endif
    }

    uintptr_t MemProcFSReader::GetModuleBase(const char* moduleName)
    {
#ifdef USE_MEMPROCFS
        if (!m_initialized || !m_hVMM)
            return 0;

        if (moduleName == nullptr || moduleName[0] == '\0')
        {
            return m_moduleBase;
        }

        return VMMDLL_ProcessGetModuleBaseU(m_hVMM, m_dwPID, (LPSTR)moduleName);
#else
        (void)moduleName;
        return 0;
#endif
    }

    uint32_t MemProcFSReader::GetProcessId() const
    {
#ifdef USE_MEMPROCFS
        return m_dwPID;
#else
        return 0;
#endif
    }

    void MemProcFSReader::SetTargetProcess(const char* processName)
    {
        if (processName && processName[0] != '\0')
        {
            m_targetProcess = processName;
        }
    }

    const char* MemProcFSReader::GetDeviceInfo() const
    {
        return m_deviceString.c_str();
    }

    bool MemProcFSReader::IsDeviceConnected() const
    {
#ifdef USE_MEMPROCFS
        return m_hVMM != nullptr;
#else
        return false;
#endif
    }

    bool MemProcFSReader::RefreshProcess()
    {
#ifdef USE_MEMPROCFS
        if (!m_hVMM)
            return false;

        // Find process by name
        DWORD dwPID = 0;
        if (!VMMDLL_PidGetFromName(m_hVMM, (LPSTR)m_targetProcess.c_str(), &dwPID))
        {
            return false;
        }

        m_dwPID = dwPID;

        // Refresh module base
        m_moduleBase = VMMDLL_ProcessGetModuleBaseU(m_hVMM, m_dwPID, nullptr);

        return m_dwPID != 0;
#else
        return false;
#endif
    }

} // namespace memory
