#pragma once
#define DEVELOPER 0		//	used for cutting out functions and instructions at compile time

#include <windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

//	Console & Basic Imports
#include <iostream>
#include <conio.h>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

//	DX11
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

//	INTERNET
#include <wininet.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

//	EXTERNAL LIBS (ImGui & MinHook)
#include "libs/ImGui/imgui.h"
#include "libs/ImGui/imgui_internal.h"
#include "libs/ImGui/imgui_Impl_dx9.h"
#include "libs/ImGui/imgui_Impl_Win32.h"
#include "libs/MinHook/MinHook.h"

/// <summary>
///	Global Variables accessible by anything in the DX9Base Namespace
namespace DX9Base {
	using namespace std::chrono_literals;
	inline HMODULE g_hModule{};
	inline LPCWSTR g_moduleName{};
	inline uintptr_t dwGameBase{};
	inline std::atomic_bool g_Running{};
	inline std::atomic_bool g_Killswitch = FALSE;

	
	struct Vector2 {
		float x, y;
	};

	struct Vector3 {
		float x, y, z;
	};

	struct Vector4 {
		float x, y, z, a;
	};

	struct DoubleVector2 {
		double x, y;
	};

	struct DoubleVector3 {
		double x, y, z;
	};

	struct DoubleVector4 {
		double x, y, z, a;
	};

	struct Matrix {
		float matrix[16];
	};
}