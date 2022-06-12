#include "D3D9Window.hpp"
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef BOOL(WINAPI* hk_SetCursorPos)(int, int);
hk_SetCursorPos origSetCursorPos = NULL;

typedef BOOL(WINAPI* hk_ShowCursor)(bool);
hk_ShowCursor origShowCursor = NULL;


typedef BOOL(WINAPI* hk_GetCursorPos)(LPPOINT);
hk_GetCursorPos origGetCursorPos = NULL;

namespace DX9Base {
	static unsigned int* MethodsTable;

#pragma region	//	CURSOR HOOKS

	/// <summary>
	/// Hooked Functions
	/// We redirect the execution of each function based on whether our menu is shown or not
	/// These are basic tests and will need to be configured on a game to game basis

	/// Moves the cursor to the specified screen coordinates. 
	/// If the new coordinates are not within the screen rectangle set by the most recent ClipCursor function call, 
	/// the system automatically adjusts the coordinates so that the cursor stays within the rectangle.
	BOOL WINAPI SetCursorPos_hook(int X, int Y)
	{
		if (g_GameVariables->m_ShowMenu)
			return TRUE;

		return origSetCursorPos(X, Y);
	}

	/// Displays or hides the cursor
	BOOL WINAPI ShowCursor_hook(bool Show)
	{
		if (g_GameVariables->m_ShowMenu)
			return FALSE;
		
		return origShowCursor(Show);
	}

	/// Retrieves the position of the mouse cursor, in screen coordinates.
	BOOL WINAPI GetCursorPos_hook(LPPOINT lpPoint)
	{
		if (g_GameVariables->m_ShowMenu)
			return FALSE;
		
		return TRUE;
	}

	/// <summary>
	/// The following functions just hook their respective functions
	/// I made a wrapper for this a little further down (bool CreateFunctionHook();)
	bool GetCursor()
	{
		if (MH_CreateHook(&GetCursorPos, &GetCursorPos_hook, reinterpret_cast<LPVOID*>(&origGetCursorPos)) != MH_OK)
			return FALSE;

		if (MH_EnableHook(&GetCursorPos) != MH_OK)
			return FALSE;

		return TRUE;
	}

	bool HookCursor()
	{
		if (MH_CreateHook(&SetCursorPos, &SetCursorPos_hook, reinterpret_cast<LPVOID*>(&origSetCursorPos)) != MH_OK)
			return FALSE;

		if (MH_EnableHook(&SetCursorPos) != MH_OK)
			return FALSE;

		return TRUE;
	}

	bool ShowCursor()
	{
		if (MH_CreateHook(&ShowCursor, &ShowCursor_hook, reinterpret_cast<LPVOID*>(&origShowCursor)) != MH_OK)
			return FALSE;

		if (MH_EnableHook(&ShowCursor) != MH_OK)
			return FALSE;

		return TRUE;
	}

	//	Special ImGui Attmpt
	static void ImGui_ImplWin32_UpdateMousePos()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
		if (io.WantSetMousePos)
		{
			POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
			::ClientToScreen(g_D3D9Window->m_GameWindowHandle, &pos);
			::SetCursorPos(pos.x, pos.y);
		}

		// Set mouse position
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		POINT pos;

		// Getting active window handle from the window threads processID # Bacause ::GetActiveWindow() fails in some cases.
		GUITHREADINFO guiThreadInfo; guiThreadInfo.cbSize = sizeof(GUITHREADINFO);
		GetGUIThreadInfo(GetWindowThreadProcessId(g_D3D9Window->m_GameWindowHandle, 0), &guiThreadInfo);

		if (guiThreadInfo.hwndActive == g_D3D9Window->m_GameWindowHandle && ::GetCursorPos(&pos))
			if (::ScreenToClient(g_D3D9Window->m_GameWindowHandle, &pos))
				io.MousePos = ImVec2((float)pos.x, (float)pos.y);
	}
#pragma endregion

	/// <summary>
	/// Window Proc Message Handler
	LRESULT APIENTRY D3D9Window::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		//	CHECK IF MENU IS SHOWN
		if (g_GameVariables->m_ShowMenu) {
			//	PASS INPUT TO IMGUI
			ImGui_ImplWin32_WndProcHandler((HWND)hwnd, uMsg, wParam, lParam);
			return TRUE;	//	BLOCK INPUTS TO GAME WINDOW
		}

		//	PASS INPUTS TO GAME MENU
		return CallWindowProc((WNDPROC)g_D3D9Window->m_OldWndProc, hwnd, uMsg, wParam, lParam);
	}

	///	WINDOW HOOKS

	/// MAIN RENDER LOOP
	HRESULT APIENTRY D3D9Window::MJEndScene(IDirect3DDevice9* pDevice) {
		if (g_Killswitch) {
			g_Hooks->Unhook();
			g_D3D9Window->oEndScene(pDevice);
			g_Running = FALSE;
			return 0;
		}
		g_D3D9Window->Overlay(pDevice);
		return g_D3D9Window->oEndScene(pDevice);
	}

	HRESULT APIENTRY D3D9Window::MJPresent(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
		return g_D3D9Window->oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	HRESULT APIENTRY D3D9Window::MJDrawIndexedPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
		return g_D3D9Window->oDrawIndexedPrimitive(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	}

	HRESULT APIENTRY D3D9Window::MJDrawPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
		return g_D3D9Window->oDrawPrimitive(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
	}

	HRESULT APIENTRY D3D9Window::MJSetTexture(LPDIRECT3DDEVICE9 pDevice, DWORD Sampler, IDirect3DBaseTexture9* pTexture) {
		return g_D3D9Window->oSetTexture(pDevice, Sampler, pTexture);
	}

	HRESULT APIENTRY D3D9Window::MJReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
		return g_D3D9Window->oReset(pDevice, pPresentationParameters);;
	}

	HRESULT APIENTRY D3D9Window::MJSetStreamSource(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sStride) {
		return g_D3D9Window->oSetStreamSource(pDevice, StreamNumber, pStreamData, OffsetInBytes, sStride);
	}

	HRESULT APIENTRY D3D9Window::MJSetVertexDeclaration(LPDIRECT3DDEVICE9 pDevice, IDirect3DVertexDeclaration9* pdecl) {
		return g_D3D9Window->oSetVertexDeclaration(pDevice, pdecl);
	}

	HRESULT APIENTRY D3D9Window::MJSetVertexShaderConstantF(LPDIRECT3DDEVICE9 pDevice, UINT StartRegister, const float* pConstantData, UINT Vector4fCount) {
		return g_D3D9Window->oSetVertexShaderConstantF(pDevice, StartRegister, pConstantData, Vector4fCount);
	}

	HRESULT APIENTRY D3D9Window::MJSetVertexShader(LPDIRECT3DDEVICE9 pDevice, IDirect3DVertexShader9* veShader) {
		return g_D3D9Window->oSetVertexShader(pDevice, veShader);
	}

	HRESULT APIENTRY D3D9Window::MJSetPixelShader(LPDIRECT3DDEVICE9 pDevice, IDirect3DPixelShader9* piShader) {
		return g_D3D9Window->oSetPixelShader(pDevice, piShader);
	}


	/// <summary>
	/// CLASS FUNCTIONS

	bool D3D9Window::Start()
	{
		if (InitializeDX()) {
			/// <summary>
			/// Initialize all of our hooks
			/// First we start with our window hooks
			CreateWindowHook(42, (void**)&oEndScene, MJEndScene);
			CreateWindowHook(17, (void**)&oPresent, MJPresent);
			CreateWindowHook(82, (void**)&oDrawIndexedPrimitive, MJDrawIndexedPrimitive);
			CreateWindowHook(81, (void**)&oDrawPrimitive, MJDrawPrimitive);
			CreateWindowHook(65, (void**)&oSetTexture, MJSetTexture);
			CreateWindowHook(16, (void**)&oReset, MJReset);
			CreateWindowHook(100, (void**)&oSetStreamSource, MJSetStreamSource);
			CreateWindowHook(87, (void**)&oSetVertexDeclaration, MJSetVertexDeclaration);
			CreateWindowHook(94, (void**)&oSetVertexShaderConstantF, MJSetVertexShaderConstantF);
			CreateWindowHook(92, (void**)&oSetVertexShader, MJSetVertexShader);
			CreateWindowHook(107, (void**)&oSetPixelShader, MJSetPixelShader);

			///FUNCTION HOOKS
			//HookCursor();
			//ShowCursor();
			//GetCursor();
			
			///	WRAPPED FUNCTION HOOKS
			//CreateFunctionHook(SetCursorPos, (void**)&SetCursorPos_hook, origSetCursorPos);
			//CreateFunctionHook(ShowCursor, (void**)&ShowCursor_hook, origShowCursor);

#if DEVELOPER
			g_Console->printdbg("D3D11Window::Hook Initialized\n", g_Console->color.pink);
#endif

			return TRUE;
		}

#if DEVELOPER
		g_Console->printdbg("[+] D3D11Window::Hook Failed to Initialize\n", g_Console->color.red);
#endif

		return FALSE;
	}

	bool D3D9Window::InitializeDX()
	{
		if (!CreateDXWindow()) {
			return FALSE;
		}

		HMODULE D3D9Module = GetModuleHandle(L"d3d9.dll");
		if (D3D9Module == NULL) {
			DestroyDXWindow();
			return FALSE;
		}

		void* Direct3DCreate9 = GetProcAddress(D3D9Module, "Direct3DCreate9");
		if (Direct3DCreate9 == NULL) {
			DestroyDXWindow();
			return FALSE;
		}

		LPDIRECT3D9 Direct3D9 = ((LPDIRECT3D9(__stdcall*)(uint32_t))(Direct3DCreate9))(D3D_SDK_VERSION);
		if (Direct3D9 == NULL) {
			DestroyDXWindow();
			return FALSE;
		}

		D3DDISPLAYMODE DisplayMode;
		if (Direct3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode) < 0) {
			DestroyDXWindow();
			return FALSE;
		}

		D3DPRESENT_PARAMETERS Params;
		Params.BackBufferWidth = 0;
		Params.BackBufferHeight = 0;
		Params.BackBufferFormat = DisplayMode.Format;
		Params.BackBufferCount = 0;
		Params.MultiSampleType = D3DMULTISAMPLE_NONE;
		Params.MultiSampleQuality = NULL;
		Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		Params.hDeviceWindow = dx_hWnd;
		Params.Windowed = 1;
		Params.EnableAutoDepthStencil = 0;
		Params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		Params.Flags = NULL;
		Params.FullScreen_RefreshRateInHz = 0;
		Params.PresentationInterval = 0;
		 
		LPDIRECT3DDEVICE9 Device;
		if (Direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dx_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &Params, &Device) < 0) {
			Direct3D9->Release();
			DestroyDXWindow();
			return FALSE;
		}

		MethodsTable = (unsigned int*)::calloc(119, sizeof(unsigned int));
		if (MethodsTable == NULL)
			return FALSE;

		memcpy(MethodsTable, *(unsigned int**)Device, 119 * sizeof(unsigned int));
		MH_Initialize();
		Direct3D9->Release();
		Direct3D9 = NULL;
		Device->Release();
		Device = NULL;
		DestroyDXWindow();
		Beep(300, 300);
		return TRUE;
	}

	bool D3D9Window::CreateDXWindow() {

		dx_WndwClass.cbSize = sizeof(WNDCLASSEX);
		dx_WndwClass.style = CS_HREDRAW | CS_VREDRAW;
		dx_WndwClass.lpfnWndProc = DefWindowProc;
		dx_WndwClass.cbClsExtra = 0;
		dx_WndwClass.cbWndExtra = 0;
		dx_WndwClass.hInstance = GetModuleHandle(NULL);
		dx_WndwClass.hIcon = NULL;
		dx_WndwClass.hCursor = NULL;
		dx_WndwClass.hbrBackground = NULL;
		dx_WndwClass.lpszMenuName = NULL;
		dx_WndwClass.lpszClassName = L"MJ";
		dx_WndwClass.hIconSm = NULL;
		RegisterClassEx(&dx_WndwClass);
		dx_hWnd = CreateWindow(dx_WndwClass.lpszClassName, L"DirectX9 Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, dx_WndwClass.hInstance, NULL);
		if (dx_hWnd == NULL) {
			return FALSE;
		}
		return TRUE;
	}

	bool D3D9Window::DestroyDXWindow() {
		DestroyWindow(dx_hWnd);
		UnregisterClass(dx_WndwClass.lpszClassName, dx_WndwClass.hInstance);
		if (dx_hWnd != NULL) {
			return FALSE;
		}
		return TRUE;
	}

	bool D3D9Window::CreateFunctionHook(void* Target, void** Detour, LPVOID Original)
	{
		if (MH_CreateHook(&Target, &Detour, reinterpret_cast<LPVOID*>(&Original)) != MH_OK)
			return FALSE;

		if (MH_EnableHook(&Target) != MH_OK)
			return FALSE;

		return TRUE;
	}

	bool D3D9Window::CreateWindowHook(uint16_t Index, void** Original, void* Function) {
		assert(Index >= 0 && Original != NULL && Function != NULL);
		void* target = (void*)MethodsTable[Index];
		if (MH_CreateHook(target, Function, Original) != MH_OK || MH_EnableHook(target) != MH_OK) {
			return FALSE;
		}
		return TRUE;
	}

	/// <summary>
	/// Initialize Dear ImGui
	bool D3D9Window::InitImGui(IDirect3DDevice9* pDevice)
	{
		if (!b_ImGuiInit) {
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			ImGui_ImplWin32_Init(g_GameVariables->g_GameWindow);
			ImGui_ImplDX9_Init(pDevice);
			ImGui_ImplDX9_CreateDeviceObjects();
			ImGui::GetIO().ImeWindowHandle = g_GameVariables->g_GameWindow;
			m_OldWndProc = (WNDPROC)SetWindowLongPtr(g_GameVariables->g_GameWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
			b_ImGuiInit = true;
#if DEVELOPER
			g_Console->printdbg("D3D11Window::Swapchain Initialized\n", g_Console->color.pink);
#endif
			return TRUE;
		}
		b_ImGuiInit = FALSE;
		return FALSE;
	}

	/// <summary>
	/// MAIN RENDER LOOP
	void D3D9Window::Overlay(IDirect3DDevice9* pDevice)
	{
		if (!b_ImGuiInit)
			InitImGui(pDevice);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::GetIO().MouseDrawCursor = g_GameVariables->m_ShowMenu;
		ImGui::GetIO().WantCaptureMouse = g_GameVariables->m_ShowMenu;
		ImGui::GetIO().WantCaptureKeyboard = g_GameVariables->m_ShowMenu;

		//	MENU
		g_Menu->Draw();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	/// <summary>
	/// EXIT
	/// 1. Restore Original Window Proc
	/// 2. Restore Window Hooks
	void D3D9Window::Stop()
	{
		SetWindowLongPtr(g_GameVariables->g_GameWindow, GWLP_WNDPROC, (LONG_PTR)m_OldWndProc);
		Detach();
		return;
	}

	void D3D9Window::DisableFunctionHook(void** Detour)
	{
		MH_DisableHook((void*)Detour);
		return;
	}

	void D3D9Window::DisableWindowHook(uint16_t Index) {
		if (MethodsTable == NULL)
			return;

		assert(Index >= 0);
		MH_DisableHook((void*)MethodsTable[Index]);
		return;
	}

	void D3D9Window::Detach() {
		DisableWindowHook(42);
		DisableWindowHook(17);
		DisableWindowHook(82);
		DisableWindowHook(81);
		DisableWindowHook(65);
		DisableWindowHook(16);
		DisableWindowHook(100);
		DisableWindowHook(87);
		DisableWindowHook(94);
		DisableWindowHook(92);
		DisableWindowHook(107);
		free(MethodsTable);
		MethodsTable = NULL;
		return;
	}

	D3D9Window::~D3D9Window()
	{
		Stop();
	}
}