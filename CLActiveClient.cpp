#include"includes.h"

//
//ClActiveClient_t* GameFunction::CL::GetClActiveClient(game_struct::LocalClientNum_t LocalClientNum)
//{
//	// auto addr = ctx::mem.gamefunction.ClActiveClient.cast<ClActiveClient_t* (*)(game_struct::LocalClientNum_t)>();
//	//return spoof_call<ClActiveClient_t*>(ctx::mem.trampoline.cast<void*>(), addr, LocalClientNum);
//}
//
//
//int ClActiveClient_t::GetCmdNumber()
//{
//	/*auto a1 = (uintptr_t)this;
//	return cmdNumber() ^ (((a1 + ctx::mem.CLActiveClient.cmdNumber.cast()) ^ cmd_number_aab()) * (((a1 + ctx::mem.CLActiveClient.cmdNumber.cast()) ^ cmd_number_aab()) + 2));*/
//}
//
//void ClActiveClient_t::SetViewAngles(math::angle_t newAngles)
//{
//	/*auto addr = ctx::mem.gamefunction.CL_SetViewAngle.cast<void(*)(CgGlobalsMP*, math::angle_t) >();
//	spoof_call<void>(ctx::mem.trampoline.cast<void*>(), addr, ctx::client.cg, newAngles);*/
//}
//
//game_struct::
// 
// _t* ClActiveClient_t::CL_GetUserCmd(int sequence_number)
//{
//	/*return reinterpret_cast<game_struct::usercmd_t*>((uintptr_t)this + ctx::mem.CLActiveClient.usercmd.cast() + ((sequence_number & 0x7F) * 0x228));*/
//
//}