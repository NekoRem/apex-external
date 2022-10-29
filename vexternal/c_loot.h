#pragma once
#include "globals.h"

class c_loot {
public:
	c_loot() = default;
	c_loot(DWORD_PTR pointer) {
		this->address = pointer;
		this->id = read<int>(pointer + OFFSET_SCRIPT_ID);
		this->origin = read<c_vec3>(pointer + OFFSET_ORIGIN);
	}

	bool valid() {
		if ((this->origin.x != NULL || this->origin.y != NULL) && (this->address != NULL))
			return true;
		return false;
	}

	int script_id() {
		return read<DWORD>(this->address + OFFSET_SCRIPT_ID);
	}

	int name_id() {
		return read<DWORD>(this->address + OFFSET_SCRIPT_NAME_ID);
	}

	float distance() {
		auto local_player = read<uintptr_t>(g_base + OFFSET_LOCAL_ENT);
		auto local_dist = read<c_vec3>(local_player + OFFSET_ORIGIN);

		return local_dist.distance(this->vec_origin());
	}

	c_vec3 vec_origin() {
		this->origin = read<c_vec3>(this->address + OFFSET_ORIGIN);
		return this->origin;
	}

	c_vec3 mins() {
		return read<c_vec3>(this->address + OFFSET_VECMINS);
	}

	c_vec3 maxs() {
		return read<c_vec3>(this->address + OFFSET_VECMAXS);
	}
private:
	DWORD_PTR address;
	int id;
	c_vec3 origin;
public:
	c_props info;
};

__forceinline s_box create_box(c_loot loot) {
	c_vec3 origin{}, min{}, max{}, flb{}, brt{}, blb{}, frt{}, frb{}, brb{}, blt{}, flt{};
	int left, top, right, bottom;

	origin = loot.vec_origin();
	min = loot.mins() + origin;
	max = loot.maxs() + origin;

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

	/*world_to_screen(points[3], flb);
	world_to_screen(points[5], brt);
	world_to_screen(points[0], blb);
	world_to_screen(points[4], frt);
	world_to_screen(points[2], frb);
	world_to_screen(points[1], brb);
	world_to_screen(points[6], blt);
	world_to_screen(points[7], flt);*/

	c_vec3 arr[] = {
	  flb,
	  brt,
	  blb,
	  frt,
	  frb,
	  brb,
	  blt,
	  flt
	};

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for (auto x = 1; x < 8; ++x) {
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
} // experimental

__forceinline void __fastcall setup_loot() {
	while (true) {
		std::vector<c_loot> loot;
		c_player local_player(read<DWORD_PTR>(g_base + OFFSET_LOCAL_ENT));
		if (local_player.valid()) {
			for (auto x = 0; x < 13000; ++x) {
				auto entity = read<DWORD_PTR>(g_base + OFFSET_ENTITYLIST + (x * 0x20));
				if (!entity) continue;
				c_loot item(entity);

				auto c_signifier = class_signifier(entity);
				if (c_signifier != _("prop_death_box") && c_signifier != _("prop_survival") && c_signifier != _("npc_dummie")) // block all apart from loot
					continue;

				auto item_id = item.script_id();
				if (item_id > 255) continue;

				for (const auto& [key, value] : item_map) {
					if (item_id == key) {
						item.info = value;
					}
				}

				loot.push_back(item);
			}
		}

		loot_list = loot;
		loot.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // maybe less than this

		get_classes(); // shouldn't really be here but it needs refreshing
	}
}

__forceinline void __fastcall loot_esp(c_player& local) {
	if (!settings.loot_enabled) return;

	for (c_loot& loot : loot_list) {
		if (!loot.valid()) continue;

		auto distance = loot.distance() / 100;
		if (distance > settings.max_distance) continue;
		if (distance < 0.5f) continue;

		auto box = create_box(loot);
		if (box.x == 0 || box.y == 0) continue;

		// not clean but should work
		auto box_color = ImGui::ColorConvertFloat4ToU32(ImVec4(settings.box_color[0], settings.box_color[1], settings.box_color[2], settings.box_color[3]));

		if (settings.loot_box) {
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

		if (settings.loot_distance) {
			char buffer[255];
			sprintf_s(buffer, _("%i"), distance);
			draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(buffer).x / 2), box.y - box.h, IM_COL32_WHITE, buffer);
		}

		/*if (settings.loot_name) {
			auto name = std::to_string(loot.script_id());
			draw_text_outline(box.x + (box.w / 2) - ImGui::CalcTextSize(name.c_str()).x, box.y + box.h, IM_COL32_WHITE, name.c_str());
		}*/

		std::string name = loot.info.item_name;
		switch (loot.info.rarity) {
		case item_rarity::COMMON:
			if (settings.loot_name && settings.selected_rarities[0])
				draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(name.c_str()).x / 2), box.y + box.h, IM_COL32(168, 168, 168, 255), name.c_str());
			break;
		case item_rarity::RARE:
			if (settings.loot_name && settings.selected_rarities[1])
				draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(name.c_str()).x / 2), box.y + box.h, IM_COL32(81, 168, 214, 255), name.c_str());
			break;
		case item_rarity::EPIC:
			if (settings.loot_name && settings.selected_rarities[2])
				draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(name.c_str()).x / 2), box.y + box.h, IM_COL32(178, 55, 200, 255), name.c_str());
			break;
		case item_rarity::LEGENDARY:
			if (settings.loot_name && settings.selected_rarities[3])
				draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(name.c_str()).x / 2), box.y + box.h, IM_COL32(206, 173, 33, 255), name.c_str());
			break;
		case item_rarity::HEIRLOOM:
			if (settings.loot_name && settings.selected_rarities[4])
				draw_text_outline(box.x + (box.w / 2) - (ImGui::CalcTextSize(name.c_str()).x / 2), box.y + box.h, IM_COL32(238, 0, 71, 255), name.c_str());
			break;
		}
	}
}