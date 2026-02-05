#include"includes.h"
#include "GameFunction.h"

bool GameFunction::Com_IsInGame()
{
	return VariadicCall<bool>(ctx::offset::com_ingame.cast());

}

int GameFunction::Com_SessionMode_GetMode()
{
	return VariadicCall<int>(ctx::offset::com_sessionmode_getmode.cast());

}

uintptr_t GameFunction::CG_GetPredictedPlayerState()
{
	return VariadicCall<uintptr_t>(ctx::offset::cg_predictedplayerstate.cast(), 0);
}

uintptr_t GameFunction::CG_GetEntityState(int index)
{
	return VariadicCall<uintptr_t>(ctx::offset::CG_GetEntityState.cast(), 0, index);

}

bool GameFunction::CG_GetPlayerViewOrigin(game_struct::LocalClientNum_t localClientNum, uintptr_t ps, math::vec3_t* outOrigin)
{
	return VariadicCall<bool>(ctx::offset::cg_getplayervieworigin, localClientNum, ps, outOrigin);
}

bool GameFunction::CG_WorldPosToScreenPos(game_struct::LocalClientNum_t localClientNum, math::vec3_t* WorldPos, math::vec3_t* outScreenPos)
{
	return VariadicCall<bool>(ctx::offset::worldpostoscreenpos.cast(),localClientNum,WorldPos,outScreenPos);
}

void GameFunction::GetViewAxisProjections(const math::vec3_t* vieworg, const math::vec3_t* viewaxis, const math::vec3_t* worldPoint, math::vec3_t* projections)
{
	VariadicCall<void>(ctx::offset::getviewaxisprojections.cast(), vieworg, viewaxis, worldPoint, projections);
}

bool GameFunction::CG_GetEntityWorldTagPosition(game_struct::LocalClientNum_t localClientNum, int entindex,  int tag, math::vec3_t* outPos)
{
	return VariadicCall<bool>(ctx::offset::cg_getentityworldtagpos, localClientNum, entindex, tag,outPos);

}

bool GameFunction::CG_GetEntityWorldTagPositionAngles(game_struct::LocalClientNum_t localclientnum, int entIndex, int tagname, math::vec3_t* outposition, math::vec3_t* outangles) 
{
	return VariadicCall<bool>(address_t(ctx::offset::pmovehandler.cast<void**>()[31]).cast(), localclientnum, entIndex, tagname, outposition, outangles); 

}

bool GameFunction::G_GetEntityWorldTagPositionAngles(game_struct::LocalClientNum_t localclientnum, int entIndex, int tagname, math::vec3_t* outposition, math::vec3_t* outangles)
{
	return VariadicCall<bool>(address_t(ctx::offset::pmovehandler.cast<void**>()[10]).cast(), localclientnum, entIndex, tagname, outposition, outangles);

}

game_struct::clientinfo_t* GameFunction::CG_GetClientInfo(game_struct::LocalClientNum_t localClientNum, unsigned int entIndex)
{
	return VariadicCall< game_struct::clientinfo_t*>(ctx::offset::cg_getclientinfo.cast() , localClientNum, entIndex);
}

uint32_t GameFunction::ScrStr_FindString(const char* str)
{
	return VariadicCall<uint32_t>(ctx::offset::sl_GetStringOfSize, str);
}

uintptr_t GameFunction::CG_GetClientInfo(unsigned int entIndex)
{
	auto cg = CG_GetLocalClientGlobals();
	if (!cg)return 0;
	auto cl = (ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_offset);
	

	return cl;
}

int GameFunction::GetHealth(unsigned int entIndex)
{
	auto cg = CG_GetLocalClientGlobals();
	if (!cg)return 0;
	auto cl = (ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_offset);
	if (!*(int*)(cl + ctx::offset::CI_infovalid))
		return 0;

	return *(int*)(ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_health);
}

int GameFunction::GetTeam(unsigned int entIndex)
{
	auto cg = CG_GetLocalClientGlobals();
	if (!cg)return 0;

	auto cl = (ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_offset);
	if (!*(int*)(cl + ctx::offset::CI_infovalid))
		return 0;

	return *(int*)(ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_team);
}

bool GameFunction::IsPlayerDead(unsigned int entIndex)
{
	auto cg = CG_GetLocalClientGlobals();
	if (!cg)return 0;

	auto cl = (ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_offset);
	if (!*(int*)(cl + ctx::offset::CI_infovalid))
		return 0;

	return *(int*)(ctx::offset::CI_size * entIndex + cg + ctx::offset::CI_dead);
}

bool GameFunction::AimTarget_IsTargetVisible_wrapper(game_struct::LocalClientNum_t localClientNum, unsigned int entIndex)
{
	return VariadicCall< bool>(ctx::offset::istargetvisible.cast(), localClientNum, entIndex);

}

bool GameFunction::AimTarget_IsTargetVisible(game_struct::LocalClientNum_t localClientNum, entity_t* ent)
{
	return VariadicCall< bool>(ctx::offset::aimtarget_istargetvisible.cast(), localClientNum, ent);

}

bool GameFunction::CG_DObjGetWorldTagMatrix(DWORD* pose, uintptr_t dobj, unsigned int tag, float* tagmat, math::vec3_t* origin, int64_t modelMask)
{
	return VariadicCall< bool>(ctx::offset::CG_DObjGetWorldTagMatrix.cast(), pose, dobj,tag,tagmat,origin,modelMask);

}

uintptr_t GameFunction::CG_GetDObj(int localclientnum, entity_t* ent)
{
	return VariadicCall<uintptr_t>(ctx::offset::cg_getdobj, localclientnum, ent);
}

 bool  GameFunction::CG_WorldPosToScreenPos(int localClientNum, math::vec3_t* worldPos, math::vec2_t* outScreenPos)
{
	return VariadicCall<bool>(ctx::offset::worldpostoscreenpos, localClientNum, worldPos, outScreenPos);

}

uintptr_t GameFunction::CG_GetLocalClientGlobals()
{

	auto ps = CG_GetPredictedPlayerState();
	if (!ps)
		return 0;
	return ps - ctx::offset::predictedplayerstate_offset;
}

game_struct::aimassist_globals* GameFunction::AimAssist_GetClientGlobals()
{
	return VariadicCall<game_struct::aimassist_globals*>(ctx::offset::aimassist_getlocalclientglobals.cast(), 0);
}

bool GameFunction::BG_GetEntityOriginAngles(game_struct::LocalClientNum_t localClientNum, unsigned int entIndex, math::vec3_t* origin, math::vec3_t* angles)
{
	return VariadicCall<bool>((uintptr_t)ctx::offset::CG_GetEntityOriginAngles.cast(), localClientNum, entIndex,origin,angles);

}

bool GameFunction::origintest(int index, math::vec3_t* origin)
{
	return VariadicCall<bool>(ctx::offset::origin_test,0, index, origin);
}

void GameFunction::CL_SetViewAngles(game_struct::LocalClientNum_t localClientNum, math::angle_t angles)
{
	 VariadicCall<void>(ctx::offset::cl_setviewangles, localClientNum,&angles);

}

int GameFunction::Trace_GetEntityHitId(const game_struct::trace_t* trace)
{
	return VariadicCall<int>(ctx::offset::Trace_GetEntityHitId, trace);
}


void GameFunction::Trace_test(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* end, int passEntityNum, uint32_t contentMask)
{
	VariadicCall<void>(ctx::offset::tracetest, results, start, end, passEntityNum, contentMask);

}

void GameFunction::CG_LocationalTrace(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* end, int passEntityNum, uint32_t contentMask, bool checkRopes)
{
	 VariadicCall<void>(ctx::offset::CG_LocationalTrace, results,start, end,passEntityNum,contentMask,checkRopes);

}

void GameFunction::CG_TraceCapsule(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* mins, const math::vec3_t* maxs, const math::vec3_t* end, int passEntityNum, uint32_t contentMask, uintptr_t* context)
{
	VariadicCall<void>(ctx::offset::cg_tracecapsule, results, start,mins,maxs, end, passEntityNum, contentMask, context);

}

void GameFunction::CG_TraceCapsule2(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* mins, const math::vec3_t* maxs, const math::vec3_t* end, int passEntityNum, int32_t contentMask)
{
	VariadicCall<void>(ctx::offset::cg_tracecapsule2, results, start, mins, maxs, end, passEntityNum, contentMask);

}

game_struct::usercmd_t* GameFunction::CL_GetCurrentUsercmd(int localclientnum, int cmdnumber)
{
	return VariadicCall<game_struct::usercmd_t*>(ctx::offset::cl_getusercmd,localclientnum, cmdnumber);
}

int GameFunction::CL_GetCmdNumber(int localclientnum)
{
	return VariadicCall<int>(ctx::offset::cl_getusercmdnumber, localclientnum);
}

int GameFunction::Sys_GetUTC()
{
	return VariadicCall<int>(ctx::offset::sys_getutc);

}

void GameFunction::UpdatePlayerInventory(int Controllerindex, int ItemID, int itemQuantity, int UTC, short unknown, bool unknown2)
{
	VariadicCall<void>(ctx::offset::livestorage_updateplayerinventory, Controllerindex, ItemID, itemQuantity, UTC, unknown, unknown2);
}

uintptr_t GameFunction::StringTable_GetAsset(const char* asset_file)
{
	return VariadicCall<uintptr_t>(ctx::offset::StringTable_GetAsset,asset_file);
}

uintptr_t GameFunction::StringTable_GetAsset2(uintptr_t asset_hash)
{
	return VariadicCall<uintptr_t>(ctx::offset::StringTable_GetAsset2, asset_hash);

}

bool GameFunction::StringTable_GetColumnValueForRow(uintptr_t asset, char** output, int row, int column)
{
	return VariadicCall<bool>(ctx::offset::StringTable_GetColumnValueForRow,asset,output,row,column);
}

uintptr_t GameFunction::FindXAssetHeader(uintptr_t* table, char** out, int row, int column)
{
	return VariadicCall<uintptr_t>(ctx::offset::FindXAssetHeader, table, out, row, column);

}






uintptr_t GameFunction::cl_test()
{
	return VariadicCall<uintptr_t>(ctx::offset::CL_Activeclienttest.cast(), 0);
}

uintptr_t GameFunction::entitytest(int* index)
{
	return VariadicCall<uintptr_t>(ctx::offset::unknown_CG_GetEntity, index);
}

 uintptr_t GameFunction::test1(int index)
 {
	 return VariadicCall<uintptr_t>((uintptr_t)ctx::offset::cl_testaddress.cast(), 0);
 }

 uintptr_t GameFunction::test2(int index)
 {
	 return VariadicCall<uintptr_t>((uintptr_t)ctx::offset::pmovehandler.cast<void**>()[18], index);

 }
