#include "imgui_custom.h"
#include <unordered_map>

void ImGui::ColorPicker(const char* name, float color[3]) noexcept
{
    char buffer[255];
    sprintf_s(buffer, "##%s", name);
    bool popup = ImGui::ColorButton(name, ImVec4{ color[0], color[1], color[2], 1.0f }); 
    ImGui::SameLine();
    ImGui::Text(name);
    if (popup) ImGui::OpenPopup(name);
    if (ImGui::BeginPopup(name)) {
        ImGui::PushItemWidth(150.f);
        ImGui::ColorPicker4(buffer, color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
        ImGui::EndPopup();
    }
}

void ImGui::arrowButtonDisabled(const char* id, ImGuiDir dir) noexcept
{
    float sz = ImGui::GetFrameHeight();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    ImGui::ArrowButtonEx(id, dir, ImVec2{ sz, sz }, ImGuiButtonFlags_Disabled);
    ImGui::PopStyleVar();
}

void ImGui::progressBarFullWidth(float fraction, float height) noexcept
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2{ -1, 0 }, CalcItemWidth(), height + style.FramePadding.y * 2.0f);
    ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    fraction = ImSaturate(fraction);
    RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
    bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));

    if (fraction == 0.0f)
        return;

    const ImVec2 p0{ ImLerp(bb.Min.x, bb.Max.x, 0.0f), bb.Min.y };
    const ImVec2 p1{ ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y };

    const float x0 = ImMax(p0.x, bb.Min.x);
    const float x1 = ImMin(p1.x, bb.Max.x);

    window->DrawList->PathLineTo({ x0, p1.y });
    window->DrawList->PathLineTo({ x0, p0.y });
    window->DrawList->PathLineTo({ x1, p0.y });
    window->DrawList->PathLineTo({ x1, p1.y });
    window->DrawList->PathFillConvex(GetColorU32(ImGuiCol_PlotHistogram));
}

void ImGui::textUnformattedCentered(const char* text) noexcept
{
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) / 2.0f);
    ImGui::TextUnformatted(text);
}

void ImGui::RenderBoxBorder(ImVec2 p_min, ImVec2 p_max, ImU32 border, ImU32 border_shadow, float rounding)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    const float border_size = 1.0f;
    window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), border_shadow, rounding, 0, border_size);
    window->DrawList->AddRect(p_min, p_max, border, rounding, 0, border_size);
}

void ImGui::RenderFilledBox(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float rounding)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
}

bool ImGui::AnimatedCheckbox(const char* label, bool* v)
{
    // prepare
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    const ImGuiID id = window->GetID(label);
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id)) return false;

    // behaviour
    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
    }

    // calculate animation
    const float animation_duration = 0.3f;
    float animation_progress = 1.0f;
    if (g.LastActiveId == id) animation_progress = ImClamp(g.LastActiveIdTimer / animation_duration, 0.0f, 1.0f);

    ImVec4 box_vec4_col = GetStyleColorVec4(ImGuiCol_FrameBg);
    ImVec4 check_vec4_col = GetStyleColorVec4(ImGuiCol_CheckMark);
    ImVec4 border_col = GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 border_shadow_col = GetStyleColorVec4(ImGuiCol_BorderShadow);
    const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

    // render fade-in animation: filled box, check mark
    // render fade-out animation: box border
    if (*v)
    {
        box_vec4_col.w *= animation_progress;
        RenderFilledBox(check_bb.Min, check_bb.Max, ColorConvertFloat4ToU32(box_vec4_col), style.FrameRounding);

        check_vec4_col.w *= animation_progress;
        RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), ColorConvertFloat4ToU32(check_vec4_col), square_sz - pad * 2.0f);

        animation_progress = 1 - animation_progress;

        border_col.w *= animation_progress;
        border_shadow_col.w *= animation_progress;
        RenderBoxBorder(check_bb.Min, check_bb.Max, ColorConvertFloat4ToU32(border_col), ColorConvertFloat4ToU32(border_shadow_col), style.FrameRounding);
    }
    // render fade-in animation: box border
    // render fade-out animation: filled box, check mark
    else
    {
        border_col.w *= animation_progress;
        border_shadow_col.w *= animation_progress;
        RenderBoxBorder(check_bb.Min, check_bb.Max, ColorConvertFloat4ToU32(border_col), ColorConvertFloat4ToU32(border_shadow_col), style.FrameRounding);

        animation_progress = 1 - animation_progress;

        box_vec4_col.w *= animation_progress;
        RenderFilledBox(check_bb.Min, check_bb.Max, ColorConvertFloat4ToU32(box_vec4_col), style.FrameRounding);

        check_vec4_col.w *= animation_progress;
        RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), ColorConvertFloat4ToU32(check_vec4_col), square_sz - pad * 2.0f);
    }

    // render label
    if (label_size.x > 0.0f)
    {
        ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
        RenderText(label_pos, label);
    }

    return true;
}

IMGUI_API bool ImGui::AnimatedButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");
    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

IMGUI_API bool ImGui::AnimatedButton(const char* label, const ImVec2& size_arg)
{
    return AnimatedButtonEx(label, size_arg, ImGuiButtonFlags_None);
}

IMGUI_API void ImGui::CustomRenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    //window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
    const float border_size = g.Style.FrameBorderSize;
    if (border && border_size > 0.0f)
    {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, 0, border_size);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, 0, border_size);
    }
}

IMGUI_API void ImGui::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

IMGUI_API bool ImGui::Spinner(const char* label, float radius, int thickness, const ImU32& color) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    // render
    window->DrawList->PathClear();

    int num_segments = 32;
    int start = abs(ImSin(g.Time * 1.0f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 10) * radius,
            centre.y + ImSin(a + g.Time * 10) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
}