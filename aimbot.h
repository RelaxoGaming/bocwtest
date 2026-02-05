#pragma once
#define AngleToShort(a) ((int)((a)*(65536/360.0f))&65535)
namespace aimbot
{
	inline bool xbox_controller = false;
	inline bool found_controller = false;
	inline bool controller_iskey_down = false;
	using Vector3 = math::vec3_t;
	inline int bestEntityId = 0;

	inline constexpr float init_center_dist = 10000.0f;
	inline float min_center_dist = init_center_dist;
	inline float reactionTime = 1.0f; //150ms
    inline int currentPlayer = -1;
	inline int updateTime = 1;
	inline math::vec3_t lock_position_angle = Vector3(0, 0, 0);
	inline std::vector<entity_t* > target, sorted_target;
	inline entity_t* best_target;
	inline math::vec3_t best_pos;

	void AimbotTest(math::vec3_t aim_pos);
	void aim();
    void draw_fov(const float aimbot_fov);
	void Aim_Loop();
	void initGamepad();
}

