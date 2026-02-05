#pragma once
#include "IMemoryReader.h"
#include "../build_config.h"
#include <unordered_map>
#include <chrono>
#include <array>

//=============================================================================
// FRAME-BASED MEMORY CACHE
//
// Caches frequently read data to minimize DMA reads.
// Critical for DMA performance where reads are expensive!
//
// Usage:
// - Call BeginFrame() at the start of each render frame
// - Use cached getters for frequently accessed data
// - Cache automatically invalidates each frame
//
// Cached Data:
// - Camera/View Matrix (read once per frame, used by all W2S calls)
// - Player base addresses
// - Entity data (positions, health, etc.)
//=============================================================================

namespace memory
{
    //-------------------------------------------------------------------------
    // Cached Data Structures
    // Grouped for efficient scatter reading
    //-------------------------------------------------------------------------

    // Camera data - read ONCE per frame, used by all WorldToScreen calls
    struct CachedCameraData
    {
        float viewMatrix[16];       // 4x4 view/projection matrix
        float viewOrigin[3];        // Camera position
        float viewAngles[3];        // Camera angles (pitch, yaw, roll)
        float fov;                  // Field of view
        int   screenWidth;          // Screen dimensions
        int   screenHeight;
        bool  valid;                // Is data valid?

        CachedCameraData() : valid(false), fov(90.0f), screenWidth(1920), screenHeight(1080)
        {
            memset(viewMatrix, 0, sizeof(viewMatrix));
            memset(viewOrigin, 0, sizeof(viewOrigin));
            memset(viewAngles, 0, sizeof(viewAngles));
        }
    };

    // Entity data - cached per entity
    struct CachedEntityData
    {
        uintptr_t baseAddress;      // Entity base address
        float   position[3];        // World position
        float   health;             // Current health
        int     team;               // Team ID
        int     clientNum;          // Client number
        bool    isAlive;            // Is entity alive
        bool    isValid;            // Is data valid
        bool    isVisible;          // Visibility (if computed)

        CachedEntityData() : baseAddress(0), health(0), team(0), clientNum(0),
                            isAlive(false), isValid(false), isVisible(false)
        {
            memset(position, 0, sizeof(position));
        }
    };

    // Local player data
    struct CachedLocalPlayerData
    {
        uintptr_t baseAddress;
        float   position[3];
        float   viewAngles[3];
        int     team;
        int     clientNum;
        bool    valid;

        CachedLocalPlayerData() : baseAddress(0), team(0), clientNum(-1), valid(false)
        {
            memset(position, 0, sizeof(position));
            memset(viewAngles, 0, sizeof(viewAngles));
        }
    };

    //-------------------------------------------------------------------------
    // Memory Cache Manager
    //-------------------------------------------------------------------------
    class MemoryCache
    {
    public:
        static MemoryCache& Instance()
        {
            static MemoryCache instance;
            return instance;
        }

        //---------------------------------------------------------------------
        // Frame Management
        //---------------------------------------------------------------------

        // Call at the start of each frame - invalidates stale cache
        void BeginFrame();

        // Call at the end of each frame - update statistics
        void EndFrame();

        // Get current frame number
        uint64_t GetFrameNumber() const { return m_frameNumber; }

        //---------------------------------------------------------------------
        // Camera Data (CRITICAL for W2S performance!)
        //---------------------------------------------------------------------

        // Get cached camera data - reads from memory only once per frame
        const CachedCameraData& GetCameraData();

        // Force camera refresh (call if view changed mid-frame, rare)
        void InvalidateCamera();

        // Set camera addresses for reading (call once during init)
        void SetCameraAddresses(uintptr_t viewMatrixAddr, uintptr_t viewOriginAddr,
                                uintptr_t viewAnglesAddr, uintptr_t fovAddr);

        //---------------------------------------------------------------------
        // Local Player Data
        //---------------------------------------------------------------------

        const CachedLocalPlayerData& GetLocalPlayerData();
        void InvalidateLocalPlayer();
        void SetLocalPlayerAddresses(uintptr_t baseAddr, uintptr_t posOffset,
                                     uintptr_t anglesOffset, uintptr_t teamOffset);

        //---------------------------------------------------------------------
        // Entity Data (Batch reading for DMA efficiency)
        //---------------------------------------------------------------------

        // Get cached entity data by index
        const CachedEntityData* GetEntityData(int index);

        // Batch update all entities - PREFERRED for DMA!
        // This uses scatter reading for optimal performance
        void UpdateAllEntities(const std::vector<uintptr_t>& entityAddresses);

        // Invalidate specific entity
        void InvalidateEntity(int index);

        // Invalidate all entities
        void InvalidateAllEntities();

        // Set entity offsets for reading
        void SetEntityOffsets(size_t posOffset, size_t healthOffset,
                             size_t teamOffset, size_t aliveOffset);

        //---------------------------------------------------------------------
        // Statistics
        //---------------------------------------------------------------------

        struct CacheStats
        {
            size_t cacheHits;
            size_t cacheMisses;
            size_t scatterReads;
            size_t totalReads;
        };

        const CacheStats& GetStats() const { return m_stats; }
        void ResetStats();

    private:
        MemoryCache();
        ~MemoryCache() = default;
        MemoryCache(const MemoryCache&) = delete;
        MemoryCache& operator=(const MemoryCache&) = delete;

        // Frame tracking
        uint64_t m_frameNumber;
        uint64_t m_cameraValidFrame;
        uint64_t m_localPlayerValidFrame;

        // Cached data
        CachedCameraData m_cameraData;
        CachedLocalPlayerData m_localPlayerData;
        std::array<CachedEntityData, MAX_CACHED_ENTITIES> m_entityData;
        std::array<uint64_t, MAX_CACHED_ENTITIES> m_entityValidFrame;

        // Addresses for reading
        struct CameraAddresses
        {
            uintptr_t viewMatrix;
            uintptr_t viewOrigin;
            uintptr_t viewAngles;
            uintptr_t fov;
        } m_cameraAddrs;

        struct LocalPlayerAddresses
        {
            uintptr_t base;
            size_t posOffset;
            size_t anglesOffset;
            size_t teamOffset;
        } m_localPlayerAddrs;

        struct EntityOffsets
        {
            size_t position;
            size_t health;
            size_t team;
            size_t alive;
        } m_entityOffsets;

        // Statistics
        CacheStats m_stats;

        // Internal helpers
        void ReadCameraData();
        void ReadLocalPlayerData();
        void ReadEntityData(int index, uintptr_t baseAddress);
    };

    //-------------------------------------------------------------------------
    // Convenience Functions
    //-------------------------------------------------------------------------

    // Quick access to cache instance
    inline MemoryCache& Cache() { return MemoryCache::Instance(); }

} // namespace memory

//=============================================================================
// MACROS FOR EASY ACCESS
//=============================================================================

// Get cached camera data
#define CACHED_CAMERA() memory::Cache().GetCameraData()

// Get cached local player
#define CACHED_LOCAL_PLAYER() memory::Cache().GetLocalPlayerData()

// Get cached entity by index
#define CACHED_ENTITY(idx) memory::Cache().GetEntityData(idx)

// Frame management
#define CACHE_BEGIN_FRAME() memory::Cache().BeginFrame()
#define CACHE_END_FRAME() memory::Cache().EndFrame()
