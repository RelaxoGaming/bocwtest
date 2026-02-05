
#include"includes.h"
namespace StartPoint
{

	struct loot_struct
	{
		char pad1[0x4];
		unsigned int retvalue;
	};


	inline loot_struct* GetItemQuantity_hook(int controller, int item)
	{
		auto ret = VariadicCall<loot_struct*>(ctx::offset::Loot_GetitemQuantity, controller, item);
		ret->retvalue = 1;
		return ret;
	}

	inline __int64  sub_1B3EBE0(int a1, int a2, int a3, void* a4, char a5) //48 89 74 24 ? 57 48 83 EC 40 4D 8B D0 48 89 5C 24 ? 4C 8B DA 48 8B F9 0F 1F 84 00 ? ? ? ? 8B 05 ? ? ? ? 
	{
		
		auto ret = _ReturnAddress();
		return VariadicCall<__int64>(0, a1, a2, a3, a4, a5);
	}
		                             
	void Init()
	{
		MH_Initialize();


		//ctx::config::Auto_Load();

		
		//48 8b 0d ? ? ? ? e8 ? ? ? ? 84 c0 74 ? ba ? ? ? ? 49 8b ce
		ctx::offset::Trampoline = mem::Scanner::Scanner("FF 23", "Trampoline").getaddr();
		ctx::offset::DVARBOOL_cl_packetdup = mem::Scanner::Scanner("48 8B 0D ? ? ? ? E8 ? ? ? ? 8B 9E ? ? ? ? 2B D8 FF CB 41 23 9F ? ? ? ? 45 8B A7", "DVARBOOL_cl_packetdup").resolve_mov(); //48 8B 0D ? ? ? ? E8 ? ? ? ? 8B 9E ? ? ? ? 2B D8 FF CB 41 23 9F ? ? ? ? 45 8B A7
		ctx::offset::DVARBOOL_cg_draw2d = mem::Scanner::Scanner("48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? E8 ? ? ? ? 84 C0 74 19", "DVARBOOL_cg_draw2d").resolve_mov();
		ctx::offset::pmovehandler = mem::Scanner::Scanner("E8 ? ? ? ? 48 8D 87 ? ? ? ? 4D 8B CE 48 89 44 24 ? 4C 8D 1D", "pmovehandler",0x14).resolve_lea();
		ctx::offset::com_sessionmode_getmode = mem::Scanner::Scanner("E8 ? ? ? ? 8B E8 83 F8 04 74 4B 48 8B D7 8B C8 E8 ? ? ? ? 8B C8 83 F8 FF 74 11", "com_sessionmode_getmode").resolve_call();
		ctx::offset::com_ingame = mem::Scanner::Scanner("E8 ? ? ? ? 84 C0 75 16 B8 ? ? ? ? 89 05 ? ? ? ? 89 05 ? ? ? ? E9 ? ? ? ?", "com_ingame").resolve_call();
		ctx::offset::cg_predictedplayerstate = mem::Scanner::Scanner("e8 ? ? ? ? 33 d2 0f 57 c0 b9", "cg_predictedplayerstate").resolve_call();
		ctx::offset::CG_GetEntityState = mem::Scanner::Scanner("e8 ? ? ? ? 66 83 b8 ? ? ? ? ? 75 ? 8b", "CG_GetEntityState").resolve_call();
		ctx::offset::unknown_CG_GetEntity = mem::Scanner::Scanner("E8 ? ? ? ? 48 8D 8B ? ? ? ? 8B F8 E8 ? ? ? ? 3B C7", "unknown_CG_GetEntity").resolve_call();
		ctx::offset::predictedplayerstate_offset = mem::Scanner::Scanner("48 05 ? ? ? ? 48 69 d1 ? ? ? ? 48 03 c2 48 81 c4", "predictedplayerstate_offset",2).get_offset(); //add rax,00856CB0
		ctx::offset::cg_refdef = mem::Scanner::Scanner("48 8D 8E ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 41 8B CE E8 ? ? ? ? ", "predictedplayerstate_offset", 3).get_offset();
		ctx::offset::nextstate = mem::Scanner::Scanner("4c 8d b0 ? ? ? ? eb ? 45 33 f6 4d 85 f6", "nextstate", 3).get_offset();

		ctx::offset::predictedplayerentity_offset = mem::Scanner::Scanner("48 8d 96 ? ? ? ? 89 9e ? ? ? ? 41 8b ce", "predictedplayerentity_offset",3).get_offset(); //lea rdx,[rsi+00854E40]
		ctx::offset::CI_health = mem::Scanner::Scanner("42 83 bc 19 ? ? ? ? ? 7e ? 0f 28 da", "cl_health",4).get_offset(); 
		ctx::offset::CI_team = mem::Scanner::Scanner("8b 80 ? ? ? ? 89 44 24 ? c6 44 24 ? ? 89 7c 24", "cl_team", 2).get_offset();
		ctx::offset::CI_size = mem::Scanner::Scanner("48 69 C8 ? ? ? ? 48 8D 82 ? ? ? ? 48 03 C1 80 B8 ? ? ? ? ? 75 02 33 C0", "cl_size", 4).get_offset();
		ctx::offset::CI_offset = mem::Scanner::Scanner("48 69 C8 ? ? ? ? 48 8D 82 ? ? ? ? 48 03 C1 80 B8 ? ? ? ? ? 75 02 33 C0", "cl_offset", 10).get_offset();
		ctx::offset::CI_dead = mem::Scanner::Scanner("48 8B 94 11 ? ? ? ? 48 85 D2 74 18 49 8B CF E8 ? ? ? ? B8", "cl_dead",4).get_offset(); //48 8b 8c 11
		ctx::offset::CI_infovalid = mem::Scanner::Scanner("80 B8 ? ? ? ? ? 75 02 33 C0 48 81 C4 ? ? ? ? 41 5E 5E 5D", "cl_infovalid", 2).get_offset();
		ctx::offset::CI_name = mem::Scanner::Scanner("49 81 c1 ? ? ? ? 4c 03 cb e8 ? ? ? ? 8b f0 0f b7 4d ? 66 3b f9", "cl_name", 3).get_offset();
		ctx::offset::CI_origin = mem::Scanner::Scanner("f3 0f 10 8c 32 ? ? ? ? f3 41 0f 5c c9", "cl_origin",5).get_offset();
		ctx::offset::cg_kickangles = mem::Scanner::Scanner("48 81 C7 ? ? ? ? 48 8D 55 50 48 8D 4D 08 F3 44 0F 10 07 F3 0F 10 7F ? F3 0F 10 77 ? E8 ? ? ? ? ", "cg_kickangles", 3).get_offset();

		ctx::offset::CG_GetEntityOriginAngles = mem::Scanner::Scanner("E8 ? ? ? ? E9 ? ? ? ? 80 F9 01 0F 85 ? ? ? ? E8 ? ? ? ? 48 8B 3D", "CG_GetEntityOriginAngles").resolve_call(); //lea rdx,[rsi+00854E40]
			//ctx::offset::origin_test = mem::Scanner::Scanner("E8 ? ? ? ? 4C 8D 86 ? ? ? ? 48 8B 86 ? ? ? ? 48 05 ? ? ? ? F3 44 0F 11 7C 24 ? 4C 8D 4D", "origin_test").resolve_call();
		ctx::offset::cg_getclientinfo = mem::Scanner::Scanner("e8 ? ? ? ? 48 85 c0 74 ? 8b 80 ? ? ? ? e9", "cg_getclientinfo").resolve_call();
		ctx::offset::istargetvisible = mem::Scanner::Scanner("E8 ? ? ? ? 84 C0 74 2F 45 84 FF 74 68 48 8B 0D", "istargetvisible").resolve_call(); //E8 ? ? ? ? 84 C0 74 0E B0 01 48 81 C4 ? ? ? ? 41 5F 5E 5D 
		ctx::offset::aimtarget_istargetvisible = mem::Scanner::Scanner("48 89 5c 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? 0f 29 bc 24 ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 48 89 55", "aimtarget_istargetvisible").getaddr(); //E8 ? ? ? ? 84 C0 74 0E B0 01 48 81 C4 ? ? ? ? 41 5F 5E 5D 

		ctx::offset::worldpostoscreenpos = mem::Scanner::Scanner("E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B CB E8 ? ? ? ? F3 0F 10 64 24 ? F3 0F 10 6C 24 ? ", "worldpostoscreenpos").resolve_call();
		ctx::offset::aimassist_getlocalclientglobals = mem::Scanner::Scanner("E8 ? ? ? ? F3 44 0F 10 1D ? ? ? ? 48 8B D8 F3 44 0F 10 0D", "aimassist_getlocalclientglobals").resolve_call();
		ctx::offset::cg_getplayervieworigin = mem::Scanner::Scanner("E8 ? ? ? ? E9 ? ? ? ? F3 0F 10 83 ? ? ? ? 4C 8D 8D", "cg_getplayervieworigin").resolve_call();
		ctx::offset::getviewaxisprojections = mem::Scanner::Scanner("E8 ? ? ? ? E9 ? ? ? ? F3 0F 10 83 ? ? ? ? 4C 8D 8D", "getviewaxisprojections").resolve_call();
		ctx::offset::cl_setviewangles = mem::Scanner::Scanner("e8 ? ? ? ? f3 0f 10 05 ? ? ? ? 48 8d 95", "cl_setviewangles").resolve_call();
		ctx::offset::sl_GetStringOfSize = mem::Scanner::Scanner("E8 ? ? ? ? 8B D8 48 8B 8D ? ? ? ? 0F B7 91 ? ? ? ? B1 24 E8 ? ? ? ? ", "sl_GetStringOfSize").resolve_call();
		ctx::offset::Trace_GetEntityHitId = mem::Scanner::Scanner("E8 ? ? ? ? 66 83 F8 7F 0F 8C ? ? ? ? F3 0F 10 5D ? F3 0F 10 05 ? ? ? ? ", "Trace_GetEntityHitId").resolve_call();
		ctx::offset::CG_LocationalTrace = mem::Scanner::Scanner("E8 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? F3 0F 10 8D ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? ", "CG_LocationalTrace").resolve_call();
		ctx::offset::tracetest = mem::Scanner::Scanner("E8 ? ? ? ? EB 05 E8 ? ? ? ? F3 44 0F 10 5D ? ", "tracetest").resolve_call(); //C7 44 24 ? ? ? ? ? E8 ? ? ? ? F3 0F 10 85 ? ? ? ? 0F 2E C6 7A 06 0F 84 ? ? ? ? 48 8D 8D ? ? ? ? E8 ? ? ? ? 
		ctx::offset::cg_tracecapsule = mem::Scanner::Scanner("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 50 48 63 84 24 ? ? ? ? 49 8B F9 C7 44 24 ? ? ? ? ? ", "cg_tracecapsule").getaddr();
		ctx::offset::cg_tracecapsule2 = mem::Scanner::Scanner("E8 ? ? ? ? F3 0F 10 3D ? ? ? ? 0F 2F 7D 40 0F 86 ? ? ? ? 48 8B 3D", "cg_tracecapsule2").resolve_call();


		ctx::offset::CL_Activeclienttest = mem::Scanner::Scanner("e8 ? ? ? ? 42 83 7c 37", "CL_Activeclienttest").resolve_call();
		ctx::offset::cl_testaddress = mem::Scanner::Scanner("E8 ? ? ? ? 48 8B F0 48 85 C0 0F 84 ? ? ? ? 8B 8B ? ? ? ? 0F 29 B4 24 ? ? ? ? 0F 29 BC 24 ? ? ? ?", "cl_testaddress").resolve_call();
		ctx::offset::jheadtest = mem::Scanner::Scanner("44 8b 25 ? ? ? ? 44 89 65 ? 45 85 e4", "jheadtest").resolve_mov();
		ctx::offset::jheadtest2 = mem::Scanner::Scanner("BlackOpsColdWar.exe","44 8b 05 ? ? ? ? 4c 8b ce", "jheadtest2").resolve_mov();
		/*ctx::offset::j_head = mem::Scanner::Scanner("BlackOpsColdWar.exe", "E8 ? ? ? ? EB 1E 4C 8B C3 48 8B D7 48 8B CE E8", "j_head").resolve_call();*/

		ctx::offset::cg_getentityworldtagpos = mem::Scanner::Scanner("BlackOpsColdWar.exe", "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 4C 8B 55 C8 66 83 BB ? ? ? ? ? 75 2F 8B 83 ? ? ? ? C1 E8 12 A8 01 74 22", "cg_getentityworldtagpos").resolve_call();

		ctx::offset::cl_patch = mem::Scanner::Scanner("48 69 d1 ? ? ? ? 48 03 d0 74 ? 80 ba ? ? ? ? ? 0f 94 c0", "cl_patch", 7).get_offset();
		ctx::offset::cl_viewangle = mem::Scanner::Scanner("E8 ? ? ? ? 84 C0 75 30 41 8B 84 24 ? ? ? ? 41 89 84 24 ? ? ? ? 41 8B 84 24 ? ? ? ? 41 89 84 24 ? ? ? ? ", "cl_viewangle", 0xD).get_offset();
		ctx::offset::Cent_size = mem::Scanner::Scanner("48 69 d9 ? ? ? ? 33 c9 48 03 d8 b8", "Cent_size", 3).get_offset();
		ctx::offset::Cent_pose = mem::Scanner::Scanner("48 81 c1 ? ? ? ? 44 8b 05", "Cent_pose", 3).get_offset();
		ctx::offset::Cent_number = mem::Scanner::Scanner("41 8b 97 ? ? ? ? 41 8b cc e8 ? ? ? ? 49 8b d7", "Cent_number", 3).get_offset();

		ctx::offset::CG_DObjGetWorldTagMatrix = mem::Scanner::Scanner("e8 ? ? ? ? 45 33 ed 84 c0 74 ? 49 8b 96", "CG_DObjGetWorldTagMatrix").resolve_call();
		ctx::offset::cg_getdobj = mem::Scanner::Scanner("e8 ? ? ? ? 48 85 c0 74 ? 49 8d 57", "cg_getdobj").resolve_call();
		//ctx::offset::livestorage_updateplayerinventory = mem::Scanner::Scanner("e8 ? ? ? ? 48 8b 5c 24 ? 48 8b 6c 24 ? 48 8b 74 24 ? 48 8b 7c 24 ? 48 83 c4 ? 41 5e c3 90 0c", "livestorage_updateplayerinventory").resolve_call();
		ctx::offset::sys_getutc = mem::Scanner::Scanner("e8 ? ? ? ? 44 8b c8 89 6c 24 ? 46 8d 04 33", "sys_getutc").resolve_call();
		ctx::offset::StringTable_GetAsset = mem::Scanner::Scanner("E8 ? ? ? ? 48 8B C8 4C 8B F8 E8 ? ? ? ? 49 8B CF 48 63 D8 41 8B F5 E8 ? ? ? ? 4C 63 F0 4C 8B E3 85 DB", "StringTable_GetAsset").resolve_call();
		ctx::offset::StringTable_GetAsset2 = mem::Scanner::Scanner("E8 ? ? ? ? 48 89 05 ? ? ? ? 48 8B C8 48 8B D8 48 85 C0 74 57 E8 ? ? ? ?", "StringTable_GetAsset2").resolve_call();
		ctx::offset::StringTable_GetColumnValueForRow = mem::Scanner::Scanner("e8 ? ? ? ? 48 8b 4d ? 48 89 8c 1e", "StringTable_GetColumnValueForRow").resolve_call();
		ctx::offset::Loot_GetitemQuantity = mem::Scanner::Scanner("48 89 5c 24 ? 57 48 83 ec ? 44 8b da", "Loot_GetitemQuantity").getaddr();
		ctx::offset::FindXAssetHeader = mem::Scanner::Scanner("E8 ? ? ? ? 48 8B CF FF C3", "FindXAssetHeader").resolve_call();
		ctx::offset::cl_getusercmd = mem::Scanner::Scanner("e8 ? ? ? ? 48 85 c0 74 ? 8b ce ba ? ? ? ? 83 e1", "cl_getusercmd").resolve_call();
		ctx::offset::cl_getusercmdnumber = mem::Scanner::Scanner("E8 ? ? ? ? 4C 8D 44 24 ? 8B D0 8B CB E8 ? ? ? ? ", "cl_getusercmdnumber").resolve_call();





		


		/////// Get Bone ids;
		 
		//ctx::offset::scr_string_start =  mem::Scanner::Scanner("8b 15 ? ? ? ? 44 8b 05 ? ? ? ? 44 8b 0d ? ? ? ? 8b 05 ? ? ? ? eb ? 8b 15", "sl_string_start").resolve_mov32(); //mov edx,[BlackOpsColdWar.exe+E607540]
		//if (ctx::offset::scr_string_start)
		//{
		//	ctx::offset::j_head			= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x64).getaddr();
		//	ctx::offset::j_neck			= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x574).getaddr();
		//	ctx::offset::j_spine4		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x58).getaddr();
		//	ctx::offset::j_spineupper	= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x594).getaddr();
		//	ctx::offset::j_spinelower	= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x590).getaddr();
		//	ctx::offset::j_mainroot		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x570).getaddr();
		//	ctx::offset::j_hip_ri		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x558).getaddr();
		//	ctx::offset::j_knee_ri		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x560).getaddr();
		//	ctx::offset::j_ankle_ri		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x514).getaddr();
		//	ctx::offset::j_ball_ri		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x51C).getaddr();
		//	ctx::offset::j_hip_le		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x554).getaddr();
		//	ctx::offset::j_knee_le		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x55C).getaddr();
		//	ctx::offset::j_ankle_le		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x510).getaddr();
		//	ctx::offset::j_ball_le		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x518).getaddr();
		//	ctx::offset::j_clavicle_le  = mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x520).getaddr();
		//	ctx::offset::j_shoulder_le  = mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x56C).getaddr();
		//	ctx::offset::j_elbow_le		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x534).getaddr();
		//	ctx::offset::j_clavicle_ri  = mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x524).getaddr();
		//	ctx::offset::j_shoulder_ri  = mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x588).getaddr();
		//	ctx::offset::j_elbow_ri		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x538).getaddr();
		//	ctx::offset::j_wrist_ri		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x584).getaddr();
		//	ctx::offset::j_wrist_le		= mem::Scanner::Scanner(ctx::offset::scr_string_start.cast() + 0x568).getaddr();



		//	
		//}

		j_head			= GameFunction::ScrStr_FindString("j_head");
		j_neck			= GameFunction::ScrStr_FindString("j_neck");
		j_spine4		= GameFunction::ScrStr_FindString("j_spine4");
		j_spineupper	= GameFunction::ScrStr_FindString("j_spineupper");
		j_spinelower   = GameFunction::ScrStr_FindString("j_spinelower");
		j_mainroot		= GameFunction::ScrStr_FindString("j_mainroot");
		j_hip_ri		= GameFunction::ScrStr_FindString("j_hip_ri");
		j_knee_ri		= GameFunction::ScrStr_FindString("j_knee_ri");
		j_ankle_ri		= GameFunction::ScrStr_FindString("j_ankle_ri");
		j_ball_ri		= GameFunction::ScrStr_FindString("j_ball_ri");
		j_hip_le		= GameFunction::ScrStr_FindString("j_hip_le");
		j_knee_le		= GameFunction::ScrStr_FindString("j_knee_le");
		j_ankle_le		= GameFunction::ScrStr_FindString("j_ankle_le");
		j_ball_le		= GameFunction::ScrStr_FindString("j_ball_le");
		j_clavicle_le  = GameFunction::ScrStr_FindString("j_clavicle_le");
		j_shoulder_le  = GameFunction::ScrStr_FindString("j_shoulder_le");
		j_elbow_le		= GameFunction::ScrStr_FindString("j_elbow_le");
		j_clavicle_ri	= GameFunction::ScrStr_FindString("j_clavicle_ri");
		j_shoulder_ri  = GameFunction::ScrStr_FindString("j_shoulder_ri");
		j_elbow_ri		= GameFunction::ScrStr_FindString("j_elbow_ri");
		j_wrist_ri		= GameFunction::ScrStr_FindString("j_wrist_ri");
		j_wrist_le		= GameFunction::ScrStr_FindString("j_wrist_le");
	
		//log::NotePad("id: %d\n", *ctx::offset::j_head.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_neck.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_spine4.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_spineupper.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_spinelower.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_mainroot.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_hip_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_hip_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_knee_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_knee_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_ball_le.cast<int*>()); 
		//log::NotePad("id: %d\n", *ctx::offset::j_ball_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_ankle_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_ankle_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_clavicle_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_clavicle_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_shoulder_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_shoulder_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_elbow_ri.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_elbow_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_wrist_le.cast<int*>());
		//log::NotePad("id: %d\n", *ctx::offset::j_wrist_ri.cast<int*>());


		//auto asset = GameFunction::StringTable_GetAsset2(0x60548737CB57046Bi64); //.rodata:0000000002717796	0000002A	C	gamedata/weapons/common/weaponoptions.csv
		//if (asset)
		//{
		//	auto num = *(int*)(asset + 0xC);
		//	log::NotePad("row size: %d\n", num);

		//	for (size_t i = 0; i < num; i++)
		//	{
		//		char* string;
		//		if(GameFunction::StringTable_GetColumnValueForRow(asset, &string, i,1))
		//		log::NotePad("%s\n", string);

		//	}
		//}
		
		/*auto time = GameFunction::Sys_GetUTC();
		for (size_t i = 0; i < 2500; i++)
		{
			 
			GameFunction::UpdatePlayerInventory(0, i, 0 , --time, 1, 1);
		}
		*/
	/*	
		for (size_t i = 0; i < 60; i++)
		{

			if (!ctx::offset::pmovehandler)
				break;

			if (ctx::offset::pmovehandler.cast<void**>()[i] == nullptr)
				continue;

			log::NotePad("id: %d ---- address: %x\n", i, ((uintptr_t)ctx::offset::pmovehandler.cast<void**>()[i] - (uintptr_t)GetModuleHandleA(0)));
		}*/

		/*MH_CreateHook(ctx::offset::Loot_GetitemQuantity.cast<void**>(), &GetItemQuantity_hook, 0);
		MH_EnableHook(ctx::offset::Loot_GetitemQuantity.cast<void**>());*/

		aimbot::initGamepad();

		if (D3D12::Initd3d12() == D3D12::Status::Success)
			D3D12::InstallHooks();

		
		auto wow64_prepare_for_exception = reinterpret_cast<wow64_prepare_for_exception_fn*>
		(address_t(reinterpret_cast<uintptr_t>(iat(GetProcAddress).get()(iat(GetModuleHandleA).get()(XOR("ntdll.dll")), XOR("KiUserExceptionDispatcher"))) + 1).self_jmp(0x3).cast());
		INIT_MEM_PROTECT_RW(wow64_prepare_for_exception, 8);
		o_wow64_prepare_for_exception = *wow64_prepare_for_exception;
		*wow64_prepare_for_exception = hk_wow64_prepare_for_exception;
	}
}