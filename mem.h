#pragma once


#define ADD_ADDRESS( name, mod_name, sig ) address_t name =  mem::find_ida_sig( mod_name, sig )
#define ADD_ADDRESS_FUNCTION( name, func ) address_t name =  func( );


namespace ctx
{
	struct mem_t
	{
		struct centity_t
		{

		}centity;

		struct CgGlobalsMP_t
		{

		} CgGlobalsMP;

		struct CharacterInfo_t
		{


		}CG_CharacterInfo;

		struct playerstate_Pointer
		{

		}playerstate_P;

		struct ClStatic_t 
		{

		}ClStatic;

		struct CLActiveClient_t
		{
			



		}CLActiveClient;



		struct GameFunction
		{


			




			

		}gamefunction;
		


   };

	
	extern mem_t mem;


	namespace offset
	{
		inline address_t Trampoline;
		inline address_t DVARBOOL_cl_packetdup;
		inline address_t DVARBOOL_cg_draw2d;

		inline address_t cg_predictedplayerstate;
		inline address_t CG_GetEntityState;
		inline address_t cgfunction;
		inline address_t com_sessionmode_getmode;
		inline address_t com_ingame;
		inline address_t unknown_CG_GetEntity;
		inline address_t CG_LocationalTrace;
		inline address_t tracetest;
		inline address_t cg_tracecapsule;
		inline address_t cg_tracecapsule2;
		inline address_t cl_getusercmd;
		inline address_t cl_getusercmdnumber;
		inline address_t sys_getutc;
		inline address_t livestorage_updateplayerinventory;


		inline address_t Trace_GetEntityHitId;

		inline address_t pmovehandler;
		inline address_t CG_GetEntityOriginAngles;
		inline address_t origin_test;
		inline address_t istargetvisible;
		inline address_t aimtarget_istargetvisible;

		inline address_t getviewaxisprojections;;
		inline address_t worldpostoscreenpos;
		inline address_t cg_getentityworldtagpos;
		inline address_t cg_getentityworldtagposangles;
		inline address_t CG_DObjGetWorldTagMatrix;
		inline address_t cg_getdobj;


		inline address_t cg_getclientinfo;
		inline address_t aimassist_getlocalclientglobals;
		inline address_t cg_getplayervieworigin;
		inline address_t cl_setviewangles;
		inline address_t sl_GetStringOfSize;
		inline address_t CL_Activeclienttest;
		inline address_t cl_testaddress;
		inline address_t jheadtest;
		inline address_t jheadtest2;

		inline address_t StringTable_GetColumnValueForRow	;
		inline address_t FindXAssetHeader;

		
		inline address_t StringTable_GetAsset;
		inline address_t StringTable_GetAsset2;
		inline address_t Loot_GetitemQuantity;


		inline address_32_t cl_patch;
		inline uint32_t nextstate;


		inline uint32_t cg_refdef;
		inline uint32_t cg_kickangles;
		inline uint32_t CI_size;
		inline uint32_t CI_offset;
		inline uint32_t CI_infovalid;
		inline uint32_t CI_health;
		inline uint32_t CI_team;
		inline uint32_t CI_dead;
		inline uint32_t CI_name;
		inline uint32_t CI_origin;
		inline uint32_t cl_viewangle;

		inline uint32_t Cent_size;
		inline uint32_t Cent_pose;
		inline uint32_t Cent_number;




		inline uint32_t predictedplayerstate_offset;
		inline uint32_t predictedplayerentity_offset;

		inline address_t scr_string_start;


		inline address_t j_head;
		inline address_t j_neck;
		inline address_t j_spine4;
		inline address_t j_spineupper;
		inline address_t j_spinelower;
		inline address_t j_mainroot;
		inline address_t j_hip_ri;
		inline address_t j_knee_ri;
		inline address_t j_ankle_ri;
		inline address_t j_ball_ri;
		inline address_t j_hip_le;
		inline address_t j_knee_le;
		inline address_t j_ankle_le;
		inline address_t j_ball_le;
		inline address_t j_clavicle_le;
		inline address_t j_shoulder_le;
		inline address_t j_elbow_le;
		inline address_t j_wrist_le;
		inline address_t j_clavicle_ri;
		inline address_t j_shoulder_ri;
		inline address_t j_elbow_ri;
		inline address_t j_wrist_ri;






	}

}


 

