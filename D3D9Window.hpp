#pragma once
#include "helper.h"
#include "Console.hpp"
#include "Game.hpp"
#include "Hooking.hpp"
#include "Menu.hpp"

namespace DX9Base {
	class D3D9Window
	{
		typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
		EndScene oEndScene = NULL;

		typedef HRESULT(APIENTRY* Present) (IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
		Present oPresent = NULL;

		typedef HRESULT(APIENTRY* DrawIndexedPrimitive)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
		DrawIndexedPrimitive oDrawIndexedPrimitive = NULL;

		typedef HRESULT(APIENTRY* DrawPrimitive)(IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, UINT);
		DrawPrimitive oDrawPrimitive = NULL;

		typedef HRESULT(APIENTRY* SetTexture)(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
		SetTexture oSetTexture = NULL;

		typedef HRESULT(APIENTRY* Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
		Reset oReset = NULL;

		typedef HRESULT(APIENTRY* SetStreamSource)(IDirect3DDevice9*, UINT, IDirect3DVertexBuffer9*, UINT, UINT);
		SetStreamSource oSetStreamSource = NULL;

		typedef HRESULT(APIENTRY* SetVertexDeclaration)(IDirect3DDevice9*, IDirect3DVertexDeclaration9*);
		SetVertexDeclaration oSetVertexDeclaration = NULL;

		typedef HRESULT(APIENTRY* SetVertexShaderConstantF)(IDirect3DDevice9*, UINT, const float*, UINT);
		SetVertexShaderConstantF oSetVertexShaderConstantF = NULL;

		typedef HRESULT(APIENTRY* SetVertexShader)(IDirect3DDevice9*, IDirect3DVertexShader9*);
		SetVertexShader oSetVertexShader = NULL;

		typedef HRESULT(APIENTRY* SetPixelShader)(IDirect3DDevice9*, IDirect3DPixelShader9*);
		SetPixelShader oSetPixelShader = NULL;

	public:

		explicit D3D9Window() = default;
		~D3D9Window() noexcept;
		D3D9Window(D3D9Window const&) = delete;
		D3D9Window(D3D9Window&&) = delete;
		D3D9Window& operator=(D3D9Window const&) = delete;
		D3D9Window& operator=(D3D9Window&&) = delete;

		DWORD m_GameProcessId;
		HMODULE m_HInstance;
		HWND m_OverlayWindowHandle;
		HWND m_GameWindowHandle;
		WNDPROC m_GameWindowProc;
		WNDPROC m_OldWndProc{};
		const char* m_WindowClassName;
		const char* m_WindowName;
		LPDIRECT3DDEVICE9EX dx_D3D9Device{};
		D3DPRESENT_PARAMETERS dx_D3D9PresentParams{};

		bool b_ImGuiInit = FALSE;

		bool Start();
		void Stop();
		void Overlay(IDirect3DDevice9* pDevice);

		bool InitializeDX();
		bool CreateDXWindow();
		bool DestroyDXWindow();
		bool CreateFunctionHook(void* Target, void** Detour, LPVOID Original);
		bool CreateWindowHook(uint16_t Index, void** Original, void* Function);
		void DisableFunctionHook(void** Hook);
		void DisableWindowHook(uint16_t Index);
		
		bool InitImGui(IDirect3DDevice9* pDevice);
		void Detach();
		static LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static HRESULT APIENTRY MJEndScene(IDirect3DDevice9* pDevice);
		static HRESULT APIENTRY MJPresent(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
		static HRESULT APIENTRY MJDrawIndexedPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
		static HRESULT APIENTRY MJDrawPrimitive(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
		static HRESULT APIENTRY MJSetTexture(LPDIRECT3DDEVICE9 pDevice, DWORD Sampler, IDirect3DBaseTexture9* pTexture);
		static HRESULT APIENTRY MJReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
		static HRESULT APIENTRY MJSetStreamSource(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT sStride);
		static HRESULT APIENTRY MJSetVertexDeclaration(LPDIRECT3DDEVICE9 pDevice, IDirect3DVertexDeclaration9* pdecl);
		static HRESULT APIENTRY MJSetVertexShaderConstantF(LPDIRECT3DDEVICE9 pDevice, UINT StartRegister, const float* pConstantData, UINT Vector4fCount);
		static HRESULT APIENTRY MJSetVertexShader(LPDIRECT3DDEVICE9 pDevice, IDirect3DVertexShader9* veShader);
		static HRESULT APIENTRY MJSetPixelShader(LPDIRECT3DDEVICE9 pDevice, IDirect3DPixelShader9* piShader);

	private:
		WNDCLASSEX dx_WndwClass;
		HWND dx_hWnd;
		LPDIRECT3DDEVICE9 d3d_Device{};
	};
	inline std::unique_ptr<D3D9Window> g_D3D9Window;
}