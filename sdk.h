#pragma once

class refdefKeyStruct {
public:
	DWORD ref0;      // 0x00
	DWORD ref1;      // 0x04
	DWORD ref2;		// 0x08
};

inline int j_helmet;
inline int j_head;
inline int j_neck;
inline int j_spine4;
inline int j_spineupper;
inline int j_spinelower;
inline int j_mainroot;
inline int j_hip_ri;
inline int j_knee_ri;
inline int j_ankle_ri;
inline int j_ball_ri;
inline int j_hip_le;
inline int j_knee_le;
inline int j_ankle_le;
inline int j_ball_le;
inline int j_clavicle_le;
inline int j_shoulder_le;
inline int j_elbow_le;
inline int j_wrist_le;
inline int j_clavicle_ri;
inline int j_shoulder_ri;
inline int j_elbow_ri;
inline int j_wrist_ri;

namespace game_sdk
{






	float units_to_m(float units);
	game_struct::refdef_t* GetRefDef();
	 bool WorldToEnemyBox(math::vec3_t WorldPos, math::vec2_t* ScreenPos, math::vec2_t* BoxSize);
	 bool IsVisible(math::vec3_t* start, math::vec3_t* end, int id);
	bool world_to_screen(const math::vec3_t& origin, math::vec2_t& screen);
	bool bones_to_screen(math::vec3_t* BonePosArray, math::vec2_t* ScreenPosArray, const long Count);
	bool w2s(const math::vec3_t& WorldPos, math::vec2_t* ScreenPos, math::vec2_t* BoxSize);
	void for_every_player(const std::function<bool(entity_t* pl)>& func, bitflag_t flags = {});
	bool in_game();
	bool in_game2();


	namespace Decryption
	{
		uintptr_t CG_GetEntity();
		uintptr_t CL_ActiveClient();

	}


	namespace draw
	{
		void draw_sketch_edge_text(ImFont* pFont, const std::string_view& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor = 0xFF000000);
		void draw_bone(math::vec2_t* bones_screenPos, long count, DWORD color);
		void draw_bone(int id, int bone_1, int bone_2, ImColor col);
		void draw_bones(unsigned long i, DWORD color);
		void draw_health(int i_health, math::vec3_t pos);
		void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color);
		void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness);
		void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness);
		void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness);

	}

}

namespace globals
{
	extern HMODULE dx9rot;;

}

