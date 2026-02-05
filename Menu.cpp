#include"includes.h"


uintptr_t testentity(int index)
{
	auto pointer = GameFunction::test1(0);
	if (!pointer)
		return 0;

	auto cent_start = pointer - (0x48C * ctx::offset::Cent_size);

	return (cent_start + (ctx::offset::Cent_size * index));


}


namespace g_menu
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	std::string str_config_name = "";
	bool b_menu_open = true;
	ImFont* menu_font = nullptr;
	const char* items[] = { "Head", "Body", "Left Feet", "Right Feet", "All" };
	const char*  target_mode[] = { "FOV+Distance", "FOV", "Distance" };
	bool enable_origin_log = false;

	void Menu()
	{
		if (ImGui::IsKeyDown(ImGuiKey_::ImGuiKey_Insert))
			b_menu_open = !b_menu_open;

		if (!b_menu_open)
			return;

		ImGui::SetNextWindowSize(ImVec2(400, 400));
		ImGui::PushFont(menu_font);
		ImGui::Begin("BOCW", &b_menu_open);
		ImGui::BeginTabBar("##1");
		
		if (ImGui::BeginTabItem("Aimbot"))
		{
			if (!xbox && !ps)
			imgui_custom::hotkey(("Aim Key"), aimbotkey, 80.f, { 90,25 });
			else if (xbox && !ps)
			{
				ImGui::Combo("Aim Controller Button", &ctx::config::gamepad::xbox_id, "Left Trigger\0Right Trigger\0Left Shoulder\0Right Shoulder\0");
			}
			else if (ps)
			{
				ImGui::Combo("Aim Controller Button", &ctx::config::gamepad::ps_id, "Left Trigger\0Right Trigger\0Left Shoulder\0Right Shoulder\0");

			}
			ImGui::Checkbox("Enable Aim", &ctx::config::aimbot::enabled);
			ImGui::Checkbox("Silent", &ctx::config::aimbot::silent);

			//ImGui::Checkbox("Silent Aim", &ctx::config::aimbot::silent);
			//ImGui::Checkbox("Only Visible", &ctx::config::aimbot::visible_only);
			//ImGui::Checkbox("Skip Downed", &ctx::config::aimbot::skip_knocked);	
			//ImGui::Checkbox("Aim At SelfRevive", &ctx::config::aimbot::self_reviving);
			//ImGui::Combo("##items",(int*) &ctx::config::aimbot::bone_priority, items, 5);
			//ImGui::Combo("##target_mode", (int*)&ctx::config::aimbot::target_mode, target_mode, 3);
			ImGui::SliderFloat("##Fov1", &ctx::config::aimbot::fov, 0.f, 1000.f, "Fov: %0.1f");
	/*		ImGui::SliderFloat("##max_distance1", &ctx::config::aimbot::max_distance, 0.f, 500.f, "distance: %0.1f");
			ImGui::SliderFloat("##smoothness1", &ctx::config::aimbot::smoothness, 0.f, 100.f, "smoothness: %0.1f");*/


			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem("Visuals"))
		{

			ImGui::Checkbox("Box", &ctx::config::esp::b_showBox);
			ImGui::Checkbox("distance", &ctx::config::esp::b_showDistance);

		
			//ImGui::Checkbox("Team", &ctx::config::esp::b_showTeam);
			//ImGui::Checkbox("Ai", &ctx::config::esp::b_showAI);
			//ImGui::Checkbox("Missile", &ctx::config::esp::b_showMissile);
			//ImGui::Checkbox("item", &ctx::config::esp::b_showItem);
			//ImGui::Checkbox("Health", &ctx::config::esp::b_showHealth);
			//ImGui::Checkbox("Distance", &ctx::config::esp::b_showDistance);
			//ImGui::Checkbox("Name", &ctx::config::esp::b_showName);
			//ImGui::Checkbox("Weapon Name", &ctx::config::esp::b_showWeapon);
			//ImGui::Checkbox("Bone", &ctx::config::esp::b_showBones);
			//ImGui::SliderFloat("##max_distanceesp", &ctx::config::esp::max_distance, 0.f, 1000.f, "distance: %0.1f");



			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("misc"))
		{



			if (ImGui::Button("Unlock All", ImVec2(110, 30)))
			{
				UnlockAll();
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Logs"))
		{
			//static uintptr_t entity1 = address_t{};
			//static uintptr_t entity2 = address_t{};
			//static uintptr_t entity3 = address_t{};

			//static uintptr_t refdef = address_t{};
			//static uintptr_t camera = address_t{};
		

			//static math::vec3_t origin = {};
			//static math::vec3_t angles = {};
			//static int index = 0;
			//static int index2 = 0;
			//static int health= 0;


			//ImGui::Text("exception: %x", ctx::config::logging::exception);

			//ImGui::Text("health: %d", health);
			//ImGui::Text("index: %d", index);
			//ImGui::Text("index2: %d", index2);
			//ImGui::Text("id: %d", ctx::config::logging::id);
			//ImGui::Text("sessionmode: %d", GameFunction::Com_SessionMode_GetMode());
			//ImGui::Text("playerstateoffset: %x", ctx::offset::predictedplayerstate_offset);
			//ImGui::Text("playerentityoffset: %x", ctx::offset::predictedplayerentity_offset);
			//ImGui::Text("CG_GetEntityState: %x", ctx::offset::CG_GetEntityState);
			//ImGui::Text("cg_predictedplayerstate offset: %x", ctx::offset::cg_predictedplayerstate);
			//ImGui::Text("CG_entity1: %p", entity1);
			//ImGui::Text("CG_entity2: %p", entity2);
			//ImGui::Text("CG_entity3: %p", entity3);

			//ImGui::Text("refdef	: %p", refdef);
			//ImGui::Text("camera	: %p", camera);
		

			////ImGui::Text("trace	: %p", ctx::config::logging::tr);

			//ImGui::Text("origin: x: %.1f ---- y: %.1f ---- z: %.1f ----", ctx::config::logging::_localpos.x, ctx::config::logging::_localpos.y, ctx::config::logging::_localpos.z);
			//ImGui::Text("angles: x: %.1f ---- y: %.1f ---- z: %.1f ----", angles.x, angles.y, angles.z);

			//
			//ImGui::SliderInt("##Sliderint", &index, 0, 32, "Index: %d");



			//if (ImGui::Button("entity test1", ImVec2(55, 25)))
			//{
			//	auto cg = GameFunction::CG_GetLocalClientGlobals();
			//	ctx::config::logging::id = reinterpret_cast<game_struct::playerState_t*>(GameFunction::CG_GetPredictedPlayerState())->ClientNum;

			//	entity1 = testentity(index);
			//	entity2 = (uintptr_t)CG_GetEntity(index);//(cg + ctx::offset::predictedplayerentity_offset);

			//	index2 = *(int*)entity1;
			////	
			////	entity2 = (uintptr_t)testentity(index);
			////	entity3 = (uintptr_t)CG_GetEntity(index); //0x48B
			//}

			//ImGui::SameLine();

			//if (ImGui::Button("entity test2", ImVec2(55, 25)))
			//{
			//	health = GameFunction::GetHealth(0);
			//	entity1 = GameFunction::CG_GetLocalClientGlobals();
			//	
			//
			//}

			//if (ImGui::Button("origin test", ImVec2(55, 25)))
			//{
			//	enable_origin_log = !enable_origin_log;
			//}

			//if (enable_origin_log)
			//{
			//	//origin = game_sdk::GetRefDef()->viewOrigin;
			//	GameFunction::BG_GetEntityOriginAngles(game_struct::LOCAL_CLIENT_0, index, &origin, &angles);
			//	//GameFunction::origintest(0, &origin);
			//}


			//if (ImGui::Button("refdef", ImVec2(55, 25)))
			//{
			//	
			//	//refdef = game_sdk::GetRefDef();
			//}

			//if (ImGui::Button("camera", ImVec2(55, 25)))
			//{
			//	camera = (uintptr_t)GameFunction::AimAssist_GetClientGlobals();
			//}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("settings"))
		{

			static std::string str_warn = XOR("Config Must be at least 3 char length!");
			ImGui::Text(XOR("Config:"));
			static char buff[128] = "";
			ImGui::InputText(XOR("##CONFIGNAME"), buff, 32);
			ImGui::SameLine();
			if (ImGui::Button(XOR("New"), ImVec2(50, 25))) {
				if (std::string(buff).length() < 3) {
					ImGui::OpenPopup(XOR("warn"));
					str_warn = XOR("Config Must be at least 3 char length!");
				}
				else {
					ctx::config::Save_Settings(buff);
					std::string(buff).clear();

				}
			}
			auto vec_files = ctx::config::GetList();
			static auto i_config = 0;
			ImGui::Combo(XOR("##CONFIG"), &i_config, vector_getter, static_cast<void*>(&vec_files), vec_files.size());
			if (ImGui::Button(XOR("Save Config"), ImVec2(180, 25))) {
				if (!vec_files.empty()) {
					ctx::config::Save_Settings(vec_files[i_config]);

				}
			}
			ImGui::SameLine();
			if (ImGui::Button(XOR("Load Config"), ImVec2(180, 25))) {
				if (!vec_files.empty()) {
					ctx::config::Load_Settings(vec_files[i_config]);

				}
			}
			if (ImGui::Button(XOR("Remove Config"), ImVec2(180, 25)))
			{
				if (!vec_files.empty()) {

					auto str_config = XOR("C:\\DefaultPath\\config\\") + vec_files[i_config] + XOR(".ini");
					if (remove(str_config.c_str()) != 0)
					{
						str_warn = XOR("Could Not Remove!");
						ImGui::OpenPopup(XOR("warn"));
					}

					str_config = XOR("");
					str_config_name = XOR("");
				}
			}
			if (ImGui::BeginPopup(XOR("warn")))
			{
				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), str_warn.c_str());
				ImGui::EndPopup();
			}

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Load Profile1: NumKey0");
			ImGui::Text("Load Profile2: NumKey1");
			ImGui::Text("Load Profile3: NumKey2");
			ImGui::Spacing();
			ImGui::Spacing();



			ImGui::EndTabItem();


		}

		if (ImGui::BeginTabItem("test"))
		{


			ImGui::TextColored(ImColor(0, 255, 0, 255), "time: %d", GameFunction::Sys_GetUTC());

			ImGui::EndTabItem();

		}


		ImGui::PopFont();
		ImGui::EndTabBar();
		ImGui::End();

	};
}