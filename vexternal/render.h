#pragma once

void draw_text_outline(float x, float y, ImColor color, const char* string, ...) {
	char buf[512];
	va_list arg_list;

	ZeroMemory(buf, sizeof(buf));

	va_start(arg_list, string);
	vsnprintf(buf, sizeof(buf), string, arg_list);
	va_end(arg_list);

	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x - 1, y), ImColor(0.0f, 0.0f, 0.0f, 1.0f), buf, 0, 0, 0);
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x + 1, y), ImColor(0.0f, 0.0f, 0.0f, 1.0f), buf, 0, 0, 0);
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x, y - 1), ImColor(0.0f, 0.0f, 0.0f, 1.0f), buf, 0, 0, 0);
	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x, y + 1), ImColor(0.0f, 0.0f, 0.0f, 1.0f), buf, 0, 0, 0);

	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x, y), color, buf, 0, 0, 0);
	return;
}

void draw_text(float x, float y, ImColor color, const char* string, ...) {
	char buf[512];
	va_list arg_list;

	ZeroMemory(buf, sizeof(buf));

	va_start(arg_list, string);
	vsnprintf(buf, sizeof(buf), string, arg_list);
	va_end(arg_list);

	ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(x, y), color, buf, 0, 0, 0);
	return;
}

void draw_line(float x, float y, float x2, float y2, ImColor color, float thickness) {
	if (x < 0)
		return;

	if (y < 0)
		return;

	if (x2 > (float)g_width)
		return;

	if (y2 > (float)g_height)
		return;

	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x2, y2), color, thickness);
	return;
}

void draw_rectangle(float x, float y, float x2, float y2, ImColor color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + x2, y + y2), color, 0, -1, thickness);
	return;
}

void draw_rectangle_filled(float x, float y, float x2, float y2, ImColor color) {
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + x2, y + y2), color);
	return;
}

void draw_rectangle_filled_multicolor(float x, float y, float x2, float y2, ImColor color, ImColor color2, ImColor color3, ImColor color4) {
	ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + x2, y + y2), color, color2, color3, color4);
	return;
}

void draw_corner_box(float x, float y, float x2, float y2, ImColor color, float thickness) {
	float box_width = x2 - x;
	float box_height = y2 - y;

	// top
	draw_line(x, y, x + box_width / 4, y, color, thickness);
	draw_line(x2, y, x2 - box_width / 4, y, color, thickness);

	// left
	draw_line(x, y, x, y + box_height / 3, color, thickness);
	draw_line(x, y2, x, y2 - box_height / 3, color, thickness);

	// right
	draw_line(x2, y, x2, y + box_height / 3, color, thickness);
	draw_line(x2, y2, x2, y2 - box_height / 3, color, thickness);

	// bottom
	draw_line(x, y2, x + box_width / 4, y2, color, thickness);
	draw_line(x2, y2, x2 - box_width / 4, y2, color, thickness);

	return;
}

void draw_circle(float x, float y, ImColor color, float radius) {
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x, y), radius, color);
	return;
}

void draw_filled_circle(float x, float y, ImColor color, float radius) {
	ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, color);
	return;
}