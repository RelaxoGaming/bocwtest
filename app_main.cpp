#include "includes.h"
#include "app_main.h"
#include "memory/IMemoryReader.h"
#include "memory/MemoryCache.h"

namespace app
{
    //=========================================================================
    // Internal State
    //=========================================================================

    static bool g_initialized = false;

#ifdef BUILD_EXE
    static HWND g_overlayWindow = nullptr;
    static bool g_running = false;
#endif

#ifdef BUILD_DLL
    static HWND g_processWindow = nullptr;

    // Window enumeration callback
    static BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lParam)
    {
        DWORD dwPid = 0;
        GetWindowThreadProcessId(hWnd, &dwPid);
        if (dwPid == lParam)
        {
            g_processWindow = hWnd;
            return FALSE;
        }
        return TRUE;
    }
#endif

    //=========================================================================
    // Initialization
    //=========================================================================

    bool Initialize()
    {
        if (g_initialized)
            return true;

        //---------------------------------------------------------------------
        // Initialize Memory Reader
        //---------------------------------------------------------------------

#ifdef BUILD_DLL
        // DLL Mode: Initialize local memory reader
        if (!memory::InitializeMemoryReader(nullptr))
        {
            OutputDebugStringA("[App] Failed to initialize memory reader\n");
            return false;
        }

        // Set module base from current process
        ctx::data::base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
#endif

#ifdef BUILD_EXE
        // EXE Mode: Initialize with target process
    #ifdef USE_MEMPROCFS
        // MemProcFS mode - connect to DMA device and find target
        const char* initString = "fpga://algo=0|BlackOpsColdWar.exe";
        if (!memory::InitializeMemoryReader(initString))
        {
            OutputDebugStringA("[App] Failed to initialize MemProcFS reader\n");
            return false;
        }
    #else
        // Testing mode - use local reader (must run on same PC as target)
        if (!memory::InitializeMemoryReader("BlackOpsColdWar.exe"))
        {
            OutputDebugStringA("[App] Failed to initialize memory reader\n");
            return false;
        }
    #endif

        // Get module base from reader
        ctx::data::base = memory::GetMemoryReader()->GetModuleBase();
#endif

        //---------------------------------------------------------------------
        // Initialize Memory Cache
        //---------------------------------------------------------------------

        // Set up camera addresses (these will be populated by game-specific code)
        // memory::Cache().SetCameraAddresses(...);

        // Set up entity offsets
        // memory::Cache().SetEntityOffsets(...);

        //---------------------------------------------------------------------
        // Game-Specific Initialization
        //---------------------------------------------------------------------

        // Call the existing initialization (pattern scanning, etc.)
        // This is done in StartPoint::Init() for DLL mode

        g_initialized = true;
        return true;
    }

    void Shutdown()
    {
        if (!g_initialized)
            return;

#ifdef BUILD_EXE
        g_running = false;
        DestroyOverlayWindow();
#endif

        memory::ShutdownMemoryReader();
        g_initialized = false;
    }

    //=========================================================================
    // Mode Information
    //=========================================================================

    bool IsDllMode()
    {
#ifdef BUILD_DLL
        return true;
#else
        return false;
#endif
    }

    bool IsExeMode()
    {
#ifdef BUILD_EXE
        return true;
#else
        return false;
#endif
    }

    bool IsDmaMode()
    {
#ifdef USE_MEMPROCFS
        return true;
#else
        return false;
#endif
    }

    //=========================================================================
    // DLL Mode Functions
    //=========================================================================

#ifdef BUILD_DLL
    HWND GetProcessWindow()
    {
        if (g_processWindow)
            return g_processWindow;

        EnumWindows(EnumWindowCallback, GetCurrentProcessId());
        return g_processWindow;
    }
#endif

    //=========================================================================
    // EXE Mode Functions
    //=========================================================================

#ifdef BUILD_EXE
    // Window procedure for overlay
    static LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_DESTROY:
            g_running = false;
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                g_running = false;
                DestroyWindow(hWnd);
            }
            return 0;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    bool CreateOverlayWindow(int width, int height)
    {
        // Register window class
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = OverlayWndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"DMAOverlay";

        if (!RegisterClassEx(&wc))
            return false;

        // Create transparent overlay window
        g_overlayWindow = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            L"DMAOverlay",
            L"Overlay",
            WS_POPUP,
            0, 0, width, height,
            nullptr, nullptr, wc.hInstance, nullptr
        );

        if (!g_overlayWindow)
            return false;

        // Make window click-through
        SetLayeredWindowAttributes(g_overlayWindow, RGB(0, 0, 0), 0, LWA_COLORKEY);

        ShowWindow(g_overlayWindow, SW_SHOW);
        UpdateWindow(g_overlayWindow);

        return true;
    }

    void DestroyOverlayWindow()
    {
        if (g_overlayWindow)
        {
            DestroyWindow(g_overlayWindow);
            g_overlayWindow = nullptr;
        }

        UnregisterClass(L"DMAOverlay", GetModuleHandle(nullptr));
    }

    int RunMainLoop()
    {
        g_running = true;

        MSG msg = {};
        while (g_running)
        {
            // Process Windows messages
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT)
                {
                    g_running = false;
                    break;
                }
            }

            if (!g_running)
                break;

            //---------------------------------------------------------------------
            // Frame Begin
            //---------------------------------------------------------------------
            memory::Cache().BeginFrame();

            //---------------------------------------------------------------------
            // Memory Reading Phase
            // All DMA reads should happen here, before rendering
            //---------------------------------------------------------------------

            // Update cached data
            // This is where scatter reads should be used for efficiency
            // memory::Cache().GetCameraData();  // Updates camera cache
            // memory::Cache().UpdateAllEntities(...);  // Batch read entities

            //---------------------------------------------------------------------
            // Rendering Phase
            // Use cached data only - no memory reads here!
            //---------------------------------------------------------------------

            // TODO: Implement standalone DirectX rendering here
            // For now, this is a placeholder

            //---------------------------------------------------------------------
            // Frame End
            //---------------------------------------------------------------------
            memory::Cache().EndFrame();

            // Frame rate limiting
            Sleep(16);  // ~60 FPS
        }

        return 0;
    }
#endif

} // namespace app
