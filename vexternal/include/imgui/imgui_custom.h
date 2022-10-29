#pragma once

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <array>

static inline float menu_accent[4] = { 0.35f, 0.0f, 0.35f, 1.0f };

namespace ImGui {
	IMGUI_API void ColorPicker(const char* name, float color[3]) noexcept;
	IMGUI_API void arrowButtonDisabled(const char* id, ImGuiDir dir) noexcept;
	IMGUI_API void progressBarFullWidth(float fraction, float height) noexcept;
	IMGUI_API void textUnformattedCentered(const char* text) noexcept;
	IMGUI_API void RenderBoxBorder(ImVec2 p_min, ImVec2 p_max, ImU32 border, ImU32 border_shadow, float rounding);
	IMGUI_API void RenderFilledBox(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float rounding);
	IMGUI_API bool AnimatedCheckbox(const char* label, bool* v);
	IMGUI_API bool AnimatedButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags);
	IMGUI_API bool AnimatedButton(const char* label, const ImVec2& size_arg);
	IMGUI_API void CustomRenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding);
	IMGUI_API void HelpMarker(const char* desc);
	IMGUI_API bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
}