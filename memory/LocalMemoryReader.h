#pragma once
#include "IMemoryReader.h"
#include "../build_config.h"

//=============================================================================
// LOCAL MEMORY READER
//
// Memory reader for DLL mode - direct memory access within the process.
// This is the default reader when injected into the target process.
//
// In DLL mode, we have direct access to the process memory, so reads
// are essentially just pointer dereferences - very fast!
//=============================================================================

namespace memory
{
    class LocalMemoryReader : public IMemoryReader
    {
    public:
        LocalMemoryReader();
        virtual ~LocalMemoryReader();

        //---------------------------------------------------------------------
        // IMemoryReader Interface Implementation
        //---------------------------------------------------------------------

        bool Initialize(const char* deviceOrProcess = nullptr) override;
        void Shutdown() override;
        bool IsInitialized() const override;

        bool Read(uintptr_t address, void* buffer, size_t size) override;
        size_t ReadScatter(std::vector<ScatterReadEntry>& entries) override;

        uintptr_t GetModuleBase(const char* moduleName = nullptr) override;
        uint32_t GetProcessId() const override;

    private:
        bool        m_initialized;
        uintptr_t   m_moduleBase;
        uint32_t    m_processId;

        // Helper to check if address is valid
        bool IsValidAddress(uintptr_t address) const;
    };

} // namespace memory
