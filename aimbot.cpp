#include"includes.h"

namespace aimbot
{
	
	std::vector< entity_t* > m_targets, m_sorted_targets;

	entity_t* m_best_target;
	int				m_best_bone;
	math::vec3_t	m_best_pos;
	bool bUpdateTick = false;
	bool			m_disabled;
	const DWORD nTickTime = 64;//64 ms


	class Result
	{
	public:
		bool hasResult;
		float a;
		float b;
	};

	struct velocityInfo_t
	{
		math::vec3_t lastPos;
		math::vec3_t delta;
	};
	std::map<DWORD, velocityInfo_t> velocityMap;

	class CTargetPlayerManager {
	public:
		bool isReactionTime = false;
		bool isTimerStart = false;
		bool isBlockPlayer(int time = -1);
	};

	void start_tick()
	{
		static DWORD lastTick = 0;
		DWORD t = GetTickCount64();
		bUpdateTick = lastTick < t;

		if (bUpdateTick)
		lastTick = t + nTickTime;
	}
	DWORD lastTick = 0;
	int old_target = -1;

	CTargetPlayerManager tpManager = CTargetPlayerManager();

	bool CTargetPlayerManager::isBlockPlayer(int time)
	{
		//inits new tick
		DWORD t = GetTickCount64();

		// Used to reset the timer
		if (time != -1)
		{
			lastTick = time;
			old_target = -1;
			currentPlayer = -1;
			return false;
		}

		if (old_target == -1 && currentPlayer == -1)
			return false;

		if (old_target == -1 && currentPlayer != -1)
		{
			lastTick = t + *(DWORD*)&reactionTime;
			old_target = currentPlayer;
			return true;
		}

		//Reset the last tick time
		if (currentPlayer != old_target)
		{
			isReactionTime = lastTick < t;
			if (isReactionTime)
			{

				lastTick = t + *(DWORD*)&reactionTime;

				old_target = currentPlayer;
			}
			return isReactionTime; // when this become TRUE means switch target is permitted
		}

		if (currentPlayer == old_target)
		{
			// If is the same player means the blocking is true
			lastTick = t + *(DWORD*)&reactionTime;
			return true;
		}

		return false;
	}



	void update_vel_map(int index, math::vec3_t vPos)
	{
		if (!bUpdateTick)
			return;

		velocityMap[index].delta = vPos - velocityMap[index].lastPos;
		velocityMap[index].lastPos = vPos;
	}

	void clear_vel_map()
	{
		if (!velocityMap.empty()) { velocityMap.clear(); }
	}

	math::vec3_t get_speed(int index)
	{
		return velocityMap[index].delta;
	}


	math::vec3_t get_prediction(int index, math::vec3_t source, math::vec3_t bone, math::vec3_t position)
	{
		auto local_velocity = get_speed(ctx::client.ps->ClientNum);
		auto target_velocity = get_speed(index);

		const auto distance = source.distance_to(position);
		const auto travel_time = distance /6800.f;
		auto pred_destination = bone + (target_velocity - local_velocity) * travel_time;
		pred_destination.z += 0.001f /** m_to_units(9.8f)*/ * travel_time * travel_time;

		return pred_destination;
	}



	Result MidnightSolver(float a, float b, float c)
	{
		Result res;
		double subsquare = b * b - 4 * a * c;
		if (subsquare < 0)
		{
			res.hasResult = false;
			return res;
		}
		else
		{
			res.hasResult = true,
				res.a = (float)((-b + sqrt(subsquare)) / (2 * a));
			res.b = (float)((-b - sqrt(subsquare)) / (2 * a));
		}
		return res;
	}

	math::vec3_t prediction_solver(math::vec3_t local_pos, math::vec3_t position, int index, float bullet_speed)
	{
		math::vec3_t aimPosition = math::vec3_t();
		auto target_speed = get_speed(index);

		local_pos -= position;

		float a = (target_speed.x * target_speed.x) + (target_speed.y * target_speed.y) + (target_speed.z * target_speed.z) - ((bullet_speed * bullet_speed) * 100);
		float b = (-2 * local_pos.x * target_speed.x) + (-2 * local_pos.y * target_speed.y) + (-2 * local_pos.z * target_speed.z);
		float c = (local_pos.x * local_pos.x) + (local_pos.y * local_pos.y) + (local_pos.z * local_pos.z);

		local_pos += position;

		Result r = MidnightSolver(a, b, c);

		if (r.a >= 0 && !(r.b >= 0 && r.b < r.a))
		{
			aimPosition = position + target_speed * r.a;
		}
		else if (r.b >= 0)
		{
			aimPosition = position + target_speed * r.b;
		}

		return aimPosition;

	}


	void draw_fov(const float aimbot_fov)
	{
		//auto refdef = game_sdk::GetRefDef();
		//if (is_bad_ptr(refdef))return;
		//ImVec2 center = ImVec2(refdef->Width / 2, refdef->Height / 2);

		//game_sdk::draw::draw_circle(center, aimbot_fov, 0xFFFFFFFF, 100.0f);
	}


	void rect(const math::vec2_t& pos, const math::vec2_t& size, const col_t& col)
	{
		ImGui::GetBackgroundDrawList()->AddRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, ImColor(col.r(), col.g(), col.b(), col.a()));
	}

	float calculate_real_fov(const math::angle_t& local_angles, const math::vec3_t& local_pos, const math::vec3_t& target_pos)
	{
		const auto distance = (target_pos - local_pos).length();
		const auto angle_difference = (local_angles - math::calc_angle(local_pos, target_pos)).normalize();

		auto pitch_difference = sin(math::deg2rad(angle_difference.x)) * distance;
		auto yaw_difference = sin(math::deg2rad(angle_difference.y)) * distance;

		return sqrt(pitch_difference * pitch_difference + yaw_difference * yaw_difference);
	}


	float get_target_in_fov(math::vec2_t target_pos2, math::vec3_t target_pos)
	{
		//auto refdef = game_sdk::GetRefDef();
		//const float x = target_pos2.x - (refdef->Width / 2);
		//const float y = target_pos2.y - (refdef->Height / 2);
		//return sqrt(pow(y, 2) + pow(x, 2));
	}


	_declspec(noinline) bool is_visible(entity_t* pl, math::vec3_t start, math::vec3_t end)
	{

	}

	bool is_key_active()
	{
	
			if (ctx::config::aimbot::aim_key == 0)
				keyPressed = aimbotkey.isDown();

			if (ctx::config::aimbot::aim_key == 1 && aimbotkey.isPressed())
				 keyPressed = !keyPressed;
	

		return keyPressed;

	}

	bool is_valid_target(entity_t* ent)
	{

		

	}

	bool is_visible(math::vec3_t start, math::vec3_t end, int index)
	{
		//return GameFunction::Trace::LegacyTrace(start, end, index);
	}

	void reset_lock()
	{
		min_center_dist = init_center_dist;
	}

	std::vector<int> get_selected_bones(entity_t* pl)
	{
		std::vector<int> selected_bones = {};

		int selected_bone_group = ctx::config::aimbot::bone_priority;

		switch (selected_bone_group)
		{
		case 0:
			selected_bones = { j_head };
			break;
		case 1:
			selected_bones = { j_spine4, j_spineupper, j_spinelower, j_mainroot };
			break;
		case 2:
			selected_bones = { j_ball_le, j_knee_le };
			break;
		case 3:
			selected_bones = { j_ball_ri, j_knee_ri };
			break;
		case 4:
			for (int i = j_spinelower; i <= j_ball_ri; i++)
			{
				if (i != j_helmet)
					if (std::find(selected_bones.begin(), selected_bones.end(), i) == selected_bones.end())
						selected_bones.push_back(i);
			}
			break;
		default:
			selected_bones = { j_mainroot };
			break;
		}

		return selected_bones;
	}


	int XboxKey(int index)
	{
		switch (index)
		{
		case 0: return 4;
		case 1: return 5;
		case 2: return 8;
		case 3: return 9;

		default: return 4;
		}
	}

	int PlaystationKey(int index)
	{
		switch (index)
		{
		case 0: return 6;
		case 1: return 7;
		case 2: return 4;
		case 3: return 5;

		default: return 6;
		}
	}

	void onButtonDown(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) {


		if (xbox_controller && !found_controller)
		{

			if (buttonID == XboxKey(ctx::config::gamepad::xbox_id))
				controller_iskey_down = true;
		}

		if (found_controller && !xbox)
		{
			if (buttonID == PlaystationKey(ctx::config::gamepad::ps_id))
				controller_iskey_down = true;

		}
	}

	void onButtonUp(struct Gamepad_device* device, unsigned int buttonID, double timestamp, void* context) {

		if (xbox_controller && !found_controller)
		{
			if (buttonID == XboxKey(ctx::config::gamepad::xbox_id))
				controller_iskey_down = false;
		}

		if (found_controller && !xbox)
		{

			if (buttonID == PlaystationKey(ctx::config::gamepad::ps_id))
				controller_iskey_down = false;

		}
	}



	void onAxisMoved(struct Gamepad_device* device, unsigned int axisID, float value, float lastValue, double timestamp, void* context) {

	}

	void onDeviceAttached(struct Gamepad_device* device, void* context) {

		if (device->vendorID != 0)
			found_controller = true;

		if (device->vendorID == 0x45E)
		{
			found_controller = false;

			xbox_controller = true;

		}



		xbox = xbox_controller;
		ps = found_controller;






	}

	void onDeviceRemoved(struct Gamepad_device* device, void* context) {

		xbox = xbox_controller = false;
		ps = found_controller = false;

	}





	void initGamepad()
	{
		Gamepad_deviceAttachFunc(onDeviceAttached, (void*)0x1);
		Gamepad_deviceRemoveFunc(onDeviceRemoved, (void*)0x2);
		Gamepad_buttonDownFunc(onButtonDown, (void*)0x3);
		Gamepad_buttonUpFunc(onButtonUp, (void*)0x4);
		Gamepad_axisMoveFunc(onAxisMoved, (void*)0x5);
		Gamepad_init();
	}



	void init()
	{
	///*	if (config::get<bool>(ctx::cfg.targetting_disableonkill) && m_best_target && !m_best_target->is_alive())
	//	{
	//		m_disabled = true;
	//	}*/

	//	aim_target = -1;
	//	m_targets.clear();
	//	m_sorted_targets.clear();

	//	m_best_target = nullptr;
	//	m_best_bone = -1;
	//	m_best_pos = math::vec3_t();

	//	if (!is_key_active())
	//	{
	//		m_disabled = false;
	//	}


	}



	void sort()
	{
	/*	m_sorted_targets = m_targets;
		if (m_sorted_targets.size() == 0)
			return;

		/// Get camera angles
		math::angle_t local_angles = ctx::client.angle + ctx::client.ps->deltaAngle;
		local_angles.z = 0;

		/// Function to determine apprioriate delta
		const auto get_delta = [&](entity_t* ent) -> float {

			switch (ctx::config::aimbot::target_mode)
			{
			case 0:
				return calculate_real_fov(local_angles, ctx::client.local_entity->get_pos(), ent->get_pos());
			case 1:
				return (local_angles - math::calc_angle(ctx::client.local_entity->get_pos(), ent->get_pos())).normalize().length();
			case 2:
				return (ent->get_pos() - ctx::client.local_entity->get_pos()).length();
			default:
				return {};
			}
		};

		/// Sort our targets by the appropriate delta
		std::sort(m_sorted_targets.begin(), m_sorted_targets.end(), [&](auto& a, auto& b) {
			return get_delta(a) < get_delta(b);
			});*/
	}


	void find()
	{
		//if (m_sorted_targets.size() == 0)
		//	return;

		//start_tick();

		//math::angle_t local_angles = ctx::client.angle + ctx::client.ps->deltaAngle;
		//local_angles.z = 0;

		//math::vec3_t vieworg;
		//GameFunction::BG::BG_GetPlayerEyePosition(ctx::mem.Weaponmap.cast<uintptr_t>(), ctx::client.ps, &vieworg, GameFunction::CG::Get_CgHandler(game_struct::LocalClientNum_t::LOCAL_CLIENT_0));

		//for (auto target : m_sorted_targets)
		//{
		//	const auto selected_bones = get_selected_bones(target);

		//	for (auto bone : selected_bones)
		//	{
		//		update_vel_map(ctx::client.local_entity->nextstate().number, vieworg);

		//		math::vec3_t bone_pos;
		//		if (!ctx::mem.targetassist.cast<GameFunction::Vtable::CGTargetAssist*>()->GetWorldBonePos(target->nextstate().number,bone, &bone_pos))
		//			continue;

		//		update_vel_map(target->nextstate().number, bone_pos);


		//		if ((local_angles - math::calc_angle(vieworg, bone_pos)).normalize().length() > ctx::config::aimbot::fov)
		//			continue;

		//		bone_pos = get_prediction(target->nextstate().number, vieworg, bone_pos, target->get_pos());


		//	///*	bool override_key = input::get_key_info(config::get<int>(ctx::cfg.aimbot_override_key)).m_state > input::IDLE;*/
		//	//	if ((override_key && config::get<bool>(ctx::cfg.aimbot_override_visiblitycheck)) || (!override_key && config::get<bool>(ctx::cfg.targetting_visibilitycheck)) && !is_target_visible(target, vieworg, bone_pos))
		//	//		continue;

		//		if (ctx::config::aimbot::visible_only && !is_visible(vieworg,bone_pos,target->nextstate().number))
		//			continue;



		//		m_best_target = target;
		//		m_best_bone = bone;
		//		m_best_pos = bone_pos;
		//		return;
		//	}
		//}
	}



	void apply()
	{
		//math::vec3_t vieworg;
		//GameFunction::BG::BG_GetPlayerEyePosition(ctx::mem.Weaponmap.cast<uintptr_t>(), ctx::client.ps, &vieworg, GameFunction::CG::Get_CgHandler(game_struct::LocalClientNum_t::LOCAL_CLIENT_0));

		///// Have we found a player to target?
		//if (m_best_target && is_key_active() && !m_disabled)
		//{
		//	if (ctx::config::aimbot::enabled)
		//	{
		//		//rect({ 0 ,0 }, { 100,100 }, { 255,255,255,255 });

		//		auto angle = math::calc_angle(vieworg, m_best_pos).normalize();

		//		if (ctx::config::aimbot::smoothness > 1.f)
		//		{
		//			auto angle_delta = angle - (ctx::client.angle + ctx::client.ps->deltaAngle).normalize();
		//			angle_delta = angle_delta.normalize();

		//			angle = (ctx::client.angle + ctx::client.ps->deltaAngle).normalize() + angle_delta / ctx::config::aimbot::smoothness;
		//			angle = angle.normalize();
		//		}

		//		if (!ctx::config::aimbot::silent)
		//		{
		//			ctx::client.cl->SetViewAngles(math::angle_t(angle.x, angle.y, ctx::client.angle.z + ctx::client.ps->deltaAngle.z));
		//		}
		//		else
		//		{
		//			ctx::client.cmd->angle[0] = GameFunction::BG::BG_UsrCmdPackAngle(angle.x - ctx::client.ps->deltaAngle.x);
		//			ctx::client.cmd->angle[1] = GameFunction::BG::BG_UsrCmdPackAngle(angle.y - ctx::client.ps->deltaAngle.y);
		//		}

		//	/*	if (config::get<bool>(ctx::cfg.aimbot_autofire))
		//			ctx::client.cmd->buttons.add_flag(1);*/

		//		aim_target = m_best_target->nextstate().number;
		//	}
		//}

		
	}


	
	_declspec(noinline) void aim()
	{
		//init();

		//if (!game_sdk::in_game())
		//{
		//	clear_vel_map();
		//	return;

		//}

		//if (is_bad_ptr(ctx::client.cg))
		//	return;

		//if (is_bad_ptr(ctx::client.local_entity))
		//	return;

		//if (is_bad_ptr(ctx::client.cl))
		//	return;

		//if (is_bad_ptr(ctx::client.cmd))
		//	return;

	
		//// loop trough all player + AI 
		//game_sdk::for_every_player([](entity_t* pl) -> bool
		//	{
		//		if (!is_valid_target(pl))
		//			return false;

		//		m_targets.push_back(pl);

		//		return false;
		//});

		//// sort the targets
		//sort();

		////find the best target
		//find();

		//// aim at the best target
		//apply();
	}


	float GetMagnitude(Vector3 vec)
	{
		return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
	}

	Vector3 NormalizeVec(Vector3 vec)
	{
		float Length = GetMagnitude(vec);
		vec /= Length;
		return vec;
	}
#define D3DX_PI    ((FLOAT)  3.1415927f)

	Vector3 calculate_angle(Vector3 src, Vector3 dest, Vector3 ViewAxis[3])
	{
		Vector3 angles(0, 0, 0);
		Vector3 aimAt = dest - src;
		Vector3 normalized = NormalizeVec(aimAt);
		angles = Vector3();
		aimAt = normalized * ViewAxis[1];
		float yaw = (float)(asin(aimAt.x + aimAt.y + aimAt.z) * (180 / D3DX_PI));
		aimAt = normalized * ViewAxis[2];
		float pitch = (float)(-asin(aimAt.x + aimAt.y + aimAt.z) * (180 / D3DX_PI));

		angles.x = yaw;
		angles.y = pitch;
		angles.z = 0.0f;

		return angles;
	}


	bool get_target_in_fov(math::vec2_t target_pos2, Vector3 target_pos, int index)
	{
		//auto refdef = /*sdk*/::get_refdef();
		auto cam = GameFunction::AimAssist_GetClientGlobals();

		//if (is_bad_ptr(refdef))return false;
		const float x = target_pos2.x - (cam->Width	 / 2);
		const float y = target_pos2.y - (cam->Height / 2);
		float center_dis = sqrt(pow(y, 2) + pow(x, 2));
	

			if (center_dis < min_center_dist && center_dis <= ctx::config::esp::fov)
			{
				bestEntityId = index;
				min_center_dist = center_dis;
				lock_position_angle = target_pos;
				return true;
			}
		

		return false;
	}



	void lock_on_target()
	{
		Gamepad_processEvents();

		if (aimbotkey.isDown())
		{


			currentPlayer = bestEntityId;
			if (min_center_dist != init_center_dist)
			{
				ImGui::Text("loooooooooooool");
				AimbotTest(lock_position_angle);
			}
		}
	
	}


	void Reset()
	{
		target.clear();
		sorted_target.clear();
		best_target = nullptr;
		best_pos = {};
	}



	void AimbotTest(math::vec3_t aim_pos)
	{
		float angle_x;
		float angle_y;
		auto cl = game_sdk::Decryption::CL_ActiveClient();
		if (!cl)
			return;

		math::vec3_t vieworigin;
		GameFunction::CG_GetPlayerViewOrigin(game_struct::LOCAL_CLIENT_0, GameFunction::CG_GetPredictedPlayerState(), &vieworigin);

		auto cam = GameFunction::AimAssist_GetClientGlobals();

		math::vec3_t delta_angle = calculate_angle(vieworigin, aim_pos,cam->ViewAxis).normalize();

		GameFunction::CL_SetViewAngles(game_struct::LOCAL_CLIENT_0, { delta_angle.x,delta_angle.y ,0});


		////angle_x = *reinterpret_cast<float*>(cl + ctx::offset::cl_viewangle);
		////angle_y = *reinterpret_cast<float*>(cl + ctx::offset::cl_viewangle + 0x4);



		////auto new_angle = math::vec3_t(angle_x, angle_y, 0) + (delta_angle /= 1.f * 1.f);


		//*reinterpret_cast<float*>(cl + ctx::offset::cl_viewangle) = delta_angle.x;
		//*reinterpret_cast<float*>(cl + ctx::offset::cl_viewangle + 0x4) = delta_angle.y;

	}




	void Aim_Loop()
	{
		if (!GameFunction::Com_IsInGame())
			return;

		Reset();


		auto cg = GameFunction::CG_GetLocalClientGlobals();
		if (!cg)
			return;

		auto ps = (game_struct::playerState_t*)(GameFunction::CG_GetPredictedPlayerState());

		auto localclient = GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0, ps->ClientNum);


		for (size_t i = 0; i < 32; i++)
		{
			if (ps->ClientNum == i)
				continue;

			auto cent = CG_GetEntity(i);


			auto client = GameFunction::CG_GetClientInfo(game_struct::LOCAL_CLIENT_0,i);
			if (!client || client->Dead() || client->Team() == localclient->Team())
				continue;

			auto visible = GameFunction::AimTarget_IsTargetVisible_wrapper(game_struct::LOCAL_CLIENT_0, i);

			if (!visible)
				continue;

			auto dobj = GameFunction::CG_GetDObj(game_struct::LOCAL_CLIENT_0, cent);
			if (!dobj)
				continue;

			float matrix[64]{};
			math::vec3_t bonepos;

			if (!GameFunction::CG_DObjGetWorldTagMatrix(cent->Pose(), dobj, j_mainroot, matrix, &bonepos, 0xFFFFFFFFFFFFFFFF))
				continue;

			target.push_back(cent);

		}


		sorted_target = target;
		if (sorted_target.size() == 0)
			return;



		const auto get_delta = [&](entity_t* ent) -> float {


			return (ent->get_pos() - localclient->Origin()).length();

		};


		std::sort(sorted_target.begin(), sorted_target.end(), [&](auto& a, auto& b) {
			return get_delta(a) < get_delta(b);
			});


		if (sorted_target.size() == 0)
			return;


		for (auto current_target : sorted_target)
		{


			float matrix[64]{};
			math::vec3_t bonepos;
			auto dobj = GameFunction::CG_GetDObj(0, current_target);

			if (!GameFunction::CG_DObjGetWorldTagMatrix((DWORD*)(current_target->Pose()), dobj, j_mainroot, matrix, &bonepos, -1))
				continue;

			auto is_visible = GameFunction::AimTarget_IsTargetVisible_wrapper(game_struct::LOCAL_CLIENT_0, current_target->Number());
			if (!is_visible)
				continue;

			best_target = current_target;
			best_pos = bonepos;
			break;

		}

		Gamepad_processEvents();


		auto vieworigin = game_sdk::GetRefDef()->viewOrigin;
		auto angle = math::calc_angle(vieworigin, best_pos);

		angle = (angle - ps->delta_angle).normalize();

		if ((aimbotkey.isDown() || controller_iskey_down) && ctx::config::aimbot::enabled)
		{

			if (!ctx::config::aimbot::silent)
			{
				GameFunction::CL_SetViewAngles(game_struct::LOCAL_CLIENT_0, angle);
			}
			else
			{
				ctx::client.cmd->angle[0] = AngleToShort(angle.x);
				ctx::client.cmd->angle[1] = AngleToShort(angle.y);
			}
		
		}
		
	}
}