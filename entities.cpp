#include"includes.h"



CgGlobalsMP* CG_GetLocalClientGlobals() // localplayer
{
		//return address_t(*ctx::mem.playerstate_P.ps_ptr.cast<uintptr_t*>() - ctx::mem.CgGlobalsMP.predictedPlayerState.cast()).cast<CgGlobalsMP*>();
}

entity_t* CG_GetEntity(int index)
{
	auto cent_start = game_sdk::Decryption::CG_GetEntity();
	if (!cent_start)
		return nullptr;

	return reinterpret_cast<entity_t*>(cent_start + ctx::offset::Cent_size * index);

	//auto pointer = GameFunction::test1(0);
	//if (!pointer)
	//	return nullptr;

	//auto cent_start = pointer - (0x48C * ctx::offset::Cent_size);

	//return reinterpret_cast<entity_t*>(cent_start + (ctx::offset::Cent_size * index));
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
//
//CharacterInfo_t* entity_t::Get_ClientInfo()
//{
//	return GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, this->Number());
//}


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
	//auto weapon = GameFunction::CG::CG_GetEntWeapon(this);
	//if (!weapon)
	//	return NULL;

	//return GameFunction::UI::UI_SafeTranslateString(GameFunction::CG::CG_GetWeaponDisplayName(weapon).data());

	return std::string_view();
}
