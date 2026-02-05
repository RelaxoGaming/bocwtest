#include "LocalMemoryReader.h"
#include <windows.h>
#include <psapi.h>
#include <cstring>

namespace memory
{
    //=========================================================================
    // Global Memory Reader Instance
    //=========================================================================

    static std::unique_ptr<IMemoryReader> g_memoryReader;

    IMemoryReader* GetMemoryReader()
    {
        return g_memoryReader.get();
    }

    void SetMemoryReader(std::unique_ptr<IMemoryReader> reader)
    {
        g_memoryReader = std::move(reader);
    }

    bool InitializeMemoryReader(const char* targetProcess)
    {
        // Create appropriate reader based on build config
#ifdef USE_LOCAL_MEMORY_READER
        auto reader = std::make_unique<LocalMemoryReader>();
#elif defined(USE_MEMPROCFS_READER)
        // Will be implemented in MemProcFSReader.cpp
        auto reader = std::make_unique<MemProcFSReader>();
#else
        #error "No memory reader defined!"
#endif

        if (!reader->Initialize(targetProcess))
        {
            return false;
        }

        SetMemoryReader(std::move(reader));
        return true;
    }

    void ShutdownMemoryReader()
    {
        if (g_memoryReader)
        {
            g_memoryReader->Shutdown();
            g_memoryReader.reset();
        }
    }

    //=========================================================================
    // LocalMemoryReader Implementation
    //=========================================================================

    LocalMemoryReader::LocalMemoryReader()
        : m_initialized(false)
        , m_moduleBase(0)
        , m_processId(0)
    {
    }

    LocalMemoryReader::~LocalMemoryReader()
    {
        Shutdown();
    }

    bool LocalMemoryReader::Initialize(const char* /*deviceOrProcess*/)
    {
        // In DLL mode, we're already in the target process
        m_processId = GetCurrentProcessId();
        m_moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
        m_initialized = (m_moduleBase != 0);

        return m_initialized;
    }

    void LocalMemoryReader::Shutdown()
    {
        m_initialized = false;
        m_moduleBase = 0;
        m_processId = 0;
    }

    bool LocalMemoryReader::IsInitialized() const
    {
        return m_initialized;
    }

    bool LocalMemoryReader::IsValidAddress(uintptr_t address) const
    {
        // Basic validation - check if address is in valid range
        // In a real implementation, you might want to use VirtualQuery
        if (address == 0)
            return false;

        // Optional: More thorough validation with VirtualQuery
        // MEMORY_BASIC_INFORMATION mbi;
        // if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi)) == 0)
        //     return false;
        // return (mbi.State == MEM_COMMIT &&
        //         !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)));

        return true;
    }

    bool LocalMemoryReader::Read(uintptr_t address, void* buffer, size_t size)
    {
        if (!m_initialized || !buffer || size == 0)
            return false;

        if (!IsValidAddress(address))
            return false;

#ifdef MEMORY_READ_STATS
        m_readCount++;
#endif

        // In DLL mode, we can directly copy memory
        // This is essentially a memcpy - very fast!
        __try
        {
            memcpy(buffer, reinterpret_cast<const void*>(address), size);
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // Access violation - address was invalid
            return false;
        }
    }

    size_t LocalMemoryReader::ReadScatter(std::vector<ScatterReadEntry>& entries)
    {
        if (!m_initialized || entries.empty())
            return 0;

#ifdef MEMORY_READ_STATS
        m_scatterBatchCount++;
#endif

        size_t successCount = 0;

        // In DLL mode, scatter reads are just individual reads
        // But we keep the interface consistent for future DMA support
        for (auto& entry : entries)
        {
            entry.success = Read(entry.address, entry.buffer, entry.size);
            if (entry.success)
                successCount++;
        }

        return successCount;
    }

    uintptr_t LocalMemoryReader::GetModuleBase(const char* moduleName)
    {
        if (!m_initialized)
            return 0;

        if (moduleName == nullptr || moduleName[0] == '\0')
        {
            // Return main module base
            return m_moduleBase;
        }

        // Get specific module base
        HMODULE hModule = GetModuleHandleA(moduleName);
        return reinterpret_cast<uintptr_t>(hModule);
    }

    uint32_t LocalMemoryReader::GetProcessId() const
    {
        return m_processId;
    }

} // namespace memory
