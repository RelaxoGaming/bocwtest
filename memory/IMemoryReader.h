#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

//=============================================================================
// MEMORY READER ABSTRACTION LAYER
//
// This interface abstracts memory reading operations to support:
// - Local memory access (DLL mode - direct pointers)
// - Remote memory access (EXE mode - MemProcFS/DMA)
//
// Design Goals:
// - Minimize read operations for DMA performance
// - Support scatter reading for batch operations
// - Frame-based caching to avoid redundant reads
//=============================================================================

namespace memory
{
    //-------------------------------------------------------------------------
    // Scatter Read Entry
    // Used for batching multiple memory reads into a single operation
    // Critical for DMA performance with MemProcFS
    //-------------------------------------------------------------------------
    struct ScatterReadEntry
    {
        uintptr_t   address;        // Virtual address to read from
        void*       buffer;         // Destination buffer
        size_t      size;           // Bytes to read
        bool        success;        // Result of the read operation

        ScatterReadEntry()
            : address(0), buffer(nullptr), size(0), success(false) {}

        ScatterReadEntry(uintptr_t addr, void* buf, size_t sz)
            : address(addr), buffer(buf), size(sz), success(false) {}
    };

    //-------------------------------------------------------------------------
    // Memory Reader Interface
    // Abstract base class for all memory reading implementations
    //-------------------------------------------------------------------------
    class IMemoryReader
    {
    public:
        virtual ~IMemoryReader() = default;

        //---------------------------------------------------------------------
        // Initialization
        //---------------------------------------------------------------------

        // Initialize the memory reader
        // For local: nothing needed
        // For MemProcFS: device string and process name
        virtual bool Initialize(const char* deviceOrProcess = nullptr) = 0;

        // Cleanup resources
        virtual void Shutdown() = 0;

        // Check if reader is ready
        virtual bool IsInitialized() const = 0;

        //---------------------------------------------------------------------
        // Basic Read Operations
        // Note: For DMA, prefer scatter reads over individual reads!
        //---------------------------------------------------------------------

        // Read memory at address into buffer
        virtual bool Read(uintptr_t address, void* buffer, size_t size) = 0;

        // Template helper for typed reads
        template<typename T>
        bool Read(uintptr_t address, T& value)
        {
            return Read(address, &value, sizeof(T));
        }

        // Template helper that returns value (returns default on failure)
        template<typename T>
        T Read(uintptr_t address)
        {
            T value{};
            Read(address, &value, sizeof(T));
            return value;
        }

        //---------------------------------------------------------------------
        // Scatter Read Operations (CRITICAL FOR DMA PERFORMANCE!)
        //
        // Usage Pattern:
        // 1. Collect all addresses you need to read
        // 2. Call ReadScatter once with all entries
        // 3. Process results
        //
        // This is MUCH faster than individual reads with DMA!
        //---------------------------------------------------------------------

        // Execute batch read of multiple addresses
        // Returns number of successful reads
        virtual size_t ReadScatter(std::vector<ScatterReadEntry>& entries) = 0;

        //---------------------------------------------------------------------
        // Process Information
        //---------------------------------------------------------------------

        // Get base address of target module (usually main executable)
        virtual uintptr_t GetModuleBase(const char* moduleName = nullptr) = 0;

        // Get process ID (for local: current process, for remote: target process)
        virtual uint32_t GetProcessId() const = 0;

        //---------------------------------------------------------------------
        // Statistics (for debugging/optimization)
        //---------------------------------------------------------------------

        // Get number of read operations this frame
        virtual size_t GetReadCount() const { return m_readCount; }

        // Get number of scatter batches this frame
        virtual size_t GetScatterBatchCount() const { return m_scatterBatchCount; }

        // Reset frame statistics
        virtual void ResetFrameStats()
        {
            m_readCount = 0;
            m_scatterBatchCount = 0;
        }

    protected:
        size_t m_readCount = 0;
        size_t m_scatterBatchCount = 0;
    };

    //-------------------------------------------------------------------------
    // Scatter Read Builder
    // Helper class to collect and execute scatter reads
    //-------------------------------------------------------------------------
    class ScatterReadBuilder
    {
    public:
        ScatterReadBuilder(IMemoryReader* reader) : m_reader(reader) {}

        // Add a read request to the batch
        template<typename T>
        ScatterReadBuilder& Add(uintptr_t address, T* destination)
        {
            m_entries.emplace_back(address, destination, sizeof(T));
            return *this;
        }

        // Add raw read request
        ScatterReadBuilder& AddRaw(uintptr_t address, void* buffer, size_t size)
        {
            m_entries.emplace_back(address, buffer, size);
            return *this;
        }

        // Execute all reads and return success count
        size_t Execute()
        {
            if (m_entries.empty() || !m_reader)
                return 0;

            size_t result = m_reader->ReadScatter(m_entries);
            return result;
        }

        // Check if a specific entry succeeded (call after Execute)
        bool Succeeded(size_t index) const
        {
            return index < m_entries.size() && m_entries[index].success;
        }

        // Clear all entries
        void Clear()
        {
            m_entries.clear();
        }

        // Get entry count
        size_t Count() const { return m_entries.size(); }

        // Get entries for inspection
        const std::vector<ScatterReadEntry>& GetEntries() const { return m_entries; }

    private:
        IMemoryReader* m_reader;
        std::vector<ScatterReadEntry> m_entries;
    };

    //-------------------------------------------------------------------------
    // Global Memory Reader Instance
    // Set during initialization, used throughout the application
    //-------------------------------------------------------------------------

    // Get the global memory reader instance
    IMemoryReader* GetMemoryReader();

    // Set the global memory reader instance
    void SetMemoryReader(std::unique_ptr<IMemoryReader> reader);

    // Initialize appropriate reader based on build config
    bool InitializeMemoryReader(const char* targetProcess = nullptr);

    // Shutdown and cleanup
    void ShutdownMemoryReader();

} // namespace memory

//=============================================================================
// CONVENIENCE MACROS
// Use these to read memory throughout the codebase
//=============================================================================

// Read a value from memory
#define MEM_READ(addr, var) memory::GetMemoryReader()->Read(addr, var)

// Read and return a value
#define MEM_READ_VAL(type, addr) memory::GetMemoryReader()->Read<type>(addr)

// Create a scatter read builder
#define MEM_SCATTER() memory::ScatterReadBuilder(memory::GetMemoryReader())
