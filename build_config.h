#pragma once

//=============================================================================
// BUILD CONFIGURATION
//
// This file controls the build mode and memory access method.
// Change these defines to switch between DLL and EXE modes.
//=============================================================================

//-----------------------------------------------------------------------------
// BUILD MODE SELECTION
// Only ONE of these should be defined at a time!
//-----------------------------------------------------------------------------

// DLL Mode (Default): Inject into target process
// - Uses DirectX hooks for overlay
// - Direct memory access within process
// - Single PC setup
#define BUILD_DLL

// EXE Mode: Standalone application on separate PC
// - Creates own overlay window
// - Uses MemProcFS for remote memory access
// - Two PC setup (Target PC + Overlay PC)
// #define BUILD_EXE

//-----------------------------------------------------------------------------
// MEMORY ACCESS METHOD
// Only relevant when BUILD_EXE is defined
//-----------------------------------------------------------------------------

// Enable MemProcFS support for DMA-based memory access
// Requires: vmmdll.h, vmmdll.lib, and DMA hardware
// #define USE_MEMPROCFS

//-----------------------------------------------------------------------------
// MEMORY READER SELECTION (compile-time)
//-----------------------------------------------------------------------------

#ifdef BUILD_DLL
    // DLL mode always uses local memory reader
    #define USE_LOCAL_MEMORY_READER
#endif

#ifdef BUILD_EXE
    #ifdef USE_MEMPROCFS
        #define USE_MEMPROCFS_READER
    #else
        // Fallback to local reader for testing EXE mode without DMA
        #define USE_LOCAL_MEMORY_READER
    #endif
#endif

//-----------------------------------------------------------------------------
// PERFORMANCE SETTINGS
//-----------------------------------------------------------------------------

// Maximum entries per scatter read batch (optimal: 50-100)
#define SCATTER_BATCH_SIZE 64

// Frame cache lifetime in frames (1 = invalidate every frame)
#define CACHE_LIFETIME_FRAMES 1

// Maximum cached entities
#define MAX_CACHED_ENTITIES 128

// Enable read statistics for debugging
// #define MEMORY_READ_STATS

//-----------------------------------------------------------------------------
// DEBUG SETTINGS
//-----------------------------------------------------------------------------

#ifdef _DEBUG
    #define MEMORY_READ_STATS
    #define LOG_SCATTER_READS
#endif

//-----------------------------------------------------------------------------
// VALIDATION
//-----------------------------------------------------------------------------

#if defined(BUILD_DLL) && defined(BUILD_EXE)
    #error "Cannot define both BUILD_DLL and BUILD_EXE!"
#endif

#if !defined(BUILD_DLL) && !defined(BUILD_EXE)
    #error "Must define either BUILD_DLL or BUILD_EXE!"
#endif
