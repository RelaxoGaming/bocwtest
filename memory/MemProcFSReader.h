#pragma once
#include "IMemoryReader.h"
#include "../build_config.h"

//=============================================================================
// MEMPROCFS MEMORY READER (STUB)
//
// Memory reader for EXE mode using MemProcFS library for DMA-based access.
// This allows reading memory from a target PC via PCIe DMA hardware.
//
// REQUIREMENTS:
// - MemProcFS library (vmmdll.h, vmmdll.lib)
// - DMA hardware (FPGA-based PCIe device)
// - Two-PC setup: Target PC (PC1) and Overlay PC (PC2)
//
// PERFORMANCE NOTES:
// - DMA reads are MUCH slower than local reads
// - ALWAYS use scatter reads (ReadScatter) instead of individual reads
// - Optimal scatter batch size: 50-100 entries per call
// - Cache frequently accessed data (camera matrix, player bases)
// - Never read the same data multiple times per frame
//
// MemProcFS API Overview:
// - VMMDLL_Initialize(): Initialize library with device string
// - VMMDLL_PidGetFromName(): Get process ID by name
// - VMMDLL_MemRead(): Single memory read (SLOW!)
// - VMMDLL_MemReadScatter(): Batch memory read (FAST!)
// - VMMDLL_ProcessGetModuleBase(): Get module base address
//=============================================================================

#ifdef USE_MEMPROCFS
// Include MemProcFS header when enabled
// Download from: https://github.com/ufrisk/MemProcFS
#include <vmmdll.h>
#pragma comment(lib, "vmmdll.lib")
#endif

namespace memory
{
    class MemProcFSReader : public IMemoryReader
    {
    public:
        MemProcFSReader();
        virtual ~MemProcFSReader();

        //---------------------------------------------------------------------
        // IMemoryReader Interface Implementation
        //---------------------------------------------------------------------

        // Initialize with device string, e.g.:
        // - "fpga://algo=0" (standard FPGA)
        // - "fpga://algo=0,device=0" (specific device)
        // After device init, searches for target process
        bool Initialize(const char* deviceOrProcess = nullptr) override;

        void Shutdown() override;
        bool IsInitialized() const override;

        // WARNING: Slow! Use ReadScatter for multiple reads!
        bool Read(uintptr_t address, void* buffer, size_t size) override;

        // PREFERRED: Batch read for optimal DMA performance
        // Optimal batch size: 50-100 entries
        size_t ReadScatter(std::vector<ScatterReadEntry>& entries) override;

        uintptr_t GetModuleBase(const char* moduleName = nullptr) override;
        uint32_t GetProcessId() const override;

        //---------------------------------------------------------------------
        // MemProcFS Specific Methods
        //---------------------------------------------------------------------

        // Set the target process name to search for
        void SetTargetProcess(const char* processName);

        // Get device information string
        const char* GetDeviceInfo() const;

        // Check if DMA device is connected
        bool IsDeviceConnected() const;

        // Refresh process list (call if target process restarts)
        bool RefreshProcess();

    private:
#ifdef USE_MEMPROCFS
        VMM_HANDLE  m_hVMM;             // MemProcFS handle
        DWORD       m_dwPID;            // Target process ID
        ULONG64     m_moduleBase;       // Target module base address

        // Scatter read structures (pre-allocated for performance)
        std::vector<VMMDLL_SCATTER_HANDLE> m_scatterHandles;

        // Helper to prepare scatter structures
        bool PrepareScatterRead(const std::vector<ScatterReadEntry>& entries);
        void ExecuteScatterRead();
        void CollectScatterResults(std::vector<ScatterReadEntry>& entries);
#endif

        bool        m_initialized;
        std::string m_targetProcess;
        std::string m_deviceString;

        // Default device and process
        static constexpr const char* DEFAULT_DEVICE = "fpga://algo=0";
        static constexpr const char* DEFAULT_PROCESS = "BlackOpsColdWar.exe";
    };

    //=========================================================================
    // IMPLEMENTATION NOTES (for future development)
    //=========================================================================
    //
    // When implementing MemProcFSReader::ReadScatter():
    //
    // 1. Create scatter handle:
    //    VMMDLL_SCATTER_HANDLE hScatter = VMMDLL_Scatter_Initialize(hVMM, dwPID, 0);
    //
    // 2. Prepare reads:
    //    for (auto& entry : entries) {
    //        VMMDLL_Scatter_Prepare(hScatter, entry.address, entry.size);
    //    }
    //
    // 3. Execute batch:
    //    VMMDLL_Scatter_Execute(hScatter);
    //
    // 4. Read results:
    //    for (auto& entry : entries) {
    //        VMMDLL_Scatter_Read(hScatter, entry.address, entry.size, entry.buffer, NULL);
    //    }
    //
    // 5. Close handle:
    //    VMMDLL_Scatter_CloseHandle(hScatter);
    //
    //=========================================================================

} // namespace memory
