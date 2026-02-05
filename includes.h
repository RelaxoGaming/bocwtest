#pragma once

// Build configuration (must be first!)
#include "build_config.h"

#include"../shared/shared.hpp"

using namespace shared;


#include <windows.h>
#include<atlcomcli.h>
#include<vector>
#include<winnt.h>
#include<memory>
#include<string>
#include<array>
#include"Imgui/imgui.h"
#include"Imgui/imgui_internal.h"
#include"Imgui/imgui_impl_win32.h"
#include"Imgui/imgui_impl_dx12.h"
#include"minhook/MinHook.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_4.h>
#include<mutex>

// Memory abstraction layer (DMA-ready)
#include "memory/IMemoryReader.h"
#include "memory/MemoryCache.h"

#include"game_struct.h"
#include"d3d.h"
#include "Menu.h"
#include"Start.h"
#include"Hook.h"
#include "utility.h"
#include"LazyImporter.h"
#include "Start.h"
#include"mem.h"
#include "GameFunction.h"
#include"spoof.h"
#include "sdk.h"
#include "entities.h"
#include "Client.h"
#include"bocw.h"
#include"CLActiveClient.h"
#include"cfg.h"
#include "visuals.h"
#include "custom_imgui.h"
#include "custom_shared.h"


#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define is_valid_ptr(p) ((uintptr_t)(p) <= 0x7FFFFFFEFFFF && (uintptr_t)(p) >= 0x1000) 

#define is_bad_ptr(p)	(!is_valid_ptr(p))