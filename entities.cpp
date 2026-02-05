#include"includes.h"



CgGlobalsMP* CG_GetLocalClientGlobals()
{
	return nullptr;
}

entity_t* CG_GetEntity(int index)
{
	auto cent_start = game_sdk::Decryption::CG_GetEntity();
	if (!cent_start)
		return nullptr;

	return reinterpret_cast<entity_t*>(cent_start + ctx::offset::Cent_size * index);
}


bool entity_t::get_bbox(math::vec4_t& box)
{

	auto pos = get_pos();
	math::vec2_t ScreenPos{}, BoxSize{}; 

	if (!game_sdk::w2s(pos, &ScreenPos, &BoxSize))
		return false;

	box.x = ScreenPos.x;
	box.y = ScreenPos.y;
	box.z = BoxSize.x;
	box.w = BoxSize.y;


	return true;
}

bool entity_t::is_alive()
{
	return GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, this->Number())->Dead() == 0;
}

bool entity_t::is_team()
{
	auto ps = reinterpret_cast<game_struct::playerState_t*>( GameFunction::CG_GetPredictedPlayerState());
	return GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, this->Number())->Team() ==  GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, ps->ClientNum)->Team();
}

math::vec3_t entity_t::get_pos()
{
	return GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, this->Number())->Origin();
}

game_struct::clientinfo_t* entity_t::Get_ClientInfo()
{
	return GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, this->Number());
}

std::string_view entity_t::GetWeaponName()
{
	return std::string_view();
}
