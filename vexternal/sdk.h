#pragma once

__forceinline void get_classes() {
	g_pglobalvars = read<s_globalvars>(g_base + OFFSET_GLOBALVARS);
	g_pinput = read<s_input>(g_base + OFFSET_INPUT);
}

struct s_matrix
{
	float matrix[16];
};

__forceinline s_matrix get_view_matrix() {
	auto view_render = read<uintptr_t>(g_base + OFFSET_RENDER);
	if (!view_render)
		return s_matrix{};

	auto pview_matrix = read<uintptr_t>(view_render + OFFSET_MATRIX);
	if (pview_matrix != 0) {
		return read<s_matrix>(pview_matrix);
	}
	return s_matrix{};
}

//__forceinline bool world_to_screen(const struct c_vec3 in, struct c_vec3 out) {
//	auto m_vmatrix = get_view_matrix().matrix;
//	float w = m_vmatrix[12] * in.x + m_vmatrix[13] * in.y + m_vmatrix[14] * in.z + m_vmatrix[15];
//	if (w < 0.01f) return false;
//
//	out.x = m_vmatrix[0] * in.x + m_vmatrix[1] * in.y + m_vmatrix[2] * in.z + m_vmatrix[3];
//	out.y = m_vmatrix[4] * in.x + m_vmatrix[5] * in.y + m_vmatrix[6] * in.z + m_vmatrix[7];
//
//	float invw = 1.0f / w;
//	out.x *= invw;
//	out.y *= invw;
//
//	float x = g_width / 2;
//	float y = g_height / 2;
//
//	x += 0.5f * out.x * g_width + 0.5f;
//	y -= 0.5f * out.y * g_height + 0.5f;
//
//	out.x = x;
//	out.y = y;
//	out.z = 0;
//
//	return true;
//}

__forceinline c_vec3 world_to_screen(const struct c_vec3 pos) {
	c_vec3 to = c_vec3();
	auto m_vmatrix = get_view_matrix().matrix;

	float w = m_vmatrix[12] * pos.x + m_vmatrix[13] * pos.y + m_vmatrix[14] * pos.z + m_vmatrix[15];

	if (w < 0.01f) return c_vec3();

	to.x = m_vmatrix[0] * pos.x + m_vmatrix[1] * pos.y + m_vmatrix[2] * pos.z + m_vmatrix[3];
	to.y = m_vmatrix[4] * pos.x + m_vmatrix[5] * pos.y + m_vmatrix[6] * pos.z + m_vmatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = g_width / 2;
	float y = g_height / 2;

	x += 0.5f * to.x * g_width + 0.5f;
	y -= 0.5f * to.y * g_height + 0.5f;

	to.x = x;
	to.y = y;
	to.z = 0;

	return to;
}