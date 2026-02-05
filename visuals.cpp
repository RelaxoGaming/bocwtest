#include"includes.h"





bool is_key_active2()
{

	if (ctx::config::aimbot::aim_key == 0)
		keyPressed = aimbotkey.isDown();

	if (ctx::config::aimbot::aim_key == 1 && aimbotkey.isPressed())
		return keyPressed = !keyPressed;


	return false;

}

namespace visuals
{
	//Draw Real PLayers
	void DrawPlayer(entity_t* ent)
	{
		/*float display_string{};
		float fdistance{};
		ImColor entColor{};
		math::vec4_t bbox{};

		if (!ent->get_bbox(bbox))
			return;


		 fdistance = game_sdk::units_to_m(ctx::client.local_entity->get_pos().distance_to(ent->get_pos()));
		 
		if (fdistance > ctx::config::esp::max_distance)
			return;

		auto team = ent->is_team();

		auto clientinfo = ent->Get_ClientInfo();
		if (!clientinfo)
			return;

		if (ctx::config::esp::b_showHealth)
		{
			game_sdk::draw::draw_health(clientinfo->health, ent->get_pos());
		}

		if (ctx::config::esp::b_showBox)
		{
			game_sdk::draw::draw_box(bbox.x, bbox.y, bbox.z, bbox.w, SET_CURRENT_COLOR(team, ent->nextstate().number, ctx::config::colors::visible_box, ctx::config::colors::visible_box),1.5f);
		}


		if (ctx::config::esp::b_showWeapon)
		{
			std::string_view name = ent->GetWeaponName();
			game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
				bbox.x + (bbox.z / 2),
				bbox.y + bbox.w + 5 + display_string),
				18.0f,
				SET_CURRENT_COLOR(team,ent->nextstate().number,ctx::config::colors::visible_Weapon, ctx::config::colors::not_visible_Weapon),
				true
			);

			display_string += 15;
		}

		if (ctx::config::esp::b_showDistance)
		{
			std::string distance_string("[ ");
			distance_string += std::to_string(fdistance);
			std::string::size_type end = distance_string.find('.');
			if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
				distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
			distance_string += "m ]";
			game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, distance_string, ImVec2(bbox.x + (bbox.z / 2),bbox.y + bbox.w + 5 + display_string),18.0f, SET_CURRENT_COLOR(team, ent->nextstate().number, ctx::config::colors::visible_distance, ctx::config::colors::not_visible_distance),true);

			display_string += 15;
		}


		if (ctx::config::esp::b_showName)
		{
			std::string name = clientinfo->name;
			game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
				bbox.x + (bbox.z / 2),
				bbox.y + bbox.w + 5 + display_string),
				18.0f,
				SET_CURRENT_COLOR(team, ent->nextstate().number, ctx::config::colors::visible_name, ctx::config::colors::not_visible_name),
				true
			);

			display_string += 15;
		}

		if (ctx::config::esp::b_showBones)
		{
			game_sdk::draw::draw_bones(ent->nextstate().number, SET_CURRENT_COLOR(team, ent->nextstate().number, ctx::config::colors::visible_name, ctx::config::colors::not_visible_name));
		}*/

	}
	// Draw Bots
	//void DrawAI(entity_t* ai)
	//{
	//	float display_string{};
	//	float fdistance{};
	//	ImColor entColor{};
	//	math::vec4_t bbox{};


	//	if (!ai->get_bbox(bbox))
	//		return;
	//	//ctx::config::logging::_localpos = ctx::client.local_entity->get_pos();

	//	if (game_sdk::units_to_m(ctx::client.local_entity->get_pos().distance_to(ai->get_pos())) > ctx::config::esp::max_ai_distance)
	//		return;

	//	fdistance = game_sdk::units_to_m(ctx::client.local_entity->get_pos().distance_to(ai->get_pos()));

	//	if (fdistance > ctx::config::esp::max_distance)
	//		return;


	//	

	//	if (ctx::config::esp::b_showDistance)
	//	{

	//		std::string distance_string("[ ");
	//		distance_string += std::to_string(fdistance);
	//		std::string::size_type end = distance_string.find('.');
	//		if (end != std::string::npos && (end + 2) <= (distance_string.length() - 1))
	//			distance_string.erase(distance_string.begin() + end + 2, distance_string.end());
	//		distance_string += "m ]";
	//		game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, distance_string, ImVec2(bbox.x + (bbox.z / 2), bbox.y + bbox.w + 5 + display_string), 18.0f, ctx::config::colors::AI_color, true);

	//		display_string += 15;
	//	}


	//	if (ctx::config::esp::b_showWeapon)
	//	{
	//		std::string_view name = ai->GetWeaponName();
	//		game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
	//			bbox.x + (bbox.z / 2),
	//			bbox.y + bbox.w + 5 + display_string),
	//			18.0f,
	//			ctx::config::colors::AI_color,
	//			true
	//		);

	//		display_string += 15;
	//	}



	//	if (ctx::config::esp::b_showName)
	//	{
	//		std::string name = "Bot";
	//		game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
	//			bbox.x + (bbox.z / 2),
	//			bbox.y + bbox.w + 5 + display_string),
	//			18.0f,
	//			ctx::config::colors::AI_color,
	//			true
	//		);

	//		display_string += 15;
	//	}

	//	if (ctx::config::esp::b_showBones)
	//	{
	//		game_sdk::draw::draw_bones(ai->nextstate().number, ctx::config::colors::AI_color);
	//	}



	//}
	////draw Dropped Weapons and items in MP
	//void DrawItem(entity_t* Item)
	//{
	//	float display_string{};
	//	float fdistance{};
	//	ImColor ItemColor{};
	//	math::vec4_t bbox{};
	//	if (!Item->get_bbox(bbox))
	//		return;

	//	fdistance = game_sdk::units_to_m(ctx::client.local_entity->get_pos().distance_to(Item->get_pos()));

	//	if (fdistance > ctx::config::esp::max_distance)
	//		return;




	//	if (ctx::config::esp::b_showItem)
	//	{
	//		std::string_view name = Item->GetWeaponName();
	//		game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
	//			bbox.x + (bbox.z / 2),
	//			bbox.y + bbox.w + 5 + display_string),
	//			18.0f,
	//			ctx::config::colors::item_color,
	//			true
	//		);

	//		display_string += 15;
	//	}

	//} 
	////draw grenades and tactical MP/WZ
	//void DrawMissile(entity_t* Missile)
	//{
	//	float fdistance{};
	//	float display_string{};
	//	ImColor MissileColor{};
	//	math::vec4_t bbox{};
	//	if (!Missile->get_bbox(bbox))
	//		return;

	//	 fdistance = game_sdk::units_to_m(ctx::client.local_entity->get_pos().distance_to(Missile->get_pos()));

	//	if (fdistance > ctx::config::esp::max_distance)
	//		return;


	//	if (ctx::config::esp::b_showMissile)
	//	{
	//		std::string_view name = Missile->GetWeaponName();
	//		game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
	//			bbox.x + (bbox.z / 2),
	//			bbox.y + bbox.w + 5 + display_string),
	//			18.0f,
	//			ctx::config::colors::Missile_color,
	//			true
	//		);

	//		display_string += 15;
	//	}



	//}

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

			auto client = GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0,i);
			if (!client)
				continue;
			
			auto is_valid = client->Dead() == 0;
			if (!is_valid)
				continue;

	/*	auto cent = CG_GetEntity(i);
			if (!cent)
				continue;*/


			if (local_client->Team() == client->Team())
				continue;
				
			if (client->Origin().is_Zero())
				continue;

			fdistance = game_sdk::units_to_m(local_client->Origin().distance_to(client->Origin()));

			math::vec2_t ScreenPos{}, BoxSize{};
			
			if (!game_sdk::WorldToEnemyBox(client->Origin(), &ScreenPos, &BoxSize))
				continue;

			auto visible = GameFunction::AimTarget_IsTargetVisible_wrapper(game_struct::LOCAL_CLIENT_0, i);

			current_color = visible ? ImColor(0, 255, 0, 255) :  ImColor(255, 0, 0, 255);

			if (ctx::config::esp::b_showHealth)
				game_sdk::draw::draw_health(client->Health(), client->Origin());

			if (ctx::config::esp::b_showBox)
				game_sdk::draw::draw_box(ScreenPos.x, ScreenPos.y, BoxSize.x, BoxSize.y, current_color, 2.f);

			if (ctx::config::esp::b_showBones)
				game_sdk::draw::draw_bones(i, current_color);



			if (ctx::config::esp::b_showDistance)
			{
				std::string name = client->Name();
				game_sdk::draw::draw_sketch_edge_text(g_menu::menu_font, name, ImVec2(
					ScreenPos.x + (BoxSize.x / 2),
					ScreenPos.y + BoxSize.y + 5 + display_string),
					18.0f,
					current_color,
					true
				);

				/*display_string += 15;*/
			}


		}



		
	}

}