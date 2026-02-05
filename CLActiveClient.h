#pragma once


struct ClActiveClient_t
{
	//OFFSET(unsigned int, cmd_number_aab, ctx::mem.CLActiveClient.cmd_number_aab);
	//OFFSET(int, cmdNumber, ctx::mem.CLActiveClient.cmdNumber);

	int GetCmdNumber();
	//void SetCmdNumber(int cmdNumber_new);

	void SetViewAngles(math::angle_t newAngles);

	game_struct::usercmd_t* CL_GetUserCmd(int sequence_number);
};