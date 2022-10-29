#pragma once

struct s_glow
{
	int8_t general, border, border_size, transparency;
};

struct s_visible_time
{
	float last_time_visible[100]; // last visible time in float
	bool last_visibility_state[100]; // last visibility state
	uintptr_t last_check_time[100]; // last visibility check time
} inline last_visible;

struct s_box { // experimental
	s_box() = default;
	s_box(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
	int x, y, w, h;
};

class c_player {
public:
	c_player() = default;
	c_player(DWORD_PTR pointer) {
		this->address = pointer;
		this->health = this->i_health();
		this->shield = this->i_shield();
		this->team = this->i_team();
		this->origin = this->vec_origin();
		this->bleed_out_state = read<int>(pointer + OFFSET_BLEED_OUT_STATE);
	}

	inline bool operator!=(const c_player& player) const {
		return (this->address != player.address);
	}

	inline uintptr_t get_address() {
		return this->address;
	}

	inline bool valid() {
		if (this->vec_origin() != c_vec3(0, 0, 0) && this->address != NULL && this->alive())
			return true;
		return false;
	}

	inline int distance() {
		auto local_player = read<uintptr_t>(g_base + OFFSET_LOCAL_ENT);
		auto local_dist = read<c_vec3>(local_player + OFFSET_ORIGIN);

		return (int)local_dist.distance(this->vec_origin());
	}

	inline bool b_downed() {
		if (this->bleed_out_state < 1) {
			return true;
		}
		return false;
	}

	inline c_weapon weapon() {
		auto weapon_pointer = read<uintptr_t>(this->address + OFFSET_CURRENT_WEAPON);
		weapon_pointer &= 0xffff;
		auto weapon_handle = read<uintptr_t>(g_base + OFFSET_ENTITYLIST + (weapon_pointer << 5));
		return c_weapon(weapon_handle);
	}

	inline std::string s_name() {
		auto index = read<uintptr_t>(this->address + OFFSET_NAME_INDEX);
		auto offsets = read<uintptr_t>((g_base + OFFSET_NAMELIST) + ((index - 1) << 4));
		char buf[64] = { 0 };
		readwtf(offsets, &buf, 64);
		return buf;
	}

	inline int i_health() {
		this->health = read<int>(this->address + OFFSET_HEALTH);
		return this->health;
	}

	inline bool on_ground() {
		this->flags = read<uint32_t>(this->address + OFFSET_FLAGS);
		return (this->flags & 0x1) != 0;
	}

	inline bool is_ducking() {
		this->flags = read<uint32_t>(this->address + OFFSET_FLAGS);
		return (this->flags & 0x2) != 0;
	}

	inline bool alive() {
		return read<int>(this->address + OFFSET_BLEED_OUT_STATE) == 0 && read<int>(this->address + OFFSET_LIFE_STATE) == 0;
	}

	inline int i_team() {
		this->team = read<int>(this->address + OFFSET_TEAM);
		return this->team;
	}

	inline int i_shield() {
		this->shield = read<int>(this->address + OFFSET_SHIELD);
		return this->shield;
	}

	inline int i_max_health() {
		this->max_health = i_health() + i_shield();
		return this->max_health;
	}

	inline bool b_diving() {
		this->diving = read<int>(this->address + OFFSET_DIVING);
		if (this->diving == 1) return true;
		else return false;
	}

	inline c_vec3 mins() {
		return read<c_vec3>(this->address + OFFSET_VECMINS);
	}

	inline c_vec3 maxs() {
		return read<c_vec3>(this->address + OFFSET_VECMAXS);
	}

	inline void glow() {
		if (settings.player_glow) {
			write<int>(this->address + 0x3C8, 1);
			write<int>(this->address + 0x3D0, 2);
			//write<s_glow>(this->address + 0x2C4, { 82, 101, 46, 90 }); //92, 233, 240 //82, 101, 46, 90
			write<float>(this->address + 0x1D0, settings.glow_color[0]); //r
			write<float>(this->address + 0x1D4, settings.glow_color[1]); //g
			write<float>(this->address + 0x1D8, settings.glow_color[2]); //b
		}
		else {
			write<int>(this->address + 0x3C8, 0);
			write<int>(this->address + 0x3D0, 1);
		}
	}

	inline void force_jump() {
		if (!this->address) return;
		if (settings.bunnyhop) {
			static bool jumped_last_tick = false;
			static bool should_fake_jump = false;

			if (!this->alive()) return;

			if (!jumped_last_tick && should_fake_jump) {
				should_fake_jump = false;
				g_pusercmd->m_iButtons |= in_jump;
			}
			else if (g_pusercmd->m_iButtons & in_jump) {
				if (this->on_ground()) {
					jumped_last_tick = true;
					should_fake_jump = true;
				}
				else {
					g_pusercmd->m_iButtons &= ~in_jump;
					jumped_last_tick = false;
				}
			}
			else {
				jumped_last_tick = false;
				should_fake_jump = false;
			}
		}
	}

	inline c_vec3 vec_origin() {
		this->origin = read<c_vec3>(this->address + OFFSET_ORIGIN);
		return this->origin;
	}

	inline c_vec3 view_angles() {
		return read<c_vec3>(this->address + OFFSET_VIEWANGLES);
	}

	inline c_vec3 camera_position() {
		return read<c_vec3>(this->address + OFFSET_CAMERAPOS);
	}

	inline c_vec3 bone_position(uint32_t id) { // should use hitboxes instead of bones for skeleton esp
		uintptr_t bone_array = read<uintptr_t>(this->address + OFFSET_BONES);
		this->origin = this->vec_origin();

		c_vec3 bone = c_vec3();
		bone.x = read<float>(bone_array + 0xCC + (id * 0x30)) + this->origin.x;
		bone.y = read<float>(bone_array + 0xDC + (id * 0x30)) + this->origin.y;
		bone.z = read<float>(bone_array + 0xEC + (id * 0x30)) + this->origin.z;
		return bone;
	}

	inline void pred_position(c_vec3& bone_pos) {
		auto current_weapon = this->weapon();
		if (current_weapon.valid()) {
			auto bullet_speed = this->weapon().bullet_speed();
			auto bullet_gravity = this->weapon().bullet_gravity();

			if (bullet_speed > 1.0f) {
				auto muzzle = read<c_vec3>(this->address + OFFSET_CAMERAPOS);
				auto time = bone_pos.distance(muzzle) / bullet_speed;

				bone_pos.z += (700.0f * bullet_gravity * 0.5f) * (time * time);
				auto delta = (this->vec_origin() * time);

				bone_pos.x += delta.x;
				bone_pos.y += delta.y;
				bone_pos.z += delta.z;
			}
		}
	}

	inline bool visible(int index) {
		if (get_ms() >= (last_visible.last_check_time[index] + 10)) {
			float s_visible_time = read<float>(this->address + OFFSET_VISIBLE_TIME);
			last_visible.last_visibility_state[index] = s_visible_time > last_visible.last_time_visible[index] || s_visible_time < 0.f && last_visible.last_time_visible[index] > 0.f;
			last_visible.last_time_visible[index] = s_visible_time;
			last_visible.last_check_time[index] = get_ms();
		}
		return last_visible.last_visibility_state[index];
	}

	// localplayer only	
	inline c_vec3 punch_angles() {
		return read<c_vec3>(this->address + OFFSET_AIMPUNCH);
	}

	inline void view_angles(c_vec3 angles) {
		write<c_vec3>(this->address + OFFSET_VIEWANGLES, angles);
	}
private:
	DWORD_PTR address;
	int health;
	int shield;
	int max_health;
	int team;
	int bleed_out_state;
	int diving;
	c_vec3 origin;
	uint32_t flags;
};

__forceinline std::string class_signifier(DWORD_PTR entity) { 
	auto class_pointer = read<uintptr_t>(entity + OFFSET_SIGNIFIER);
	char buffer[128] = { 0 };
	readwtf(class_pointer, &buffer, 64);
	return buffer;
} // this looks cleaner

__forceinline s_box create_box(c_player player) { // used for dynamic boxes and esp
	c_vec3 origin{}, min{}, max{}, flb{}, brt{}, blb{}, frt{}, frb{}, brb{}, blt{}, flt{};
	int left, top, right, bottom;

	origin = player.vec_origin();
	min = player.mins() + origin;
	max = player.maxs() + origin;

	c_vec3 points[] = {
	  c_vec3(min.x, min.y, min.z),
	  c_vec3(min.x, max.y, min.z),
	  c_vec3(max.x, max.y, min.z),
	  c_vec3(max.x, min.y, min.z),
	  c_vec3(max.x, max.y, max.z),
	  c_vec3(min.x, max.y, max.z),
	  c_vec3(min.x, min.y, max.z),
	  c_vec3(max.x, min.y, max.z)
	};

	flb = world_to_screen(points[3]);
	brt = world_to_screen(points[5]);
	blb = world_to_screen(points[0]);
	frt = world_to_screen(points[4]);
	frb = world_to_screen(points[2]);
	brb = world_to_screen(points[1]);
	blt = world_to_screen(points[6]);
	flt = world_to_screen(points[7]);

	c_vec3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	if (left || top || right || bottom) {
		for (auto x = 1; x < 8; ++x) { // 8 different points
			if (left > arr[x].x)
				left = arr[x].x;
			if (bottom < arr[x].y)
				bottom = arr[x].y;
			if (right < arr[x].x)
				right = arr[x].x;
			if (top > arr[x].y)
				top = arr[x].y;
		}
		return { (int)left, (int)top, int(right - left), int(bottom - top) };
	}
} // experimental

__forceinline void __fastcall setup_player() {
	while (true) {
		std::vector<c_player> players;
		c_player local_player(read<DWORD_PTR>(g_base + OFFSET_LOCAL_ENT));
		if (local_player.valid()) {
			for (auto x = 0; x < 100 /*g_pglobalvars.maxClients + 1*/; ++x) {
				auto entity = read<DWORD_PTR>(g_base + OFFSET_ENTITYLIST + (x * 0x20));
				if (!entity) continue;
				c_player player(entity);

				if (player != local_player && player.alive()) {
					auto c_signifier = class_signifier(entity);
					if (c_signifier != _("player")) // block all apart from players
						continue;
				}

				players.push_back(player);
			}
		}

		player_list = players;
		players.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(20)); // maybe less than this
	}
}

__forceinline void __fastcall player_esp( c_player& local ) {
	if (!settings.player_enabled) return;
	c_player local_player(read<DWORD_PTR>(g_base + OFFSET_LOCAL_ENT));
	if (!local_player.valid()) return;

	for (auto x = 0; x < player_list.size(); ++x) {
		c_player& player(player_list[x]);
		if (settings.player_teamcheck) {
			if (player.i_team() == local.i_team()) continue;
		}

		auto distance = player.distance() / 100.0f;
		if (distance > settings.max_distance) continue;

		auto weapon = player.weapon().get_name();
		auto name = player.s_name();
		auto health = player.i_health();
		auto shield = player.i_shield();

		auto box = create_box(player);
		if (box.x == 0 || box.y == 0) continue;

		player.glow();

		// not clean but should work
		auto box_color = ImGui::ColorConvertFloat4ToU32(ImVec4(settings.box_color[0], settings.box_color[1], settings.box_color[2], settings.box_color[3]));
		auto snapline_color = ImGui::ColorConvertFloat4ToU32(ImVec4(settings.snapline_color[0], settings.snapline_color[1], settings.snapline_color[2], settings.snapline_color[3]));
		auto text_color = ImGui::ColorConvertFloat4ToU32(ImVec4(settings.names_color[0], settings.names_color[1], settings.names_color[2], settings.names_color[3]));

		if (settings.player_prediction) { // help player predict somebody when not using aimbot
			auto head = player.bone_position(8);
			local.pred_position(head);
			head = world_to_screen(head);
			draw_filled_circle(head.x, head.y, box_color, 4.f);
		}

		if (settings.player_box) {
			switch (settings.box_type) {
			case 0:
				draw_rectangle(box.x, box.y, box.w, box.h, box_color, 1.f);
				break;
			case 1:
				draw_rectangle_filled(box.x, box.y, box.w, box.h, box_color);
				break;
			case 2:
				draw_corner_box(box.x, box.y, box.x + box.w, box.y + box.h, box_color, 1.f);
				break;
			}
		}

		if (settings.player_health) {
			ImColor color = ImColor(min(510 * (100 - health) / 100, 255), min(510 * health / 100, 255), 25, 255);
			int delta = health * box.h / 100;
			//char buffer[255];
			//sprintf_s(buffer, _("%i"), health);

			draw_rectangle_filled(box.x - 2, box.y + (box.h - delta) + 1, 2, delta - 1, IM_COL32(20, 20, 20, 255));
			draw_rectangle_filled(box.x - 2, box.y + (box.h - delta) + 1, 2, delta - 1, color);
		}

		if (settings.player_shield) {
			int delta = shield * box.h / 100;
			//char buffer[255];
			//sprintf_s(buffer, _("%i"), shield);

			if (settings.player_health) {
				draw_rectangle_filled(box.x - 4, box.y + (box.h - delta) + 1, 2, delta - 1, IM_COL32(20, 20, 20, 255));
				draw_rectangle_filled(box.x - 4, box.y + (box.h - delta) + 1, 2, delta - 1, IM_COL32(40, 40, 255, 255));
			}
			else {
				draw_rectangle_filled(box.x - 2, box.y + (box.h - delta) + 1, 2, delta - 1, IM_COL32(20, 20, 20, 255));
				draw_rectangle_filled(box.x - 2, box.y + (box.h - delta) + 1, 2, delta - 1, IM_COL32(40, 40, 255, 255));
			}
		}

		if (settings.player_snapline) {
			draw_line(g_width / 2, g_height, box.x + (box.w / 2), box.y + box.h, snapline_color, 1.f);
		}

		/*if (settings.player_weapon) {
			draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(weapon.c_str()).x / 2), box.y + box.h, text_color, weapon.c_str());
		}*/

		if (settings.player_name) {
			draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(name.c_str()).x / 2), box.y - box.h, text_color, name.c_str());
		}
	}
}

__forceinline void __fastcall player_recoil( c_player& local ) {
	static c_vec3 last_punch_angle;
	if (local.alive()) {
		auto view_angles = local.view_angles();
		auto punch_angles = local.punch_angles() * (settings.aim_recoil / 100.0f);

		view_angles += last_punch_angle;
		view_angles -= punch_angles;
		view_angles.normalize();

		local.view_angles(view_angles);
		last_punch_angle = punch_angles;
	}
}

__forceinline void __fastcall player_aim( c_player& local ) { // this once worked but i did something and couldn't remember what and didn't care enough
	if (settings.aim_enable) {
		float closest_distance = FLT_MAX;
		c_player best_target{};
		int best_target_index = 0;
		int aim_hitbox = 0;

		switch (settings.aim_bone) {
		case 0:
			aim_hitbox = 8; // head
			break;
		case 1:
			aim_hitbox = 3; // pelvis
		}

		auto view_angles = local.view_angles();
		auto camera_pos = local.camera_position();

		for (auto x = 0; x < player_list.size(); ++x) {
			c_player player(player_list[x]);
			if (settings.player_teamcheck) {
				if (player.i_team() == local.i_team()) continue;
			}

			auto distance = player.distance() / 100.0f;
			if (distance > settings.max_distance) continue;

			//if (player.b_downed()) continue;
			//if (player.b_diving()) continue;

			auto entity_bone = player.bone_position(aim_hitbox);
			auto calculated_angles = c_vec3();
			calculated_angles.calc_angle(camera_pos, entity_bone);

			auto delta = calculated_angles - view_angles;
			delta.normalize();

			auto fov = sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
			if (fov < settings.aim_fov && fov < closest_distance) {
				closest_distance = fov;
				best_target = player;
				best_target_index = x;
			}
		}

		if (best_target.valid()) {
			auto view_angles = local.view_angles();
			auto camera_pos = local.camera_position();
			auto target_bone = best_target.bone_position(aim_hitbox);
			
			auto final_angle = c_vec3();
			final_angle.calc_angle(camera_pos, target_bone);
			final_angle.normalize();

			auto delta = final_angle - view_angles;
			delta.normalize();

			auto smoothed_angle = view_angles + (settings.aim_smooth ? delta / settings.aim_smoothing : delta);
			smoothed_angle.normalize();
			smoothed_angle.clamp();

			if (settings.aim_visible) {
				auto visible = best_target.visible(best_target_index);
				if (visible && aiming) {
					local.view_angles(smoothed_angle);
				}
			}
			else {
				if (aiming) {
					auto aim = world_to_screen(target_bone);
					local.view_angles(smoothed_angle);
				}
			}
		}
	}
}

__forceinline void skin_changer() { // was test, but i later found out you need to do this in main menu or precache models :/
	c_player local_player(read<DWORD_PTR>(g_base + OFFSET_LOCAL_ENT));

	auto weapon = local_player.weapon();
	if (weapon.weapon_id() == -1) return;

	weapon.change_skin(settings.skin_id);
}

__forceinline void force_bhop() { // never got around to this

}