#pragma once

struct entity_t;
struct ClActiveClient_t;

namespace GameFunction
{
	bool Com_IsInGame();
	int  Com_SessionMode_GetMode();
	uintptr_t CG_GetPredictedPlayerState();
	uintptr_t CG_GetEntityState(int index);
	bool CG_GetPlayerViewOrigin(game_struct::LocalClientNum_t localClientNum, uintptr_t ps, math::vec3_t* outOrigin);
	bool  CG_WorldPosToScreenPos(game_struct::LocalClientNum_t localClientNum, math::vec3_t* WorldPos, math::vec3_t* outScreenPos);
	void  GetViewAxisProjections(const math::vec3_t* vieworg, const math::vec3_t* viewaxis, const math::vec3_t* worldPoint, math::vec3_t* projections);
	bool  CG_GetEntityWorldTagPosition(game_struct::LocalClientNum_t localClientNum, int entindex, int tag, math::vec3_t* outPos);
	bool CG_GetEntityWorldTagPositionAngles(game_struct::LocalClientNum_t localclientnum, int entIndex, int tagname, math::vec3_t* outposition, math::vec3_t* outangles);
	bool G_GetEntityWorldTagPositionAngles(game_struct::LocalClientNum_t localclientnum, int entIndex, int tagname, math::vec3_t* outposition, math::vec3_t* outangles);

	game_struct::clientinfo_t* CG_GetClientInfo(game_struct::LocalClientNum_t localClientNum, unsigned int entIndex);
	uint32_t ScrStr_FindString(const char* str);
	uintptr_t CG_GetClientInfo(unsigned int entIndex);
	int GetHealth(unsigned int entIndex);
	int GetTeam(unsigned int entIndex);
	bool IsPlayerDead(unsigned int entIndex);
	bool AimTarget_IsTargetVisible_wrapper(game_struct::LocalClientNum_t localClientNum, unsigned int entIndex);
	bool AimTarget_IsTargetVisible(game_struct::LocalClientNum_t localClientNum, entity_t* ent);
	bool  CG_DObjGetWorldTagMatrix(DWORD* pose, uintptr_t  dobj, unsigned int tag, float* tagmat, math::vec3_t* origin, int64_t modelMask);
	uintptr_t CG_GetDObj(int localclientnum, entity_t* ent);
	 bool  CG_WorldPosToScreenPos(int localClientNum, math::vec3_t* worldPos, math::vec2_t* outScreenPos);
	uintptr_t CG_GetLocalClientGlobals();
	game_struct::aimassist_globals* AimAssist_GetClientGlobals();
	bool BG_GetEntityOriginAngles(game_struct::LocalClientNum_t localClientNum, unsigned int entIndex, math::vec3_t* origin, math::vec3_t* angles);
	bool origintest(int index, math::vec3_t* origin);
	void CL_SetViewAngles(game_struct::LocalClientNum_t localClientNum, math::angle_t angles);
	int  Trace_GetEntityHitId(const game_struct::trace_t* trace);
	void  Trace_test(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* end, int passEntityNum, uint32_t contentMask);
	void  CG_LocationalTrace(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* end, int passEntityNum, uint32_t contentMask, bool checkRopes);
	void  CG_TraceCapsule(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* mins, const math::vec3_t* maxs, const math::vec3_t* end, int passEntityNum, uint32_t contentMask, uintptr_t* context);
	void  CG_TraceCapsule2(game_struct::trace_t* results, const math::vec3_t* start, const math::vec3_t* mins, const math::vec3_t* maxs, const math::vec3_t* end, int passEntityNum, int32_t contentMask);
	game_struct::usercmd_t* CL_GetCurrentUsercmd(int localclientnum, int cmdnumber);
	int CL_GetCmdNumber(int localclientnum);
	int Sys_GetUTC();
	void UpdatePlayerInventory(int Controllerindex, int ItemID, int itemQuantity, int UTC, short unknown, bool unknown2);
	uintptr_t StringTable_GetAsset(const char* asset_file);
	uintptr_t StringTable_GetAsset2(uintptr_t asset_hash);

	bool StringTable_GetColumnValueForRow(uintptr_t asset, char** output, int row, int column);
	uintptr_t FindXAssetHeader(uintptr_t* table, char** out, int row, int column);
	uintptr_t cl_test();

	 uintptr_t entitytest(int* index);
	 uintptr_t test1(int index);
	 uintptr_t test2(int index);

}



