#pragma once

extern HWND wndproc;



namespace D3D12 {

    enum class Status {
        UnknownError = -1,
        NotSupportedError = -2,
        ModuleNotFoundError = -3,

        AlreadyInitializedError = -4,
        NotInitializedError = -5,

        Success = 0,
    };

    Status Initd3d12();
    extern bool CreateDeviceD3D();
    extern void DrawCB(const std::function<void(void)>& callback);
    Status InstallHooks();
    Status RemoveHooks();
}



















