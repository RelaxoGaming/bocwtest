#include "includes.h"

namespace ctx
{
	bocw cw = bocw{};
	Client_t client = Client_t{};
	mem_t mem = mem_t{};
}

HWND wndproc = 0;


template<typename T>
static void SafeRelease(T*& res) {
	if (res)
		res->Release();
	res = NULL;
}
extern  LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx12/main.cpp
struct FrameContext {
	CComPtr<ID3D12CommandAllocator> command_allocator = NULL;
	CComPtr<ID3D12Resource> main_render_target_resource = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
};

// Data
static std::vector<FrameContext> g_FrameContext;
static UINT						g_FrameBufferCount = 0;

static CComPtr<ID3D12DescriptorHeap> g_pD3DRtvDescHeap = NULL;
static CComPtr<ID3D12DescriptorHeap> g_pD3DSrvDescHeap = NULL;
static CComPtr<ID3D12CommandQueue> g_pD3DCommandQueue = NULL;
static CComPtr<ID3D12GraphicsCommandList> g_pD3DCommandList = NULL;

LRESULT APIENTRY WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef long(__fastcall* Present) (IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags);
static Present OriginalPresent;

typedef void(*ExecuteCommandLists)(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists);
static ExecuteCommandLists OriginalExecuteCommandLists;

typedef long(__fastcall* ResizeBuffers)(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
static ResizeBuffers OriginalResizeBuffers;

static WNDPROC OriginalWndProc;
static HWND Window = nullptr;
IDXGISwapChain3* g_pSwapChain;
static uint64_t* g_MethodsTable = NULL;
static bool g_Initialized = false;
bool init_hook = false;



LRESULT APIENTRY WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	
	
	if (msg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		g_menu::b_menu_open = !g_menu::b_menu_open;
	}
	
	if (g_Initialized) {

		if (g_menu::b_menu_open)
		{
			ImGuiIO& io = ImGui::GetIO();
			switch (msg) {
			case WM_LBUTTONDOWN:
				io.MouseDown[0] = true;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_LBUTTONUP:
				io.MouseDown[0] = false;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_RBUTTONDOWN:
				io.MouseDown[1] = true;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_RBUTTONUP:
				io.MouseDown[1] = false;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_MBUTTONDOWN:
				io.MouseDown[2] = true;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_MBUTTONUP:
				io.MouseDown[2] = false;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_MOUSEWHEEL:
				io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_MOUSEMOVE:
				io.MousePos.x = (signed short)(lParam);
				io.MousePos.y = (signed short)(lParam >> 16);
				return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_KEYDOWN:
				if (wParam < 256)
					io.KeysDown[wParam] = 1;
				return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_KEYUP:
				if (wParam < 256)
					io.KeysDown[wParam] = 0;
				return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			case WM_CHAR:
				// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
				if (wParam > 0 && wParam < 0x10000)
					io.AddInputCharacter((unsigned short)wParam);
				return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
			}
		}
		else
		{
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

		}





	}
	return CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
}



bool init_d3d12()
{
	if (g_Initialized)
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		return true;
	}


	if (!g_Initialized) {
		log::NotePad("inside veh and settings up renderer\n");
		ID3D12Device* pD3DDevice;
		IDXGISwapChain3* pSwapChain = g_pSwapChain;
		if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&pD3DDevice))) {
			return false;
		}

		{
			DXGI_SWAP_CHAIN_DESC desc;
			pSwapChain->GetDesc(&desc);
			Window = desc.OutputWindow;
			if (!OriginalWndProc) {
				OriginalWndProc = (WNDPROC)SetWindowLongPtr(Window, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
			}
			g_FrameBufferCount = desc.BufferCount;
			g_FrameContext.clear();
			g_FrameContext.resize(g_FrameBufferCount);
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = g_FrameBufferCount;  // increase for textures
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			if (pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3DSrvDescHeap)) != S_OK) {
				return false;
			}
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = g_FrameBufferCount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;

			if (pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3DRtvDescHeap)) != S_OK) {
				return false;
			}

			const auto rtvDescriptorSize = pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pD3DRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

			for (UINT i = 0; i < g_FrameBufferCount; i++) {

				g_FrameContext[i].main_render_target_descriptor = rtvHandle;
				pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_FrameContext[i].main_render_target_resource));
				pD3DDevice->CreateRenderTargetView(g_FrameContext[i].main_render_target_resource, nullptr, rtvHandle);
				rtvHandle.ptr += rtvDescriptorSize;
			}

		}

		{
			ID3D12CommandAllocator* allocator;
			if (pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK) {
				return false;
			}

			for (size_t i = 0; i < g_FrameBufferCount; i++) {
				if (pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_FrameContext[i].command_allocator)) != S_OK) {
					return false;
				}
			}

			if (pD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_FrameContext[0].command_allocator, NULL, IID_PPV_ARGS(&g_pD3DCommandList)) != S_OK || g_pD3DCommandList->Close() != S_OK) {
				return false;
			}
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();




		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(Window);
		ImGui_ImplDX12_Init(pD3DDevice, g_FrameBufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM, g_pD3DSrvDescHeap,
			g_pD3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_pD3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart());


		ImGuiIO& io = ImGui::GetIO();

		/*g_menu::logo_image = InitTexture((stbi_uc*)&logo, sizeof(logo), pD3DDevice, g_pD3DSrvDescHeap, 1);
		g_menu::target_icon = InitTexture((stbi_uc*)&target, sizeof(target), pD3DDevice, g_pD3DSrvDescHeap, 2);
		g_menu::visual_icon = InitTexture((stbi_uc*)&visual, sizeof(visual), pD3DDevice, g_pD3DSrvDescHeap, 3);
		g_menu::misc_icon = InitTexture((stbi_uc*)&misc, sizeof(misc), pD3DDevice, g_pD3DSrvDescHeap, 4);
		g_menu::config_icon = InitTexture((stbi_uc*)&config, sizeof(config), pD3DDevice, g_pD3DSrvDescHeap, 5);*/

		g_menu::menu_font = io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Tahoma.ttf"), 18.0f);


		pD3DDevice->Release();
		g_Initialized = true;
		log::NotePad("done\n");
	}

	return true;
}

void cache()
{
	if (!g_Initialized)
		return;

	ImGui::EndFrame();
	ImGui::Render();
}

void RenderBegin()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
}


void RenderEnd()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
}

void __stdcall hk_wow64_prepare_for_exception(PEXCEPTION_RECORD exception_record, PCONTEXT context)
{



	std::unique_lock< std::mutex > lock_think(hook_mutex);
	{


		if (*ctx::offset::DVARBOOL_cg_draw2d.cast<uintptr_t*>() != 0xDeadBeaB)
		{
			// cl_packetdup
			orig = *ctx::offset::DVARBOOL_cg_draw2d.cast<uintptr_t*>();
			*ctx::offset::DVARBOOL_cg_draw2d.cast<uintptr_t*>() = 0xDeadBeaB;
		}


	
	
	}
	lock_think.unlock();



	if (exception_record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && exception_record->ExceptionInformation[1] >= 0xDeadBeaB && exception_record->ExceptionInformation[1] <= 0xDeadBeaB + 0x1000)

	{
		//log::NotePad("packet: %x", exception_record->ExceptionInformation[1]);

		//ctx::config::logging::exception = exception_record->ExceptionInformation[1];
		/// Fix all registers.




		for (uintptr_t* registry_value = &context->Rax; registry_value <= &context->R15; registry_value++)
		{
			const auto delta = *registry_value - 0xDeadBeaB;
			if (delta > 0x100)
				continue;

			*registry_value = orig + delta;
		}


		if (!g_Initialized)
		{
			init_d3d12();
		}

		//ctx::client.cmd = GameFunction::CL_GetCurrentUsercmd(0, GameFunction::CL_GetCmdNumber(0));

		::D3D12::DrawCB([]() -> void {

			visuals::draw();
			g_menu::Menu();

		});
	


		*(uintptr_t*)_AddressOfReturnAddress() += 0x12;

		
	}





	


}



namespace D3D12 {





	uintptr_t original{};


	bool b_panic = false;
	//Timer timer = Timer();
	bool start_timer = false;
	bool is_hooked = false;


	long __fastcall HookPresent2(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {


		g_pSwapChain = pSwapChain;
		g_pD3DCommandQueue = *reinterpret_cast<decltype(g_pD3DCommandQueue)*>((std::uintptr_t)pSwapChain + 0x118);




		if (!g_Initialized) {
			ID3D12Device* pD3DDevice;

			if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&pD3DDevice))) {
				return OriginalPresent(pSwapChain, SyncInterval, Flags);
			}

			{
				DXGI_SWAP_CHAIN_DESC desc;
				pSwapChain->GetDesc(&desc);
				Window = desc.OutputWindow;
				if (!OriginalWndProc) {
					OriginalWndProc = (WNDPROC)SetWindowLongPtr(Window, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
				}
				g_FrameBufferCount = desc.BufferCount;
				g_FrameContext.clear();
				g_FrameContext.resize(g_FrameBufferCount);
			}

			{
				D3D12_DESCRIPTOR_HEAP_DESC desc = {};
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				desc.NumDescriptors = g_FrameBufferCount;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				if (pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3DSrvDescHeap)) != S_OK) {
					return OriginalPresent(pSwapChain, SyncInterval, Flags);
				}
			}

			{
				D3D12_DESCRIPTOR_HEAP_DESC desc;
				desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				desc.NumDescriptors = g_FrameBufferCount;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				desc.NodeMask = 1;

				if (pD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pD3DRtvDescHeap)) != S_OK) {
					return OriginalPresent(pSwapChain, SyncInterval, Flags);
				}

				const auto rtvDescriptorSize = pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pD3DRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

				for (UINT i = 0; i < g_FrameBufferCount; i++) {

					g_FrameContext[i].main_render_target_descriptor = rtvHandle;
					pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_FrameContext[i].main_render_target_resource));
					pD3DDevice->CreateRenderTargetView(g_FrameContext[i].main_render_target_resource, nullptr, rtvHandle);
					rtvHandle.ptr += rtvDescriptorSize;
				}

			}

			{
				ID3D12CommandAllocator* allocator;
				if (pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK) {
					return OriginalPresent(pSwapChain, SyncInterval, Flags);
				}

				for (size_t i = 0; i < g_FrameBufferCount; i++) {
					if (pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_FrameContext[i].command_allocator)) != S_OK) {
						return OriginalPresent(pSwapChain, SyncInterval, Flags);
					}
				}

				if (pD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_FrameContext[0].command_allocator, NULL, IID_PPV_ARGS(&g_pD3DCommandList)) != S_OK || g_pD3DCommandList->Close() != S_OK) {
					return OriginalPresent(pSwapChain, SyncInterval, Flags);
				}
			}

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();






			// Setup Dear ImGui style

			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(Window);
			ImGui_ImplDX12_Init(pD3DDevice, g_FrameBufferCount,
				DXGI_FORMAT_R8G8B8A8_UNORM, g_pD3DSrvDescHeap,
				g_pD3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
				g_pD3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart());


			//InitTextures(pD3DDevice, g_pD3DSrvDescHeap);
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->AddFontDefault();
			g_menu::menu_font = io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Tahoma.ttf"), 18.0f);

			g_Initialized = true;

			pD3DDevice->Release();
		}





		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX12_NewFrame();
		ImGui::NewFrame();


		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("##A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		{

			g_menu::Menu();
			visuals::draw();
		}
		ImGui::End();


		ImGui::EndFrame();


		FrameContext& currentFrameContext = g_FrameContext[pSwapChain->GetCurrentBackBufferIndex()];
		currentFrameContext.command_allocator->Reset();

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		g_pD3DCommandList->Reset(currentFrameContext.command_allocator, nullptr);
		g_pD3DCommandList->ResourceBarrier(1, &barrier);
		g_pD3DCommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
		g_pD3DCommandList->SetDescriptorHeaps(1, &g_pD3DSrvDescHeap);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pD3DCommandList);
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		g_pD3DCommandList->ResourceBarrier(1, &barrier);
		g_pD3DCommandList->Close();

		g_pD3DCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&g_pD3DCommandList);
		return OriginalPresent(pSwapChain, SyncInterval, Flags);
	}




	long __fastcall HookPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {


		//DXGI_SWAP_CHAIN_DESC desc;
		//pSwapChain->GetDesc(&desc);
		//if (GetForegroundWindow() != desc.OutputWindow)
		//	return OriginalPresent(pSwapChain, SyncInterval, Flags);

		g_pSwapChain = pSwapChain;
		g_pD3DCommandQueue = *reinterpret_cast<decltype(g_pD3DCommandQueue)*>((std::uintptr_t)pSwapChain + 0x118);


		std::unique_lock< std::recursive_mutex > lock_render(render_hook);
		{
			if (g_Initialized)
			{
				FrameContext& currentFrameContext = g_FrameContext[pSwapChain->GetCurrentBackBufferIndex()];
				currentFrameContext.command_allocator->Reset();

				D3D12_RESOURCE_BARRIER barrier;
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
				barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
				g_pD3DCommandList->Reset(currentFrameContext.command_allocator, nullptr);
				g_pD3DCommandList->ResourceBarrier(1, &barrier);
				g_pD3DCommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
				g_pD3DCommandList->SetDescriptorHeaps(1, &g_pD3DSrvDescHeap);
				ImGui::Render();
				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pD3DCommandList);
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				g_pD3DCommandList->ResourceBarrier(1, &barrier);
				g_pD3DCommandList->Close();
				g_pD3DCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&g_pD3DCommandList);
			}

		}
		lock_render.unlock();



		return OriginalPresent(pSwapChain, SyncInterval, Flags);
	}





	void HookExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) {
		if (!g_pD3DCommandQueue) {
			g_pD3DCommandQueue = queue;
		}

		OriginalExecuteCommandLists(queue, NumCommandLists, ppCommandLists);
	}

	void ResetState() {

		if (g_Initialized) {
			g_Initialized = false;
			ImGui_ImplWin32_Shutdown();
			ImGui_ImplDX12_Shutdown();
			init_hook = false;
		}

		g_pD3DCommandQueue = nullptr;
		g_FrameContext.clear();
		g_pD3DCommandList = nullptr;
		g_pD3DRtvDescHeap = nullptr;
		g_pD3DSrvDescHeap = nullptr;



	}

	long HookResizeBuffers(IDXGISwapChain3* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{

		HRESULT ret = S_OK;


		ResetState();
		return  OriginalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	}

	Status Initd3d12() {
		WNDCLASSEX windowClass;
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = DefWindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = GetModuleHandle(NULL);
		windowClass.hIcon = NULL;
		windowClass.hCursor = NULL;
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = "Fake Window";
		windowClass.hIconSm = NULL;

		::RegisterClassEx(&windowClass);

		HWND window = ::CreateWindow(windowClass.lpszClassName, "Fake DirectX Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

		HMODULE libDXGI;
		HMODULE libD3D12;

		if ((libDXGI = ::GetModuleHandle("dxgi.dll")) == NULL) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::ModuleNotFoundError;
		}

		if ((libD3D12 = ::GetModuleHandle("d3d12.dll")) == NULL) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::ModuleNotFoundError;
		}

		void* CreateDXGIFactory;
		if ((CreateDXGIFactory = ::GetProcAddress(libDXGI, "CreateDXGIFactory")) == NULL) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		CComPtr<IDXGIFactory> factory;
		if (((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&factory) < 0) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		CComPtr<IDXGIAdapter> adapter;
		if (factory->EnumAdapters(0, &adapter) == DXGI_ERROR_NOT_FOUND) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		void* D3D12CreateDevice;
		if ((D3D12CreateDevice = ::GetProcAddress(libD3D12, "D3D12CreateDevice")) == NULL) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		CComPtr<ID3D12Device> device;
		if (((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device) < 0) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;

		CComPtr<ID3D12CommandQueue> commandQueue;
		if (device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue) < 0) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		CComPtr<ID3D12CommandAllocator> commandAllocator;
		if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&commandAllocator) < 0) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		CComPtr<ID3D12GraphicsCommandList> commandList;
		if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&commandList) < 0) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		DXGI_RATIONAL refreshRate;
		refreshRate.Numerator = 60;
		refreshRate.Denominator = 1;

		DXGI_MODE_DESC bufferDesc;
		bufferDesc.Width = 100;
		bufferDesc.Height = 100;
		bufferDesc.RefreshRate = refreshRate;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc = sampleDesc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = window;
		swapChainDesc.Windowed = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		CComPtr<IDXGISwapChain> swapChain;
		if (factory->CreateSwapChain(commandQueue, &swapChainDesc, &swapChain) < 0) {
			::DestroyWindow(window);
			::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
			return Status::UnknownError;
		}

		g_MethodsTable = (uint64_t*)::calloc(150, sizeof(uint64_t));
		::memcpy(g_MethodsTable, *(uint64_t**)(void*)device, 44 * sizeof(uint64_t));
		::memcpy(g_MethodsTable + 44, *(uint64_t**)(void*)commandQueue, 19 * sizeof(uint64_t));
		::memcpy(g_MethodsTable + 44 + 19, *(uint64_t**)(void*)commandAllocator, 9 * sizeof(uint64_t));
		::memcpy(g_MethodsTable + 44 + 19 + 9, *(uint64_t**)(void*)commandList, 60 * sizeof(uint64_t));
		::memcpy(g_MethodsTable + 44 + 19 + 9 + 60, *(uint64_t**)(void*)swapChain, 18 * sizeof(uint64_t));

		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return Status::Success;
	}


	Status Hook(uint16_t _index, void** _original, void* _function) {
		void* target = (void*)g_MethodsTable[_index];
		if (MH_CreateHook(target, _function, _original) != MH_OK || MH_EnableHook(target) != MH_OK) {
			return Status::UnknownError;
		}
		return Status::Success;
	}

	Status Unhook(uint16_t _index, void** _original, void* _function) {
		void* target = (void*)g_MethodsTable[_index];
		MH_DisableHook(target);
		return Status::Success;
	}

	LRESULT APIENTRY WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (g_Initialized) {

			if (g_menu::b_menu_open)
			{
				ImGuiIO& io = ImGui::GetIO();
				switch (msg) {
				case WM_LBUTTONDOWN:
					io.MouseDown[0] = true;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_LBUTTONUP:
					io.MouseDown[0] = false;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_RBUTTONDOWN:
					io.MouseDown[1] = true;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_RBUTTONUP:
					io.MouseDown[1] = false;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_MBUTTONDOWN:
					io.MouseDown[2] = true;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_MBUTTONUP:
					io.MouseDown[2] = false;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_MOUSEWHEEL:
					io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_MOUSEMOVE:
					io.MousePos.x = (signed short)(lParam);
					io.MousePos.y = (signed short)(lParam >> 16);
					return io.WantCaptureMouse ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_KEYDOWN:
					if (wParam < 256)
						io.KeysDown[wParam] = 1;
					return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_KEYUP:
					if (wParam < 256)
						io.KeysDown[wParam] = 0;
					return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				case WM_CHAR:
					// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
					if (wParam > 0 && wParam < 0x10000)
						io.AddInputCharacter((unsigned short)wParam);
					return io.WantCaptureKeyboard ? 0 : CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
				}
			}
			else
			{
				ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

			}





		}
		return CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
	}


	void DrawCB(const std::function<void(void)>& callback)
	{
		std::unique_lock<std::recursive_mutex> render_lock(render_hook);
		{
			init_d3d12();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::Begin("##A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
			
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->DrawList->PushClipRectFullScreen();
			{
				std::invoke(callback);
			}
			ImGui::End();
			cache();
		}
		render_lock.unlock();
	}

	Status InstallHooks() {
		//auto addr =mem::find_ida_sig("DiscordHook64.dll", "41 89 f0 ff 15 ? ? ? ? 89 c6 E8 ? ? ? ? E8"); /*(PVOID)Scanner::Scanner(xorstr("DiscordHook64.dll"), xorstr("41 89 f0 ff 15 ? ? ? ? 89 c6 E8 ? ? ? ? E8"), xorstr("Discord"), 3).vtable_call();*/
		//if (addr)
		//{
		//	addr = addr.self_offset(3).self_jmp(0x1);
		//	//const auto presentptr = (PVOID)Scanner::Scanner(addr).vtable_call();
		//	*(PVOID*)&OriginalPresent = _InterlockedExchangePointer(addr.cast<PVOID*>(), &HookPresent);

		//}
		//else
		{
			Hook(140, (void**)&OriginalPresent, HookPresent);
			Hook(145, (void**)&OriginalResizeBuffers, HookResizeBuffers);
		}
		return Status::Success;
	}

	Status RemoveHooks() {
		//Unhook(54, (void**)&OriginalExecuteCommandLists, HookExecuteCommandLists);
		Unhook(140, (void**)&OriginalPresent, HookPresent);
		Unhook(145, (void**)&OriginalResizeBuffers, HookResizeBuffers);

		if (Window && OriginalWndProc) {
			SetWindowLongPtr(Window, GWLP_WNDPROC, (__int3264)(LONG_PTR)OriginalWndProc);
		}

		ResetState();

		ImGui::DestroyContext();

		//wait for hooks to finish if in one. maybe not needed, but seemed more stable after adding it.
		Sleep(1000);
		return Status::Success;
	}
}