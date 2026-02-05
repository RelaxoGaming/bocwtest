#pragma once
#include "build_config.h"

//=============================================================================
// APPLICATION MAIN HEADER
//
// Provides unified initialization for both DLL and EXE modes.
// Contains shared startup logic that both entry points use.
//=============================================================================

namespace app
{
    //-------------------------------------------------------------------------
    // Initialization
    //-------------------------------------------------------------------------

    // Initialize the application (called from both DllMain and WinMain)
    // Returns true on success
    bool Initialize();

    // Shutdown and cleanup
    void Shutdown();

    //-------------------------------------------------------------------------
    // Mode Information
    //-------------------------------------------------------------------------

    // Check if running in DLL mode (injected into target)
    bool IsDllMode();

    // Check if running in EXE mode (standalone)
    bool IsExeMode();

    // Check if using DMA memory access
    bool IsDmaMode();

    //-------------------------------------------------------------------------
    // Main Loop (EXE mode only)
    //-------------------------------------------------------------------------

#ifdef BUILD_EXE
    // Run the main application loop (EXE mode)
    // Returns exit code
    int RunMainLoop();

    // Create overlay window for standalone rendering
    bool CreateOverlayWindow(int width, int height);

    // Destroy overlay window
    void DestroyOverlayWindow();
#endif

    //-------------------------------------------------------------------------
    // Process Window (DLL mode)
    //-------------------------------------------------------------------------

#ifdef BUILD_DLL
    // Get the target process window handle
    HWND GetProcessWindow();
#endif

} // namespace app
