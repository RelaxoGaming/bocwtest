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

		if (ImGui::BeginTabItem("Visuals"))
		{
			ImGui::Checkbox("Box", &ctx::config::esp::b_showBox);
			ImGui::Checkbox("Distance", &ctx::config::esp::b_showDistance);
			ImGui::Checkbox("Name", &ctx::config::esp::b_showName);
			ImGui::Checkbox("Health", &ctx::config::esp::b_showHealth);
			ImGui::Checkbox("Bones", &ctx::config::esp::b_showBones);

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
