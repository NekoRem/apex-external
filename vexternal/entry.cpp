#include "globals.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) { // just playing around with this
	if (key == GLFW_KEY_END && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void handle_key_presses() { // toggle overlay
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		g_window_visible = !g_window_visible;
		glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, !g_window_visible);
		if (g_window_visible) {
			HWND overlay_window = glfwGetWin32Window(g_window);
			SetForegroundWindow(overlay_window);
		}
		else {
			activate_window();
		}
	}
	if (GetAsyncKeyState(VK_END) & 1) { // cleanup and exit
		cleanup_window();
		exit(EXIT_SUCCESS);
	}
	if (GetKeyState(VK_RBUTTON) & 0x8000) {
		aiming = true;
	}
	else {
		aiming = false;
	}
}

void run_render() {
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	draw_text_outline(10, 10, IM_COL32_WHITE, _("generic p2c watermark"));

	if (settings.aim_draw_fov) {
		draw_circle(g_width / 2, g_height / 2, IM_COL32_WHITE, settings.aim_fov * 5);
	}

	c_player local_player(read<DWORD_PTR>(g_base + OFFSET_LOCAL_ENT)); // reduce amount of times fetched
	if (local_player.valid()) { // should def use mutex for proper thread sync
		player_esp(local_player);
		player_recoil(local_player);
		player_aim(local_player);
		loot_esp(local_player);
		//local_player.force_jump(); // bhop
	}

	if (g_window_visible) {
		{ // imgui window
			static auto current_tab = 0;
			static auto button_height = 30.0f;
			static auto config_height = 20.0f;
			static auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse;
			static const char* hitboxes[]{ "head", "chest" };
			static const char* box_types[]{ "regular", "filled", "cornered" };
			static const char* rarities[]{ "common", "rare", "epic", "legendary", "heirloom" };
			static std::string combo_preview = ""; // lmao

			update_style(); // update style so the accents will change (maybe don't call this as often)

			ImGui::SetNextWindowSize(ImVec2(450, 350));
			ImGui::Begin(_("apex legends"), nullptr, flags);
			if (ImGui::BeginChild(1, { ImGui::GetContentRegionAvail().x * 0.20f, ImGui::GetContentRegionAvail().y }, true)) {
				if (ImGui::AnimatedButton(_("visuals"), { ImGui::GetContentRegionAvail().x, button_height })) { current_tab = 0; }
				if (ImGui::AnimatedButton(_("aimbot"), { ImGui::GetContentRegionAvail().x, button_height })) { current_tab = 1; }
				if (ImGui::AnimatedButton(_("other"), { ImGui::GetContentRegionAvail().x, button_height })) { current_tab = 2; }
				ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y * 1.35f);
				ImGui::Separator();
				if (ImGui::AnimatedButton(_("save"), { ImGui::GetContentRegionAvail().x, config_height })) { save_config(); }
				if (ImGui::AnimatedButton(_("load"), { ImGui::GetContentRegionAvail().x, config_height })) { load_config(); }
				ImGui::EndChild();
			}
			ImGui::SameLine();
			switch (current_tab) {
			case 0:
				if (ImGui::BeginChild(3, { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y }, true)) {
					ImGui::AnimatedCheckbox(_("player##enabledplayer"), &settings.player_enabled);
					ImGui::Separator();
					ImGui::AnimatedCheckbox(_("glow"), &settings.player_glow);
					ImGui::AnimatedCheckbox(_("box##player"), &settings.player_box);
					ImGui::AnimatedCheckbox(_("name"), &settings.player_name);
					ImGui::AnimatedCheckbox(_("health"), &settings.player_health);
					ImGui::AnimatedCheckbox(_("shield"), &settings.player_shield);
					ImGui::AnimatedCheckbox(_("weapon"), &settings.player_weapon);
					ImGui::AnimatedCheckbox(_("snapline"), &settings.player_snapline);
					ImGui::AnimatedCheckbox(_("prediction dot"), &settings.player_prediction);
					ImGui::EndChild();
				}
				ImGui::SameLine();
				ImGui::BeginGroup(); {
					if (ImGui::BeginChild(4, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.45f }, true)) {
						ImGui::AnimatedCheckbox(_("loot##enabledloot"), &settings.loot_enabled);
						ImGui::Separator();
						ImGui::AnimatedCheckbox(_("box##loot"), &settings.loot_box);
						ImGui::AnimatedCheckbox(_("distance##loot"), &settings.loot_distance);
						ImGui::AnimatedCheckbox(_("name##loot"), &settings.loot_name);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::BeginCombo(_("##rarityloot"), combo_preview.c_str())) {
							combo_preview = "";
							std::vector<std::string> preview;
							for (auto x = 0; x < IM_ARRAYSIZE(rarities); ++x) {
								ImGui::Selectable(rarities[x], &settings.selected_rarities[x], ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_SpanAvailWidth);
								if (settings.selected_rarities[x]) { preview.push_back(rarities[x]); }
							}
							for (auto x = 0; x < preview.size(); ++x) {
								if (preview.size() == 1) { combo_preview += preview.at(x); }
								else if (!(x == preview.size() - 1)) { combo_preview += preview.at(x) + ","; }
								else { combo_preview += preview.at(x); }
							}
							ImGui::EndCombo();
						}
						ImGui::EndChild();
					}
					if (ImGui::BeginChild(5, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }, true)) {
						ImGui::Text(_("other"));
						ImGui::Separator();
						ImGui::AnimatedCheckbox(_("team check"), &settings.player_teamcheck);
						ImGui::Text(_("max distance"));
						ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
						ImGui::SliderInt(_("##max distance"), &settings.max_distance, 50, 500);
						ImGui::Text(_("box type"));
						ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
						ImGui::Combo(_("##boxtype"), &settings.box_type, box_types, sizeof(box_types) / sizeof(*box_types));
						ImGui::EndChild();
					}
					ImGui::EndGroup();
				}
				break;
			case 1:
				if (ImGui::BeginChild(6, { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y }, true)) {
					ImGui::Text(_("aimbot"));
					ImGui::Separator();
					ImGui::AnimatedCheckbox(_("enable"), &settings.aim_enable);
					ImGui::AnimatedCheckbox(_("visible check"), &settings.aim_visible);
					ImGui::AnimatedCheckbox(_("downed check"), &settings.aim_downed);
					ImGui::AnimatedCheckbox(_("smoothing"), &settings.aim_smooth);
					ImGui::AnimatedCheckbox(_("draw fov"), &settings.aim_draw_fov);
					ImGui::EndChild();
				}
				ImGui::SameLine();
				if (ImGui::BeginChild(7, ImGui::GetContentRegionAvail(), true)) {
					ImGui::Text(_("other"));
					ImGui::Separator();
					ImGui::Text(_("aimbot fov"));
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
					ImGui::SliderInt(_("##aimbotfov"), &settings.aim_fov, 10, 500); // 360 will break aimbot
					ImGui::Text(_("smooth control"));
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
					ImGui::SliderFloat(_("##smooth"), &settings.aim_smoothing, 1, 100);
					ImGui::Text(_("recoil percentage"));
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
					ImGui::SliderInt(_("##recoilpercentagex"), &settings.aim_recoil, 1, 100);
					ImGui::Text(_("hitbox"));
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
					ImGui::Combo(_("##hitbox"), &settings.aim_bone, hitboxes, sizeof(hitboxes) / sizeof(*hitboxes));
					ImGui::EndChild();
				}
				break;
			case 2:
				if (ImGui::BeginChild(8, { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y }, true)) {
					ImGui::Text(_("colours"));
					ImGui::Separator();
					//ImGui::SliderInt(_("skin id"), &settings.skin_id, 0, 500);
					ImGui::ColorPicker(_("glow"), settings.glow_color);
					ImGui::ColorPicker(_("boxes"), settings.box_color);
					ImGui::ColorPicker(_("snaplines"), settings.snapline_color);
					ImGui::ColorPicker(_("text"), settings.names_color);
					ImGui::ColorPicker(_("menu accent"), menu_accent);
					ImGui::EndChild();
				}
				ImGui::SameLine();
				if (ImGui::BeginChild(9, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }, true)) {
					ImGui::Text(_("misc"));
					ImGui::Separator();
					ImGui::AnimatedCheckbox(_("hide from capture"), &settings.hide_from_capture); // public
					ImGui::EndChild();
				}
				break;
			}

			ImGui::End();
			ImGui::EndFrame();
		}
	}

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(g_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(g_window);
}

int main() {
	while (!FindWindowA(_("Respawn001"), nullptr)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	g_pid = get_procid(_(L"r5apex.exe"));
	if (driver_init()) {
		g_base = get_base_address();
		if (g_base == 0x0) {
			std::cout << _("[!] driver was not found, reboot and try again.\n");
			system(_("pause"));
			exit(1);
		}

		std::cout << _("[+] process id -> ") << g_pid << std::endl;
		std::cout << _("[+] base address -> ") << g_base << std::endl;

		EnumWindows(retrieve_window, NULL);
		if (!g_game_window) {
			std::cout << _("could not find window.\n");
			std::system(_("pause"));
			return 1;
		}

		setup_window();
		if (!g_window) {
			std::cout << _("could not setup window.\n");
			std::system(_("pause"));
			return 1;
		}

		std::thread(setup_player).detach();
		std::thread(setup_loot).detach();
		//std::thread(skin_changer).detach();

		while (!glfwWindowShouldClose(g_window)) {
			handle_key_presses();
			run_render();
		}
	}
	std::cout << _("[!] failed to connect to driver\n");
	system("pause");
	exit(1);
}
