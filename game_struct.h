#pragma once
#include"includes.h"
#include"mem.h"
#include "includes.h"
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define AutoPadding(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
#define AutoPadding_0(type, name, offset) type name

#define MAKE_FUNCTION_OFFSET(offset,FunctionName,type) type&FunctionName(){return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this)+ (offset));}
#define MAKE_FUNCTION_OFFSET2(offset,FunctionName,type) type FunctionName(){return reinterpret_cast<type>(reinterpret_cast<uintptr_t>(this)+ (offset));}


#define MAKE_CHAR_ARRAY_FUNCTION_OFFSET(offset, FunctionName) char(&FunctionName())[32] { return *reinterpret_cast<char(*)[32]>(reinterpret_cast<uintptr_t>(this) + (offset));}



#define ShortToAngle(a) ((float)((a)*(360.0f/65536)))

namespace game_struct
{
	enum LocalClientNum_t
	{
		LOCAL_CLIENT_INVALID = 0xFFFFFFFF,
		LOCAL_CLIENT_0 = 0x0,
		LOCAL_CLIENT_1 = 0x1,
		LOCAL_CLIENT_LAST = 0x1,
		LOCAL_CLIENT_COUNT = 0x2,
	};

	class secured_vec3_t
	{
	private:
		int keys[4];

		__forceinline uintptr_t base(int idx) const
		{
			return reinterpret_cast<uintptr_t>(&keys[idx]);
		}
	public:
		math::vec3_t get() const
		{
			//cod vec3_t encryption
			int v42 = 0, v54[3] = {};
			v42 = keys[3];
			v54[0] = static_cast<int>(keys[0] ^ ((base(0) ^ v42) * ((base(0) ^ v42) + 2)));
			v54[1] = static_cast<int>(keys[1] ^ ((base(1) ^ v42) * ((base(1) ^ v42) + 2)));
			v54[2] = static_cast<int>(keys[2] ^ ((v42 ^ base(2)) * ((v42 ^ base(2)) + 2)));
			return math::vec3_t(*reinterpret_cast<float*>(&v54[0]), *reinterpret_cast<float*>(&v54[1]), *reinterpret_cast<float*>(&v54[2]));
		}

		void set(float x, float y, float z)
		{
			//cod vec3_t decryption
			int v42, v43, v54[3];
			v42 = keys[3];
			v54[0] = static_cast<int>(keys[0] ^ ((base(0) ^ v42) * ((base(0) ^ v42) + 2)));
			v54[1] = static_cast<int>(keys[1] ^ ((base(1) ^ v42) * ((base(1) ^ v42) + 2)));
			v54[2] = static_cast<int>(keys[2] ^ ((v42 ^ base(2)) * ((v42 ^ base(2)) + 2)));

			*reinterpret_cast<float*>(&v54[0]) = x;
			*reinterpret_cast<float*>(&v54[1]) = y;
			*reinterpret_cast<float*>(&v54[2]) = z;

			v43 = v42 + keys[4];
			keys[3] = v43;
			keys[0] = static_cast<int>(v54[0] ^ ((v43 ^ base(0)) * ((v43 ^ base(0)) + 2)));
			keys[1] = static_cast<int>(v54[1] ^ ((v43 ^ base(1)) * ((v43 ^ base(1)) + 2)));
			keys[2] = static_cast<int>(v54[2] ^ ((v43 ^ base(2)) * ((v43 ^ base(2)) + 2)));
		}

		void set(math::vec3_t value)
		{
			return set(value.x, value.y, value.z);
		}
	};

	enum ShockViewTypes : __int32
	{
		SHELLSHOCK_VIEWTYPE_BLURRED = 0x0,
		SHELLSHOCK_VIEWTYPE_FLASHED = 0x1,
		SHELLSHOCK_VIEWTYPE_PHOTO = 0x2,
		SHELLSHOCK_VIEWTYPE_NONE = 0x3,
	};

	struct unnamed_type_screenBlend
	{
		int blurredFadeTime;
		int blurredEffectTime;
		int flashWhiteFadeTime;
		int flashShotFadeTime;
		ShockViewTypes type;
	};

	struct unnamed_type_view
	{
		int fadeTime;
		float kickRate;
		float kickRadius;
	};

	struct unnamed_type_sound
	{
		bool affect;
		char loop[64];
		char loopSilent[64];
		char end[64];
		char endAbort[64];
		int fadeInTime;
		int fadeOutTime;
		char audioZone[64];
		int modEndDelay;
		int loopFadeTime;
		int loopEndDelay;
	};

	struct unnamed_type_lookControl
	{
		bool affect;
		int fadeTime;
		float mouseSensitivity;
		float maxPitchSpeed;
		float maxYawSpeed;
	};

	struct unnamed_type_movement
	{
		bool affect;
		bool breakSprint;
		bool breakJog;
	};

	struct unnamed_type_mount
	{
		bool affect;
	};

	struct shellshock_parms_t
	{
		unnamed_type_screenBlend screenBlend;
		unnamed_type_view view;
		unnamed_type_sound sound;
		unnamed_type_lookControl lookControl;
		unnamed_type_movement movement;
		unnamed_type_mount mount;
	};


	struct shellshock_t
	{
		shellshock_parms_t* parms;
		int startTime;
		int duration;
		float sensitivity;
		math::vec2_t viewDelta;
		int wantSavedScreen;
		int hasSavedScreen;
		int hasScissor;
		bool lookControlActive;
		int loopEndTime;
	};

	class aimassist_globals
	{
	public:

		char pad_0000[56]; //0x0000
		math::vec3_t origin; //0x0038
		char pad_0044[80]; //0x0044
		math::vec3_t vieworigin; //0x0094
		math::vec3_t ViewAxis[3]; //0x00A0
		math::vec2_t tanHalf; //0x00C4
		char pad_00CC[20]; //0x00CC
		float Height; //0x00E0
		float Width; //0x00E4
		char pad_00E8[2032]; //0x00E8

	}; //Size: 0x2BE0

	class refdef_t
	{
	public:
		char pad_0000[8]; //0x0000
		int32_t width; //0x0008
		int32_t height; //0x000C
		char pad_0010[20]; //0x0010
		float tanhalfX; //0x0024
		float tanhalfY; //0x0028
		char pad_002C[8]; //0x002C
		math::vec3_t viewOrigin; //0x0034
		math::vec3_t ViewAxis[3]; //0x0040
		char pad_0064[4176]; //0x0064
	}; //Size: 0x10B4

	enum RadarMode : __int8
	{
		RADAR_MODE_NORMAL = 0x0,
		RADAR_MODE_FAST = 0x1,
		RADAR_MODE_CONSTANT = 0x2,
		RADAR_MODE_SLOW = 0x3,
		RADAR_MODE_COUNT = 0x4,
	};


	struct Weapon_t
	{
		unsigned __int16 weaponIdx;
		unsigned __int16 stickerIndices[4];
		unsigned __int16 weaponClientLoadout;
		unsigned __int16 weaponOthers;
		unsigned __int8 weaponAttachments[13];
		unsigned __int8 attachmentVariationIndices[29];
		unsigned __int8 weaponCamo;
		unsigned __int8 weaponLootId;
		unsigned __int8 scopeVariation;
		unsigned __int8 visualAttachmentHighAddr;
	};

	struct StringTable {
		char pad[12];
		int rowCount;
	};

	struct  ScriptableDef
	{
		char pad[0x18];
		int flags; //0x18;
		unsigned int numParts;//0x14;


	};

	//struct ScriptableInstanceContextSecure
	//{
	//	ScriptableDef* def; // constains some usefull stuff we can use 
	//	char pad[0x38];
	//	unsigned int linkedObjectIndex; // Scriptable index
	//	char padding[0x24];
	//};


	struct ConstScriptableModelUnion
	{

	};

	struct  ScriptableInstanceContextSecure
	{
		ScriptableDef* def; //0x0
		void* originInitial; //0x8
		void* anglesInitial;//0x10
		math::vec3_t origin;//0x18


		//union 
		//{
		//	AutoPadding(vec3_t, origin, 0x18);
		//	AutoPadding(uint32_t, linkedObjectIndex, 0x40);
		//	AutoPadding(uint32_t, WeaponId, 0x4A); // or A8
		//	AutoPadding(ScriptableLinkType, linkType, 0x4D);

		//};
	};



	struct LerpEntityStateTypeUnion
	{

	};

	struct LerpEntityState_s
	{
		uint32_t nFlags; //0xC
		char m_pad[0x58];
		LerpEntityStateTypeUnion u; //0x5C
		char m_pad00[0x4];
	};

	enum entityType_s : __int16
	{
		ET_FIRST = 0x0,
		ET_GENERAL = 0x0,
		ET_PLAYER = 0x1,
		ET_PLAYER_CORPSE = 0x2,
		ET_ITEM = 0x3,
		ET_MISSILE = 0x4,
		ET_INVISIBLE = 0x5,
		ET_SCRIPTMOVER = 0x6,
		ET_SOUND = 0x7,
		ET_FX = 0x8,
		ET_LOOP_FX = 0x9,
		ET_PRIMARY_LIGHT = 0xA,
		ET_TURRET = 0xB,
		ET_HELICOPTER = 0xC,
		ET_PLANE = 0xD,
		ET_VEHICLE = 0xE,
		ET_VEHICLE_CORPSE = 0xF,
		ET_VEHICLE_SPAWNER = 0x10,
		ET_AGENT = 0x11,
		ET_AGENT_CORPSE = 0x12,
		ET_ACTOR = 0x13,
		ET_ACTOR_SPAWNER = 0x14,
		ET_ACTOR_CORPSE = 0x15,
		ET_PHYSICS_CHILD = 0x16,
		ET_BEAM = 0x17,
		ET_CLIENT_CHARACTER = 0x18,
		ET_RAGDOLL_CONSTRAINT = 0x19,
		ET_PHYSICS_VOLUME = 0x1A,
		ET_COVERWALL = 0x1B,
		ET_INFO_VOLUME_GRAPPLE = 0x1C,
		ET_EVENTS = 0x1D,
		ET_COUNT = 0x1E,
		ET_MAX = 0x100,
	};

	struct StaticEntityStateGeneral
	{
		unsigned int xmodel;
	};

	/* 18116 */
	struct BgWeaponHandle
	{
		unsigned int m_mapEntryId;
	};

	/* 23267 */
	struct StaticEntityStatePlayer
	{
		BgWeaponHandle offhandWeaponHandle;
		BgWeaponHandle stowedWeaponHandle;
	};

	/* 23268 */
	struct StaticEntityStateVehiclePlayer
	{
		unsigned __int8 playerIndex;
		unsigned __int8 teamId;
		unsigned __int8 pad[2];
	};

	/* 23269 */
	struct StaticEntityStateTurret
	{
		unsigned int xmodel;
		unsigned int carrierEntNum;
	};

	/* 23270 */

	/* 23271 */
	union StaticEntityStateMover
	{
		unsigned int scriptableIndex;
		unsigned int reflectionProbeIndex;
		int clientNum;
		unsigned int weaponIndex;
		unsigned int headModelIndex;
	};

	/* 23272 */


	/* 23273 */
	struct StaticEntityStatePhysicsVolume
	{
		int shapeID;
	};

	/* 23274 */
	struct StaticEntityStatePrimaryLight
	{
		unsigned int offset;
	};

	/* 23275 */
	struct StaticEntityStatePlane
	{
		unsigned int xmodel;
	};

	/* 23276 */
	struct StaticEntityStateCoverWall
	{
		unsigned int xmodel;
	};

	/* 23277 */
	struct StaticEntityStateMissile
	{
		unsigned int attackerEnt;
	};

	/* 23278 */
	struct StaticEntityStateAgentCorpse
	{
		BgWeaponHandle stowedWeaponHandle;
	};

	/* 23279 */
	struct StaticEntityStateFX
	{
		unsigned int index;
		int killEffectDef;
	};

	/* 23280 */
	struct StaticEntityStateAnonymous
	{
		int data[2];
	};






	//class playerState_t
	//{
	//public:
	//	char pad_0000[3584]; //0x0000
	//	math::angle_t delta_angle; //0x0E00
	//	char pad_0E0C[76]; //0x0E0C
	//	int ClientNum; //0x0E58
	//	char pad_0E5C[2604]; //0x0E5C

	//}; //Size: 0x20A4 

	class playerState_t
	{
	public:
		char pad_0000[3584]; //0x0000
		math::angle_t  delta_angle; //0x0E00
		math::angle_t  viewangle; //0x0E0C
		char pad_0E18[64]; //0x0E18
		int32_t ClientNum; //0x0E58
		char pad_0E5C[1588]; //0x0E5C
	}; //Size: 0x1490


	enum BRDataFlags : uint32_t
	{
		InGulag = 0x2 << 8,
		InGulagMatch = 0x4 << 8,
		Downed = 0x08 << 8,
		BeingRevived = 0x10 << 8,
		SelfReviving = 0x20 << 8,
	};


	class clientinfo_t {
	public:


			MAKE_FUNCTION_OFFSET(ctx::offset::CI_origin - ctx::offset::CI_offset, Origin, math::vec3_t)
			MAKE_FUNCTION_OFFSET(ctx::offset::CI_health - ctx::offset::CI_offset, Health, int)
			MAKE_FUNCTION_OFFSET(ctx::offset::CI_dead - ctx::offset::CI_offset, Dead, bool)
			MAKE_FUNCTION_OFFSET(ctx::offset::CI_team - ctx::offset::CI_offset, Team, int)
			MAKE_FUNCTION_OFFSET(0x80, Number, int)

			MAKE_CHAR_ARRAY_FUNCTION_OFFSET(ctx::offset::CI_name - ctx::offset::CI_offset, Name);
	};


	struct UserClientBits_t
	{
		unsigned int data[7];
		bool isVisible(int entIndex)
		{
			auto v9 = 0x80000000 >> (entIndex & 0x1F);
			auto v10 = entIndex >> 5;
			return data[v10] & v9;
		}
	};




	//struct usercmd_t
	//{
	//	//shared::bitflag_t  buttons; //0
	//	//UserCommandStateFlags    stateFlags; //8
	//	//int    serverTime; //12
	//	//int    commandTime; //16
	//	//int    inputTime; //20
	//	//int angles[3]; //24
	//	//Weapon weapon; //36
	//	//Weapon offHand; //96
	//	//signed char forwardmove; //156 
	//	//signed char rightmove; //157
	//	//signed char pitchmove; //158
	//	//char yawmove; //159
	//	//short meleeChargeEnt; //160 
	//	//char packedControllerAim; //162
	//	//char selectedLoc[2];  //163
	//	//char selectedAngle;   //165
	//	//char remoteControlAngles[2];    //166
	//	//char remoteControlMove[3];      //168
	//	//char mlgSelectedLoc[2];         //171
	//	//char mlgSelectedAngle;          //173
	//	//LightSample lightSample;        //174
	//	//short    avoidForward;           //176
	//	//short    avoidRight;
	//	//unsigned char    upmove; //unsigned char, which gives you at least the 0 to 255 range.
	//	//unsigned char    downmove;
	//	//bool    inputFromGamepad;
	//	//bool    isAlternate;
	//	//short    vehAngles[3];
	//	//int random; //NOT SURE IF GOES HERE
	//	//int random1; //NOT SURE IF GOES HERE
	//	//float    vehOrgX;
	//	//float    vehOrgY;
	//	//float    vehOrgZ;
	//	//float    gunPitch;
	//	//float    gunYaw;
	//	//float    gunXOfs;
	//	//float    gunYOfs;
	//	//float    gunZOfs;
	//	//float    rollSensitivity;
	//	//short    scriptedMeleeTarget;
	//	//short    airburstMarkDistance;
	//	//UserCmdClientBits sightedClientMask;
	//	//short    extrapolationMax;
	//	//short    interpolationMax;
	//	////float random1; //NOT SURE IF GOES HERE
	//	union
	//	{
	//		AutoPadding_0(shared::bitflag_t, buttons, 0x0);
	//		AutoPadding(int, servertime, 0x20);
	//		AutoPadding(int, commandtime, 0x24);
	//		AutoPadding(int, cmdAngle[3], 0x2C);
	//		AutoPadding(secured_vec3_t, vehangle, 0x128);

	//	};



	//};
	struct usercmd_t
	{
		char pad[0x40];
		int angle[3];
		math::angle_t GetAngle()
		{
			math::angle_t ret;

			ret.x = ShortToAngle(angle[0]);
			ret.y = ShortToAngle(angle[1]);
			ret.z = ShortToAngle(angle[2]);


			return ret;
		}

	};
	struct AimAssistGlobals_t
	{
		char      m_pad_0x160[0x160];
		secured_vec3_t    m_viewOrigin;
	};

	enum Physics_WorldId
	{
		PHYSICS_WORLD_ID_INVALID = 0xFFFFFFFF,
		PHYSICS_WORLD_ID_FIRST = 0x0,
		PHYSICS_WORLD_ID_SERVER_FIRST = 0x0,
		PHYSICS_WORLD_ID_SERVER_MAIN = 0x0,
		PHYSICS_WORLD_ID_SERVER_DETAIL = 0x1,
		PHYSICS_WORLD_ID_SERVER_LAST = 0x1,
		PHYSICS_WORLD_ID_CLIENT_FIRST = 0x2,
		PHYSICS_WORLD_ID_CLIENT0_FIRST = 0x2,
		PHYSICS_WORLD_ID_CLIENT0_PREDICTIVE = 0x2,
		PHYSICS_WORLD_ID_CLIENT0_AUTHORITATIVE = 0x3,
		PHYSICS_WORLD_ID_CLIENT0_DETAIL = 0x4,
		PHYSICS_WORLD_ID_CLIENT0_LAST = 0x4,
		PHYSICS_WORLD_ID_CLIENT1_FIRST = 0x5,
		PHYSICS_WORLD_ID_CLIENT1_PREDICTIVE = 0x5,
		PHYSICS_WORLD_ID_CLIENT1_AUTHORITATIVE = 0x6,
		PHYSICS_WORLD_ID_CLIENT1_DETAIL = 0x7,
		PHYSICS_WORLD_ID_CLIENT1_LAST = 0x7,
		PHYSICS_WORLD_ID_CLIENT_LAST = 0x7,
		PHYSICS_WORLD_ID_LAST = 0x7,
		PHYSICS_WORLD_ID_COUNT = 0x8,
	};

	enum TraceHitType : __int32
	{
		TRACE_HITTYPE_BEGIN = 0x0,
		TRACE_HITTYPE_NONE = 0x0,
		TRACE_HITTYPE_ENTITY = 0x1,
		TRACE_HITTYPE_DYNENT_MODEL = 0x2,
		TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
		TRACE_HITTYPE_GLASS = 0x4,
		TRACE_HITTYPE_SCRIPTABLE = 0x5,
		TRACE_HITTYPE_CLIENT_MODEL = 0x6,
		TRACE_HITTYPE_END = 0x6,
	};

	/* 1037 */
	enum TraceHitSubType : __int32
	{
		TRACE_HITSUBTYPE_NONE = 0x0,
		TRACE_HITSUBTYPE_COVERWALL = 0x1,
	};

	/* 962 */

	/* 21856 */
	struct TraceSubTypeData_CoverWall
	{
		unsigned __int16 id;
	};

	/* 21857 */
	union TraceHitSubTypeData
	{
		TraceSubTypeData_CoverWall coverWall;
	};

	/* 21858 */


	class trace_t
	{
	public:
		char pad_0000[16]; //0x0000
		float fraction; //0x0010
		char pad_0014[12]; //0x0014
		int32_t hitId; //0x0020
	}; //Size: 0x0080


}

