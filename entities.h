#pragma once
struct CharacterInfo_t;
struct entity_t;
struct CgGlobalsMP;


CgGlobalsMP* CG_GetLocalClientGlobals();
entity_t* CG_GetEntity(int index);
struct entity_t
{
	
	bool get_bbox(math::vec4_t& box);
	//CharacterInfo_t* Get_ClientInfo();
	bool is_alive();
	bool is_team();
	math::vec3_t get_pos();
	game_struct::clientinfo_t* Get_ClientInfo();
	std::string_view  GetWeaponName();

	MAKE_FUNCTION_OFFSET2(ctx::offset::Cent_pose, Pose, DWORD*);
	MAKE_FUNCTION_OFFSET(ctx::offset::Cent_number, Number, int);


	/*OFFSET(game_struct::entityState_t, nextstate, ctx::mem.centity.nextState);
	OFFSET(char, flag, ctx::mem.centity.flags);*/

};


struct CgGlobalsMP
{
	//OFFSET(game_struct::UserClientBits_t, ClientBits, ctx::mem.CgGlobalsMP.sightedEnemyFools); // visible check
	//OFFSET(entity_t*, predictedPlayerEntity, ctx::mem.CgGlobalsMP.predictedPlayerEntity); // pointer to centity_t. used to bypass the pointer encryption
	//OFFSET(game_struct::playerState_t, predictedPlayerState, ctx::mem.CgGlobalsMP.predictedPlayerState);   // pointer to playerstate_t. used to bypass the pointer encryption of GetLocalClientGlobals(aka cg_t)
	//OFFSET(game_struct::shellshock_t, shellShock, ctx::mem.CgGlobalsMP.shellshock); // stun nade effects
	//OFFSET(game_struct::secured_vec3_t, kickAngle, ctx::mem.CgGlobalsMP.KickAngle); //recoil
	//OFFSET(CharacterInfo_t*, character_ptr, ctx::mem.CgGlobalsMP.characterinfo_ptr); // pointer to characterinfo. used to bypass the pointer encryption 

};