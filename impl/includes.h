#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <chrono>
#include <ctime>
#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <winternl.h>
#include <cstdint>
#include <DbgHelp.h>
#include <thread>
#include <mutex>
#include <map>
#include <algorithm>
#include <d3d11.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <functional>
#include <cassert>
#include <corecrt_math.h>
#include <limits>
#include <numbers>
#include <shared_mutex>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11tex.h>
#include <dwmapi.h>
#include <unordered_set>
#include <workspace/nlohmann/json.hpp>
#include <filesystem>
#include <shellapi.h>
#include <emmintrin.h>
#include <atomic>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

#ifdef _MSC_VER
#include <intrin.h>
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

#include <dependencies/font/font.h>
#include <dependencies/imgui/images.h>
#include <dependencies/imgui/imgui.h>
#include <dependencies/imgui/imgui_impl_dx11.h>
#include <dependencies/imgui/imgui_impl_win32.h>
#include <dependencies/imgui/imgui_settings.h>

#include <dependencies/hexrays/hexrays.h>
#include <dependencies/skcrypt/skcrypter.h>
#include <dependencies/logger/logger.hxx>

#include <workspace/game/config/config.h>
#include <workspace/game/config/json/json.hxx>       
auto g_config = std::make_shared< config::c_config >( );

#include <impl/lock/lock.h>
#include <impl/ia32/ia32.h>
#include <impl/wdk/wdk.h>
#include <workspace/driver/driver.hxx>
#include <workspace/game/unreal/unreal.h>
#include <workspace/game/features/tracers/tracers.hxx>
std::unordered_map<engine::a_fort_player_pawn_athena*, tracers::c_tracers> g_player_traces;

#include <workspace/game/features/chams/chams.hxx>
auto g_chams = std::make_shared< chams::c_chams >( );

#include <workspace/game/drawing/debug/debug.hxx>
auto g_debug = std::make_shared< debug::c_debug >( );

#include <workspace/game/drawing/drawing.hxx>
auto g_drawing = std::make_shared< drawing::c_drawing >( );

#include <workspace/game/entities/lists/lists.h>
#include <workspace/game/entities/world/world.hxx>
auto g_world = std::make_shared< world::c_world >( );

#include <workspace/game/unreal/engine/functions/functions.hxx>
#include <workspace/game/features/targeting/targeting.hxx>
auto g_targeting = std::make_shared< targeting::c_targeting >( );

#include <workspace/game/features/radar/radar.hxx>
auto g_radar = std::make_shared< radar::c_radar >( );

#include <workspace/game/features/enviroment/enviroment.hxx>
#include <workspace/game/features/player/player.hxx>
#include <workspace/game/drawing/render/render.hxx>
#include <workspace/exception/exception.hxx>