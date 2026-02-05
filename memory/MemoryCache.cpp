#include "MemoryCache.h"
#include <cstring>

namespace memory
{
    MemoryCache::MemoryCache()
        : m_frameNumber(0)
        , m_cameraValidFrame(0)
        , m_localPlayerValidFrame(0)
    {
        memset(&m_cameraAddrs, 0, sizeof(m_cameraAddrs));
        memset(&m_localPlayerAddrs, 0, sizeof(m_localPlayerAddrs));
        memset(&m_entityOffsets, 0, sizeof(m_entityOffsets));
        memset(&m_stats, 0, sizeof(m_stats));

        // Initialize entity valid frames
        m_entityValidFrame.fill(0);
    }

    //=========================================================================
    // Frame Management
    //=========================================================================

    void MemoryCache::BeginFrame()
    {
        m_frameNumber++;

        // Reset per-frame stats
        if (auto* reader = GetMemoryReader())
        {
            reader->ResetFrameStats();
        }
    }

    void MemoryCache::EndFrame()
    {
        // Update statistics from reader
        if (auto* reader = GetMemoryReader())
        {
            m_stats.totalReads += reader->GetReadCount();
            m_stats.scatterReads += reader->GetScatterBatchCount();
        }
    }

    //=========================================================================
    // Camera Data
    //=========================================================================

    const CachedCameraData& MemoryCache::GetCameraData()
    {
        // Check if cache is valid for this frame
        if (m_cameraValidFrame >= m_frameNumber && m_cameraData.valid)
        {
            m_stats.cacheHits++;
            return m_cameraData;
        }

        // Cache miss - need to read from memory
        m_stats.cacheMisses++;
        ReadCameraData();

        return m_cameraData;
    }

    void MemoryCache::InvalidateCamera()
    {
        m_cameraData.valid = false;
        m_cameraValidFrame = 0;
    }

    void MemoryCache::SetCameraAddresses(uintptr_t viewMatrixAddr, uintptr_t viewOriginAddr,
                                         uintptr_t viewAnglesAddr, uintptr_t fovAddr)
    {
        m_cameraAddrs.viewMatrix = viewMatrixAddr;
        m_cameraAddrs.viewOrigin = viewOriginAddr;
        m_cameraAddrs.viewAngles = viewAnglesAddr;
        m_cameraAddrs.fov = fovAddr;
        InvalidateCamera();
    }

    void MemoryCache::ReadCameraData()
    {
        auto* reader = GetMemoryReader();
        if (!reader)
        {
            m_cameraData.valid = false;
            return;
        }

        // Use scatter read for efficiency
        ScatterReadBuilder scatter(reader);

        if (m_cameraAddrs.viewMatrix)
            scatter.AddRaw(m_cameraAddrs.viewMatrix, m_cameraData.viewMatrix, sizeof(m_cameraData.viewMatrix));

        if (m_cameraAddrs.viewOrigin)
            scatter.AddRaw(m_cameraAddrs.viewOrigin, m_cameraData.viewOrigin, sizeof(m_cameraData.viewOrigin));

        if (m_cameraAddrs.viewAngles)
            scatter.AddRaw(m_cameraAddrs.viewAngles, m_cameraData.viewAngles, sizeof(m_cameraData.viewAngles));

        if (m_cameraAddrs.fov)
            scatter.Add(m_cameraAddrs.fov, &m_cameraData.fov);

        // Execute batch read
        size_t success = scatter.Execute();

        m_cameraData.valid = (success > 0);
        m_cameraValidFrame = m_frameNumber;
    }

    //=========================================================================
    // Local Player Data
    //=========================================================================

    const CachedLocalPlayerData& MemoryCache::GetLocalPlayerData()
    {
        if (m_localPlayerValidFrame >= m_frameNumber && m_localPlayerData.valid)
        {
            m_stats.cacheHits++;
            return m_localPlayerData;
        }

        m_stats.cacheMisses++;
        ReadLocalPlayerData();

        return m_localPlayerData;
    }

    void MemoryCache::InvalidateLocalPlayer()
    {
        m_localPlayerData.valid = false;
        m_localPlayerValidFrame = 0;
    }

    void MemoryCache::SetLocalPlayerAddresses(uintptr_t baseAddr, uintptr_t posOffset,
                                              uintptr_t anglesOffset, uintptr_t teamOffset)
    {
        m_localPlayerAddrs.base = baseAddr;
        m_localPlayerAddrs.posOffset = posOffset;
        m_localPlayerAddrs.anglesOffset = anglesOffset;
        m_localPlayerAddrs.teamOffset = teamOffset;
        InvalidateLocalPlayer();
    }

    void MemoryCache::ReadLocalPlayerData()
    {
        auto* reader = GetMemoryReader();
        if (!reader || !m_localPlayerAddrs.base)
        {
            m_localPlayerData.valid = false;
            return;
        }

        m_localPlayerData.baseAddress = m_localPlayerAddrs.base;

        ScatterReadBuilder scatter(reader);

        scatter.AddRaw(m_localPlayerAddrs.base + m_localPlayerAddrs.posOffset,
                       m_localPlayerData.position, sizeof(m_localPlayerData.position));

        scatter.AddRaw(m_localPlayerAddrs.base + m_localPlayerAddrs.anglesOffset,
                       m_localPlayerData.viewAngles, sizeof(m_localPlayerData.viewAngles));

        scatter.Add(m_localPlayerAddrs.base + m_localPlayerAddrs.teamOffset,
                    &m_localPlayerData.team);

        size_t success = scatter.Execute();

        m_localPlayerData.valid = (success > 0);
        m_localPlayerValidFrame = m_frameNumber;
    }

    //=========================================================================
    // Entity Data
    //=========================================================================

    const CachedEntityData* MemoryCache::GetEntityData(int index)
    {
        if (index < 0 || index >= MAX_CACHED_ENTITIES)
            return nullptr;

        if (m_entityValidFrame[index] >= m_frameNumber && m_entityData[index].isValid)
        {
            m_stats.cacheHits++;
            return &m_entityData[index];
        }

        // Entity not cached - return nullptr
        // Use UpdateAllEntities() to batch-read entities
        return nullptr;
    }

    void MemoryCache::UpdateAllEntities(const std::vector<uintptr_t>& entityAddresses)
    {
        auto* reader = GetMemoryReader();
        if (!reader || entityAddresses.empty())
            return;

        // Build scatter read for all entities
        ScatterReadBuilder scatter(reader);

        size_t count = std::min(entityAddresses.size(), (size_t)MAX_CACHED_ENTITIES);

        for (size_t i = 0; i < count; i++)
        {
            if (entityAddresses[i] == 0)
                continue;

            auto& entity = m_entityData[i];
            entity.baseAddress = entityAddresses[i];

            // Add reads for each entity field
            scatter.AddRaw(entityAddresses[i] + m_entityOffsets.position,
                          entity.position, sizeof(entity.position));

            scatter.Add(entityAddresses[i] + m_entityOffsets.health, &entity.health);
            scatter.Add(entityAddresses[i] + m_entityOffsets.team, &entity.team);
            scatter.Add(entityAddresses[i] + m_entityOffsets.alive, &entity.isAlive);
        }

        // Execute batch read - THIS IS THE KEY FOR DMA PERFORMANCE!
        scatter.Execute();

        // Mark entities as valid
        for (size_t i = 0; i < count; i++)
        {
            if (entityAddresses[i] != 0)
            {
                m_entityData[i].isValid = true;
                m_entityData[i].clientNum = static_cast<int>(i);
                m_entityValidFrame[i] = m_frameNumber;
            }
        }
    }

    void MemoryCache::InvalidateEntity(int index)
    {
        if (index >= 0 && index < MAX_CACHED_ENTITIES)
        {
            m_entityData[index].isValid = false;
            m_entityValidFrame[index] = 0;
        }
    }

    void MemoryCache::InvalidateAllEntities()
    {
        for (int i = 0; i < MAX_CACHED_ENTITIES; i++)
        {
            m_entityData[i].isValid = false;
        }
        m_entityValidFrame.fill(0);
    }

    void MemoryCache::SetEntityOffsets(size_t posOffset, size_t healthOffset,
                                       size_t teamOffset, size_t aliveOffset)
    {
        m_entityOffsets.position = posOffset;
        m_entityOffsets.health = healthOffset;
        m_entityOffsets.team = teamOffset;
        m_entityOffsets.alive = aliveOffset;
        InvalidateAllEntities();
    }

    void MemoryCache::ReadEntityData(int index, uintptr_t baseAddress)
    {
        if (index < 0 || index >= MAX_CACHED_ENTITIES || baseAddress == 0)
            return;

        auto* reader = GetMemoryReader();
        if (!reader)
            return;

        auto& entity = m_entityData[index];
        entity.baseAddress = baseAddress;

        ScatterReadBuilder scatter(reader);
        scatter.AddRaw(baseAddress + m_entityOffsets.position, entity.position, sizeof(entity.position));
        scatter.Add(baseAddress + m_entityOffsets.health, &entity.health);
        scatter.Add(baseAddress + m_entityOffsets.team, &entity.team);
        scatter.Add(baseAddress + m_entityOffsets.alive, &entity.isAlive);

        scatter.Execute();

        entity.isValid = true;
        entity.clientNum = index;
        m_entityValidFrame[index] = m_frameNumber;
    }

    //=========================================================================
    // Statistics
    //=========================================================================

    void MemoryCache::ResetStats()
    {
        memset(&m_stats, 0, sizeof(m_stats));
    }

} // namespace memory
