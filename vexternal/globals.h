#pragma once

#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <intrin.h>

#include <dwmapi.h>
#include <xmmintrin.h>
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")

#include <atomic>
#include <mutex>
#include <iostream>
#include <vector>
#include <thread>

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

inline HWND g_game_window;
inline GLFWwindow* g_window;
inline bool g_window_visible;
inline int g_width;
inline int g_height;
inline int g_pid;
inline uintptr_t g_base;

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_custom.h"

#include "offsets.h"
#include "vector.h"

class c_weapon;
class c_player;
class c_loot;

inline std::vector<c_player> player_list;
inline std::vector<c_loot> loot_list;
inline bool aiming;

struct s_settings {
	// common, rare, epic, legendary, heirloom
	bool selected_rarities[5];

	int max_distance = 150;
	int box_type = 2;

	bool player_enabled = true;
	bool player_glow = true;
	bool player_box = true;
	bool player_name = true;
	bool player_weapon = true;
	bool player_health = true;
	bool player_shield = true;
	bool player_snapline = false;
	bool player_teamcheck = true;
	bool player_prediction = false;

	bool loot_enabled = false;
	bool loot_box = false;
	bool loot_distance = false;
	bool loot_name = false;

	bool aim_enable = false;
	bool aim_visible = false;
	bool aim_draw_fov = false;
	bool aim_downed = false;
	bool aim_smooth = false;

	int aim_bone = 0;
	int aim_fov = 25;
	float aim_smoothing = 4.f;
	int aim_recoil = 37;

	int skin_id = 0;

	float box_color[4] = { 255, 255, 255, 255 };
	float snapline_color[4] = { 255, 255, 255, 255 };
	float names_color[4] = { 255, 255, 255, 255 };
	float glow_color[3] = { 70, 0, 70 };

	bool bunnyhop = false;
	bool hide_from_capture = false;
} settings;

#include "render.h"

__forceinline int get_procid(std::wstring target_process) {
	std::wstring process_name = target_process;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 0 to get all processes
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);

	if (!Process32First(snapshot, &entry)) {
		return 0;
	}

	while (Process32Next(snapshot, &entry)) {
		if (std::wstring(entry.szExeFile) == process_name) {
			g_pid = entry.th32ProcessID;
			return entry.th32ProcessID;
		}
	}

	return 0;
}

__forceinline uintptr_t get_ms() { // gets the time
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount64();
	}
}

#include "strings.h"
#include "helper.h"
#include "driver.h"

// sdk
#include "c_input.h"
#include "globalvars.h"
#include "usercmd.h"
#include "sdk.h"
#include "c_weapon.h"
#include "c_player.h"
#include "c_loot.h"

__forceinline bool save_config() { // very simple config system thats not very stable sometimes
	FILE* file = NULL;
	errno_t err = fopen_s(&file, _("config.dat"), _("wb+"));
	if (err != 0) {
		return false;
	}

	fwrite(&settings, sizeof(settings), 1, file);
	fclose(file);
	return true;
}

__forceinline bool load_config() { // very simple config system thats not very stable sometimes
	FILE* file = NULL;
	errno_t err = fopen_s(&file, _("config.dat"), _("rb"));
	if (err != 0) {
		return false;
	}

	fread(&settings, sizeof(settings), 1, file);
	fclose(file);
	return true;
}

__forceinline static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, _("glfw error %d: %s\n"), error, description);
}

__forceinline void activate_window() { // poggers (please don't hate me)
	SetForegroundWindow(g_game_window);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); 
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

__forceinline void cleanup_window() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(g_window);
	glfwTerminate();
}

__forceinline BOOL CALLBACK retrieve_window(HWND hwnd, LPARAM lparam) {
	DWORD process_id;
	GetWindowThreadProcessId(hwnd, &process_id);
	if (process_id == g_pid) {
		g_game_window = hwnd;
	}
	return TRUE;
}

__forceinline void __fastcall update_style() noexcept {
	SetWindowDisplayAffinity(glfwGetWin32Window(g_window), settings.hide_from_capture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->WindowPadding = { 10, 10 };
	style->FramePadding = { 2, 2 };
	style->ChildRounding = 2.f;
	style->WindowRounding = 0.f;
	style->ScrollbarRounding = 1.f;
	style->FrameRounding = 2.f;
	style->ItemSpacing = { 3, 6 };
	style->ScrollbarSize = 3.f;
	//style->ScaleAllSizes(0.80f);

	/*static constexpr auto frequency = 1 / 0.55f;
	float alpha = 0;
	alpha = clip(alpha + frequency * ImGui::GetIO().DeltaTime, 0.f, 1.f);
	style->Alpha = alpha;*/

	auto menu_color = ImVec4(menu_accent[0], menu_accent[1], menu_accent[2], menu_accent[3]);
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

	colors[ImGuiCol_WindowBg] = menu_color;
	colors[ImGuiCol_Border] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(menu_color.x - 0.20f, menu_color.y - 0.20f, menu_color.z - 0.20f, 1.00f);

	colors[ImGuiCol_TitleBg] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
	colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];

	colors[ImGuiCol_FrameBg] = ImVec4(menu_color.x - 0.30f, menu_color.y - 0.30f, menu_color.z - 0.30f, 0.75f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(menu_color.x - 0.30f, menu_color.y - 0.30f, menu_color.z - 0.30f, 0.75f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 0.75f);

	colors[ImGuiCol_CheckMark] = ImVec4(menu_color.x + 0.30f, menu_color.y + 0.30f, menu_color.z + 0.30f, 1.00f);

	colors[ImGuiCol_Button] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(menu_color.x + 0.10f, menu_color.y + 0.10f, menu_color.z + 0.10f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);

	colors[ImGuiCol_SliderGrab] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = colors[ImGuiCol_SliderGrab];

	colors[ImGuiCol_Separator] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_Separator];
	colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_Separator];

	colors[ImGuiCol_Tab] = ImVec4(menu_color.x - 0.20f, menu_color.y - 0.20f, menu_color.z - 0.20f, 1.00f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabActive] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_Tab];

	colors[ImGuiCol_ChildBg] = colors[ImGuiCol_Separator];
	colors[ImGuiCol_PopupBg] = colors[ImGuiCol_Separator];

	colors[ImGuiCol_NavHighlight] = ImVec4(menu_color.x - 0.20f, menu_color.y - 0.20f, menu_color.z - 0.20f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(menu_color.x - 0.20f, menu_color.y - 0.20f, menu_color.z - 0.20f, 1.00f);

	colors[ImGuiCol_Header] = ImVec4(menu_color.x + 0.20f, menu_color.y + 0.20f, menu_color.z + 0.20f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(menu_color.x - 0.20f, menu_color.y - 0.20f, menu_color.z - 0.20f, 1.00f);
	colors[ImGuiCol_HeaderActive] = colors[ImGuiCol_Header];
}

__forceinline void setup_window() {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		std::cout << _("glfwinit didnt work.\n");
		return;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor) {
		fprintf(stderr, _("failed to get primary monitor!\n"));
		return;
	}

	g_width = glfwGetVideoMode(monitor)->width;
	g_height = glfwGetVideoMode(monitor)->height;

	glfwWindowHint(GLFW_FLOATING, true);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_MAXIMIZED, true);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	g_window = glfwCreateWindow(g_width, g_height, _("Chrome Legacy Window"), NULL, NULL); // hijack window and create context there instead
	if (g_window == NULL) {
		std::cout << _("could not create window.\n");
		return;
	}

	glfwSetWindowAttrib(g_window, GLFW_DECORATED, false);
	glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, true);
	glfwSetWindowMonitor(g_window, NULL, 0, 0, g_width, g_height + 1, 0);

	glfwMakeContextCurrent(g_window);
	glfwSwapInterval(1); // vsync

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, _("failed to initialize opengl loader!\n"));
		return;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	io.WantSaveIniSettings = false;

	update_style();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs);

	// setup platform/renderer backends
	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
	ImGui_ImplOpenGL3_Init(_("#version 130"));
}