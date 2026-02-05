//=============================================================================
// ENTRY POINTS
//
// This file contains the entry points for both DLL and EXE modes.
// Only one entry point is compiled based on build configuration.
//=============================================================================

#include "includes.h"
#include "build_config.h"
#include "app_main.h"
#include "memory/IMemoryReader.h"
#include "memory/MemoryCache.h"

//=============================================================================
// DLL MODE ENTRY POINT
// Used when injecting into the target process (default)
//=============================================================================

#ifdef BUILD_DLL

static HWND g_hwnd = nullptr;

static BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hWnd, &dwPid);
    if (dwPid == lParam)
    {
        g_hwnd = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND get_process_window()
{
    if (g_hwnd)
        return g_hwnd;

    EnumWindows(EnumWindowCallBack, GetCurrentProcessId());
    return g_hwnd;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
    (void)reserved;

    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        // Initialize memory system
        if (!memory::InitializeMemoryReader(nullptr))
        {
            return FALSE;
        }

        // Set up process-specific data
        ctx::data::base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
        wndproc = get_process_window();

        // Start initialization thread
        utility::I_beginthreadex(0, 0, (_beginthreadex_proc_type)StartPoint::Init, 0, 0, 0);
        break;

    case DLL_PROCESS_DETACH:
        // Cleanup
        memory::ShutdownMemoryReader();
        break;
    }

    return TRUE;
}

#endif // BUILD_DLL

//=============================================================================
// EXE MODE ENTRY POINT
// Used for standalone operation with DMA/MemProcFS (future)
//=============================================================================

#ifdef BUILD_EXE

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    //-------------------------------------------------------------------------
    // Initialize Application
    //-------------------------------------------------------------------------

    if (!app::Initialize())
    {
        MessageBoxA(nullptr, "Failed to initialize application.\n"
                            "Make sure DMA device is connected and target is running.",
                    "Initialization Error", MB_ICONERROR);
        return 1;
    }

    //-------------------------------------------------------------------------
    // Create Overlay Window
    //-------------------------------------------------------------------------

    // Get screen dimensions (or use target window size)
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (!app::CreateOverlayWindow(screenWidth, screenHeight))
    {
        MessageBoxA(nullptr, "Failed to create overlay window.",
                    "Window Error", MB_ICONERROR);
        app::Shutdown();
        return 1;
    }

    //-------------------------------------------------------------------------
    // Initialize DirectX Rendering (standalone)
    //-------------------------------------------------------------------------

    // TODO: Initialize DirectX 12 for standalone rendering
    // This will be different from the hooked version in d3d.cpp
    // Need to create own device, swap chain, etc.

    //-------------------------------------------------------------------------
    // Initialize Game Data
    //-------------------------------------------------------------------------

    // Run pattern scanning and offset initialization
    // Note: In DMA mode, this reads from remote memory!
    StartPoint::Init();

    //-------------------------------------------------------------------------
    // Run Main Loop
    //-------------------------------------------------------------------------

    int exitCode = app::RunMainLoop();

    //-------------------------------------------------------------------------
    // Cleanup
    //-------------------------------------------------------------------------

    app::Shutdown();

    return exitCode;
}

#endif // BUILD_EXE
