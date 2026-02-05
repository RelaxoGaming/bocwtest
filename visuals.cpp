#include"includes.h"

namespace visuals
{
	_declspec(noinline) void draw()
	{
		if (!GameFunction::Com_IsInGame())
			return;

		ImColor current_color{};
		float fdistance{};
		float display_string{};

		auto cg = GameFunction::CG_GetLocalClientGlobals();
		if (!cg)
			return;

		auto ps = (game_struct::playerState_t*) GameFunction::CG_GetPredictedPlayerState();
		if (!ps)
			return;

		auto local_client = GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, ps->ClientNum);

		for (size_t i = 0; i < 16; i++)
		{
			if (i == ps->ClientNum)
				continue;

			auto client = GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, i);
			if (!client)
				continue;

			auto is_valid = client->Dead() == 0;
			if (!is_valid)
				continue;

			if (local_client->Team() == client->Team())
				continue;

			if (client->Origin().is_Zero())
				continue;

			fdistance = game_sdk::units_to_m(local_client->Origin().distance_to(client->Origin()));

			math::vec2_t ScreenPos{}, BoxSize{};

			if (!game_sdk::WorldToEnemyBox(client->Origin(), &ScreenPos, &BoxSize))
				continue;

			auto visible = GameFunction::AimTarget_IsTargetVisible_wrapper(game_struct::LOCAL_CLIENT_0, i);

			current_color = visible ? ImColor(0, 255, 0, 255) : ImColor(255, 0, 0, 255);

			if (ctx::config::esp::b_showHealth)
				game_sdk::draw::draw_health(client->Health(), client->Origin());

			if (ctx::config::esp::b_showBox)
				game_sdk::draw::draw_box(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, current_color, 2.f);

			if (ctx::config::esp::b_showBones)
				game_sdk::draw::draw_bones(i, current_color);

			if (ctx::config::esp::b_showName)
			{
				std::string name = client->Name();
				game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
					ScreenPos.x + (BoxSize.x / 2),
					ScreenPos.y + BoxSize.y + 5 + display_string),
					18.0f,
					current_color,
					true
				);
			}
		}
	}
}
