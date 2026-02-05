#pragma once
namespace ctx
{


	namespace config
	{
		namespace esp
		{
			extern bool b_showTeam;
			extern bool b_showAI;
			extern bool b_showMissile;
			extern bool b_showItem;
			extern bool b_showDistance;
			extern bool b_showHealth;
			extern bool b_showName;
			extern bool b_showWeapon;
			extern bool b_showBox;
			extern bool b_showBones;
			


			extern float max_distance;
			extern float max_ai_distance;
			extern float fov;
		}

		namespace itemesp
		{
			
		}

		namespace aimbot
		{
			extern bool enabled;
			extern bool Aim_at_ai;

			extern bool visible_only;
			extern bool silent;
			extern bool skip_knocked;
			extern bool self_reviving;

			extern uint32_t bone_priority;
			extern uint32_t aim_key;
			extern uint32_t target_mode;
			extern float  max_distance;
			extern float fov;
			extern float smoothness;

		}

		namespace misc
		{
			extern bool noflash;
			extern bool nostun;
			extern bool rapidfire;
			extern float f_recoilslider ;
		}

		namespace gamepad
		{
			extern int xbox_id;
			extern int ps_id;

		}

		namespace colors
		{
		

			extern ImColor AI_color;
			extern ImColor Missile_color;
			extern ImColor item_color;


			extern ImColor visible_team;
			extern ImColor not_visible_team;

			extern ImColor visible_box;
			extern ImColor not_visible_box;

			extern ImColor visible_distance;
			extern ImColor not_visible_distance;

			extern ImColor visible_name;
			extern ImColor not_visible_name;

			extern ImColor visible_Weapon;
			extern ImColor not_visible_Weapon;

			extern ImColor visible_Bone;
			extern ImColor not_visible_Bone;
		}

		namespace logging
		{
			extern uintptr_t exception;
			extern math::vec3_t _localpos;
			extern math::vec3_t pos;
			extern math::angle_t angle;
			extern	game_struct::trace_t* tr;
			extern int id;



		}

		void Load_Settings_byKey();
		void Save_Settings(std::string fileName);
		void Load_Settings(std::string fileName);
		void Auto_Load();
		std::vector<std::string> GetList();
	}

	
}