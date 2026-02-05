#include"includes.h"

struct Vec4 {
public: union { float v[4]; struct { float x; float y; float z; float w; }; };
};
static Vec4 asVec4V(float x, float y, float z, float w) { Vec4 out; out.x = x; out.y = y; out.z = z; out.w = w; return out; }
struct Matrix4x4 { union { Vec4 v[4]; float m[4][4]; struct { Vec4 right; Vec4 up; Vec4 forward; Vec4 trans; }; }; };
typedef Matrix4x4 LinearTransform;




namespace game_sdk
{

    float units_to_m(float units) {
        return units * 0.0254;
    }

    game_struct::refdef_t* GetRefDef()
	{
        auto cg = GameFunction::CG_GetLocalClientGlobals();
        if (!cg)
            return{};

        return reinterpret_cast<game_struct::refdef_t*>(cg + ctx::offset::cg_refdef);
	}




    bool WorldToEnemyBox(math::vec3_t WorldPos, math::vec2_t* ScreenPos, math::vec2_t* BoxSize)
    {

        math::vec2_t screenpos1, screenpos2;
        if (!GameFunction::CG_WorldPosToScreenPos(0,&WorldPos, &screenpos1))
            return false;


        WorldPos.z += 65.f;
        if (!GameFunction::CG_WorldPosToScreenPos(0, &WorldPos, &screenpos2))
            return false;


        screenpos2.x -= (screenpos1.y - screenpos2.y) / 4;
        screenpos1.x += (screenpos1.y - screenpos2.y) / 4;

        ScreenPos->x = screenpos2.x;
        ScreenPos->y = screenpos2.y;
        BoxSize->x = screenpos1.x - screenpos2.x;
        BoxSize->y = screenpos1.y - screenpos2.y;


        return true;
    }

    bool IsVisible(math::vec3_t* start, math::vec3_t* end, int id)
    {
        auto ps = (game_struct::playerState_t*)GameFunction::CG_GetPredictedPlayerState();
        game_struct::trace_t trace;

        uintptr_t useless;
        math::vec3_t v3Zero = { 0.0f, 0.0f, 0.0f };
         GameFunction::CG_TraceCapsule(&trace, start, &v3Zero,&v3Zero,end, ps->ClientNum, 0x2818011,&useless); //0xA0E813 0x207801 0x809841  0x808000 byte_210211 0x818011 0xA1B401  0x2818011u, 0x281B401u,0x220011 A0EA13 115C02D0 206813
         /*ctx::config::logging::tr = &trace;*/

         ctx::config::logging::id = trace.hitId;/* GameFunction::Trace_GetEntityHitId(&trace);*/

         return ( trace.fraction >= 1.0);
    }


    float xangle(const math::vec3_t& LocalPos, const math::vec3_t& WorldPos)
    {
        float dl = sqrt((WorldPos.x - LocalPos.x) * (WorldPos.x - LocalPos.x) + (WorldPos.y - LocalPos.y) * (WorldPos.y - LocalPos.y));

        if (dl == 0.0f)
            dl = 1.0f;

        float dl2 = abs(WorldPos.x - LocalPos.x);
        float teta = ((180.0f / M_PI) * acos(dl2 / dl));

        if (WorldPos.x < LocalPos.x)
            teta = 180.0f - teta;

        if (WorldPos.y < LocalPos.y)
            teta = teta * -1.0f;

        if (teta > 180.0f)
            teta = (360.0f - teta) * (-1.0f);

        if (teta < -180.0f)
            teta = (360.0f + teta);

        return teta;
    }

    void rotation_point_alpha(float x, float y, float z, float alpha, math::vec3_t* outVec3)
    {

        typedef LinearTransform* (WINAPI* t_D3DXMatrixRotationY)(LinearTransform* pOut, FLOAT Angle);
        static t_D3DXMatrixRotationY D3DXMatrixRotationY = NULL;
        if (D3DXMatrixRotationY == NULL)
            D3DXMatrixRotationY = (t_D3DXMatrixRotationY)GetProcAddress(globals::dx9rot, XOR("D3DXMatrixRotationY")); // need to load lib

        typedef Vec4* (WINAPI* t_D3DXVec3Transform)(Vec4* pOut, CONST Vec4* pV, CONST LinearTransform* pM);
        static t_D3DXVec3Transform D3DXVec4Transform = NULL;
        if (D3DXVec4Transform == NULL)
            D3DXVec4Transform = (t_D3DXVec3Transform)GetProcAddress(globals::dx9rot, XOR("D3DXVec4Transform")); // need to load lib

        Matrix4x4 rot1;
        Vec4 vec = { x, z, y, 1.0f };
        D3DXMatrixRotationY((LinearTransform*)&rot1, alpha * M_PI / 180.0f);
        D3DXVec4Transform((Vec4*)&vec, (const Vec4*)&vec, (const LinearTransform*)&rot1);

        outVec3->x = vec.x;
        outVec3->y = vec.z;
        outVec3->z = vec.y;
    };
    bool bones_to_screen(math::vec3_t* BonePosArray, math::vec2_t* ScreenPosArray, const long Count)
    {
        for (long i = 0; i < Count; ++i)
        {
            if (!world_to_screen(BonePosArray[i], ScreenPosArray[i]))
                return false;
        }
        return true;
    }

    bool world_to_screen(const math::vec3_t& origin, math::vec2_t& screen)
    {
    

        //auto refdef_ptr = GetRefDef();
        //if (!refdef_ptr)
        //    return false;

        ////auto refdef = *reinterpret_cast<game_struct::refdef_t*>(refdef_ptr);

        //auto camera_location = refdef_ptr->viewOrigin;

        //math::vec3_t local = origin - camera_location; 
        //math::vec3_t trans = math::vec3_t(local.dot(refdef_ptr->ViewAxis[1]), local.dot(refdef_ptr->ViewAxis[2]), local.dot(refdef_ptr->ViewAxis[0]));

        //if (trans.z < 0.01f)
        //    return false;

        //screen.x = ((float)refdef_ptr->width/ 2) * (1 - (trans.x / refdef_ptr->tanhalfX / trans.z));
        //screen.y = ((float)refdef_ptr->height / 2) * (1 - (trans.y / refdef_ptr->tanhalfY / trans.z));
        //return true;



        auto camera = GameFunction::AimAssist_GetClientGlobals();
        if (!camera)
            return false;

        auto cg = GameFunction::CG_GetLocalClientGlobals();

   

       auto refdef_ptr = GetRefDef();

        //auto refdef = *reinterpret_cast<game_struct::refdef_t*>(refdef_ptr);

       math::vec3_t vieworigin;
       GameFunction::CG_GetPlayerViewOrigin(game_struct::LOCAL_CLIENT_0, GameFunction::CG_GetPredictedPlayerState(), &vieworigin);

       if (vieworigin.is_Zero())
           return false;


        //math::vec3_t local = origin - refdef_ptr->viewOrigin;
        //math::vec3_t trans = math::vec3_t(local.dot(camera->ViewAxis[0]), local.dot(camera->ViewAxis[2]), local.dot(camera->ViewAxis[1]));
       math::vec3_t projections;
       GameFunction::GetViewAxisProjections(&refdef_ptr->viewOrigin, refdef_ptr->ViewAxis, &origin, &projections);

        if (projections.z < 0.01f)
            return false;

        screen.x = ((float)refdef_ptr->width / 2) * (1 - (projections.x / refdef_ptr->tanhalfX / projections.z));
        screen.y = ((float)refdef_ptr->height/ 2) * (1 - (projections.y / refdef_ptr->tanhalfY / projections.z));
        return true;

    }

    bool w2s(const math::vec3_t& WorldPos, math::vec2_t* ScreenPos, math::vec2_t* BoxSize)
    {
       /* auto ViewOrig = ctx::mw2.aa_globalArray[game_struct::LocalClientNum_t::LOCAL_CLIENT_0]->m_viewOrigin.get();

        float angleX = xangle(ViewOrig, WorldPos);

        math::vec3_t posl, posr;

        rotation_point_alpha(-16.0f, 0.0f, 65.0f, -angleX + 90.0f, &posl);
        rotation_point_alpha(16.0f, 0.0f, -5.0f, -angleX + 90.0f, &posr);

        math::vec3_t vposl, vposr;

        vposl.x = WorldPos.x + posl.x;
        vposl.y = WorldPos.y + posl.y;
        vposl.z = WorldPos.z + posl.z;

        vposr.x = WorldPos.x + posr.x;
        vposr.y = WorldPos.y + posr.y;
        vposr.z = WorldPos.z + posr.z;

        math::vec2_t screenPosl, screenPosr;

        if (!world_to_screen(vposl, screenPosl) || !world_to_screen(vposr, screenPosr))
        {
            return false;
        }

        BoxSize->x = abs(screenPosr.x - screenPosl.x);
        BoxSize->y = abs(screenPosl.y - screenPosr.y);

        ScreenPos->x = screenPosr.x - BoxSize->x;
        ScreenPos->y = screenPosl.y;

        return true;*/
    }

    void for_every_player(const std::function<bool(entity_t* pl)>& func, bitflag_t flags)
    {

     /*   for (auto i = 0; i < 2047; i++)
        {
            auto player = CG_GetEntity(i);
            if (is_bad_ptr( player))
                continue;

            if (!player->is_alive())
                continue;
            
           
            if(ctx::client.local_entity == player)
                continue;

                  if (func(player))
                      break;
        }*/
    }


    bool in_game()
    {
        //return (*ctx::mem.FrontendCheck.cast<int*>()) == 0;
    }

    bool in_game2()
    {
        return  0;/**ctx::mem.GameMode.self_offset(0x198).cast<int*>() > 1;*/
    }


    namespace draw
    {
        void draw_sketch_edge_text(ImFont* pFont, const std::string_view& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor)
        {
            constexpr float fStrokeVal1 = 1.0f;
            ImGuiWindow* window = ImGui::GetCurrentWindow();

            float Edge_a = (EdgeColor >> 24) & 0xff;
            float Edge_r = (EdgeColor >> 16) & 0xff;
            float Edge_g = (EdgeColor >> 8) & 0xff;
            float Edge_b = (EdgeColor) & 0xff;
            std::stringstream steam(std::string(text), std::ios::in);
            std::string line;
            float y = 0.0f;
            int i = 0;

            if (!pFont)
                pFont = ImGui::GetDefaultFont();


            while (std::getline(steam, line))
            {
                ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
                if (center)
                {
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
                }
                else
                {
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x) - fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x) + fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
                    window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
                }
                y = pos.y + textSize.y * (i + 1);
                i++;
            }
        }

        void draw_bone(math::vec2_t* bones_screenPos, long count, DWORD color)
        {
            long last_count = count - 1;
            for (long i = 0; i < last_count; ++i)
                draw_line(ImVec2(bones_screenPos[i].x, bones_screenPos[i].y), ImVec2(bones_screenPos[i + 1].x, bones_screenPos[i + 1].y), color, 1.5f);
        }

        void draw_bone(int id, int bone_1, int bone_2, ImColor col)
        {
            math::vec3_t pos_bone_1, pos_bone_2, angle;
            math::vec2_t screen_bone_1, screen_bone_2;
            float random[64];

            auto cent = CG_GetEntity(id);
            auto dobj = GameFunction::CG_GetDObj(0, cent);
            if (!dobj)
                return;

            if (!GameFunction::CG_DObjGetWorldTagMatrix(cent->Pose(), dobj, bone_1, random, &pos_bone_1, -1))
                return;

            if (!GameFunction::CG_DObjGetWorldTagMatrix(cent->Pose(), dobj, bone_2, random, &pos_bone_2, -1))
                return;
             

            if (GameFunction::CG_WorldPosToScreenPos(game_struct::LOCAL_CLIENT_0, &pos_bone_1, &screen_bone_1) && GameFunction::CG_WorldPosToScreenPos(game_struct::LOCAL_CLIENT_0, &pos_bone_2, &screen_bone_2))
                draw_line(ImVec2(screen_bone_1.x, screen_bone_1.y), ImVec2(screen_bone_2.x, screen_bone_2.y), col, 1.5f);

        }



        void draw_bones(unsigned long id, DWORD color)
        {
             int LeftArm[] = { j_neck , j_clavicle_le , j_shoulder_le ,  j_elbow_le ,j_wrist_le };
             int rightArm[] = { j_neck,  j_clavicle_ri,  j_shoulder_ri, j_elbow_ri,  j_wrist_ri };

             int Body[] = { j_head,  j_neck, j_spine4,j_spineupper, j_spinelower, j_mainroot };

             int LeftLeg[] = { j_mainroot, j_hip_le,j_knee_le, j_ankle_le,j_ball_le };
             int RightLeg[] = { j_mainroot, j_hip_ri, j_knee_ri,j_ankle_ri, j_ball_ri };


            for (int i = 0; i < 5; i++)
            {
                draw_bone(id, Body[i], Body[i + 1], color);
            }

            //Arms
            for (int i = 0; i < 4; i++)
            {
                draw_bone(id, LeftArm[i], LeftArm[i + 1], color);
                draw_bone(id, rightArm[i], rightArm[i + 1],color);
            }
            //Legs
            for (int i = 0; i < 4; i++)
            {
                draw_bone(id, LeftLeg[i], LeftLeg[i + 1], color);
                draw_bone(id, RightLeg[i], RightLeg[i + 1], color);
            }

        }

        void draw_health(int i_health, math::vec3_t pos)
        {
            math::vec2_t bottom, top;

            if (!GameFunction::CG_WorldPosToScreenPos(0,&pos, &bottom))
                return;

            pos.z += 60;
            if (!GameFunction::CG_WorldPosToScreenPos(0, &pos, &top))

                return;

            top.y -= 5;
            auto height = top.y - bottom.y;
            auto width = height / 2.f;
            auto x = top.x - width / 1.8f;
            auto y = top.y;

            auto bar_width = max(0, min(150, i_health)) * (bottom.y - top.y) / 150.f;
            auto health = max(0, min(150, i_health));
            uint32_t color_health = ImGui::ColorConvertFloat4ToU32(ImVec4(0.000f, 1.000f, 0.000f, 1.000f));

            //fill_rectangle(x, y, 4, 127 * (bottom.y - top.y) / 127.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.000f, 0.000f, 0.000f, 1.000f)));
            fill_rectangle(x + 1, y + 1, 2, bar_width - 2, color_health);
        }

        void fill_rectangle(const float x, const float y, const float width, const float hight, const uint32_t color)
        {
            const float end_y = y + hight;
            auto black_color = ImGui::ColorConvertFloat4ToU32({ 0.f, 0.f, 0.f, 1.f });
            for (float curr_y = y; curr_y < end_y; ++curr_y)
                draw_line(ImVec2(x, curr_y), ImVec2(x + width, curr_y), color, 5.f);
        }

        void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness)
        {
            draw_line(ImVec2(x, y), ImVec2(x + width, y), color, thickness);
            draw_line(ImVec2(x, y), ImVec2(x, y + height), color, thickness);
            draw_line(ImVec2(x, y + height), ImVec2(x + width, y + height), color, thickness);
            draw_line(ImVec2(x + width, y), ImVec2(x + width, y + height), color, thickness);
        }

        void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
        {
            auto window = ImGui::GetWindowDrawList();
            window->AddLine(from, to, color, thickness);
        }

        void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness)
        {
            float step = (float)M_PI * 2.0f / thickness;
            for (float a = 0; a < (M_PI * 2.0f); a += step)
            {
                draw_line(
                    ImVec2(radius * cosf(a) + position.x, radius * sinf(a) + position.y),
                    ImVec2(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y),
                    color,
                    1.5f
                );
            }
        }


    }
}
     
namespace globals
{
     HMODULE dx9rot;

}

uintptr_t game_sdk::Decryption::CG_GetEntity()
{
    auto peb = __readgsqword(0x60);
    auto baseModuleAddr = ctx::data::base;
    uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
    rax = *(uintptr_t*)(baseModuleAddr + 0x101abcd0);
    if (!rax)
        return rax;
    rbx = peb;              //mov rbx, gs:[rcx]
    //failed to translate: mov [rsp+0x90], rdi
    rcx = rbx;              //mov rcx, rbx
    rcx >>= 0x17;           //shr rcx, 0x17
    //failed to translate: mov [rsp+0x88], r12
    rcx &= 0xF;
    switch (rcx) {
    case 0:
    {
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                 //mov r9, [0x000000000299F659]
        rcx = 0xBED7B7906279197C;               //mov rcx, 0xBED7B7906279197C
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xB5E2F49C4AB752FD;               //mov rcx, 0xB5E2F49C4AB752FD
        rax *= rcx;             //imul rax, rcx
        rcx = 0x832963058443FA63;               //mov rcx, 0x832963058443FA63
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x21;           //shr rcx, 0x21
        rax ^= rcx;             //xor rax, rcx
        rax -= rbx;             //sub rax, rbx
        rax -= rbx;             //sub rax, rbx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        return rax;
    }
    case 1:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299F13E]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CAAFF0]
        rax ^= rbx;             //xor rax, rbx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xF92DB4D9DE5493D5;               //mov rcx, 0xF92DB4D9DE5493D5
        rax *= rcx;             //imul rax, rcx
        rcx = rax + r11 * 1;            //lea rcx, [rax+r11*1]
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rdx ^= r10;             //xor rdx, r10
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0x9);                 //mov rax, [rdx+0x09]
        rax *= rcx;             //imul rax, rcx
        rcx = 0x2B3162F02A44EE69;               //mov rcx, 0x2B3162F02A44EE69
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x2;            //shr rcx, 0x02
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x4;            //shr rcx, 0x04
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x4E7933D768D921BA;               //mov rcx, 0x4E7933D768D921BA
        rax += rcx;             //add rax, rcx
        return rax;
    }
    case 2:
    {
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                 //mov r9, [0x000000000299ECFE]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CAABB0]
        rax -= rbx;             //sub rax, rbx
        rcx = 0x3B17FB2CD7095BB1;               //mov rcx, 0x3B17FB2CD7095BB1
        rax ^= rcx;             //xor rax, rcx
        rax -= r11;             //sub rax, r11
        rcx = 0xC00A737BD6258E2D;               //mov rcx, 0xC00A737BD6258E2D
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x3D3A86FBD37A2637;               //mov rcx, 0x3D3A86FBD37A2637
        rax *= rcx;             //imul rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x27;           //shr rcx, 0x27
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        return rax;
    }
    case 3:
    {
        //failed to translate: pop rax
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                 //mov r9, [0x000000000299E870]
        r12 = baseModuleAddr + 0x6771BCDF;              //lea r12, [0x000000005C3C6401]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x22;           //shr rcx, 0x22
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x3D4517FCEC05209;                //mov rcx, 0x3D4517FCEC05209
        rax *= rcx;             //imul rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rcx = 0x44E0DA30A68391DD;               //mov rcx, 0x44E0DA30A68391DD
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1C;           //shr rcx, 0x1C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x38;           //shr rcx, 0x38
        rax ^= rcx;             //xor rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx *= r12;             //imul rcx, r12
        rax -= rcx;             //sub rax, rcx
        rax -= rbx;             //sub rax, rbx
        rcx = 0x473AF3AC5F1AE221;               //mov rcx, 0x473AF3AC5F1AE221
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    case 4:
    {
        //failed to translate: pop rax
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                 //mov r9, [0x000000000299E3CF]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CAA281]
        rcx = 0x6524D44AA1DE6841;               //mov rcx, 0x6524D44AA1DE6841
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1F;           //shr rcx, 0x1F
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x3E;           //shr rcx, 0x3E
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xAB5D8DF18ACB8034;               //mov rcx, 0xAB5D8DF18ACB8034
        rax ^= rcx;             //xor rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rax ^= r11;             //xor rax, r11
        rax ^= rbx;             //xor rax, rbx
        rcx = baseModuleAddr + 0x424;           //lea rcx, [0xFFFFFFFFF4CAA49B]
        rax += rbx;             //add rax, rbx
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x9;            //shr rcx, 0x09
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x12;           //shr rcx, 0x12
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x24;           //shr rcx, 0x24
        rax ^= rcx;             //xor rax, rcx
        return rax;
    }
    case 5:
    {
        //failed to translate: pop rbx
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299DF50]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA9DF7]
        rcx = 0x612833258D1E3078;               //mov rcx, 0x612833258D1E3078
        rax ^= rcx;             //xor rax, rcx
        rcx = rbx;              //mov rcx, rbx
        uintptr_t RSP_0x77;
        RSP_0x77 = baseModuleAddr + 0x6055;             //lea rcx, [0xFFFFFFFFF4CAFE63] : RBP+0x77
        rcx *= RSP_0x77;                //imul rcx, [rbp+0x77]
        rax += rcx;             //add rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rcx = 0xB030CFC771DB4259;               //mov rcx, 0xB030CFC771DB4259
        rax *= rcx;             //imul rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx -= r11;             //sub rcx, r11
        rcx -= 0xFA35;          //sub rcx, 0xFA35
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x2B50DC517231252A;               //mov rcx, 0x2B50DC517231252A
        rax -= rcx;             //sub rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1B;           //shr rcx, 0x1B
        rax ^= rcx;             //xor rax, rcx
        rdx = rax;              //mov rdx, rax
        rdx >>= 0x36;           //shr rdx, 0x36
        rdx ^= rax;             //xor rdx, rax
        rax = baseModuleAddr + 0x9BA8;          //lea rax, [0xFFFFFFFFF4CB37A9]
        rax *= rbx;             //imul rax, rbx
        rax += rdx;             //add rax, rdx
        return rax;
    }
    case 6:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299DA69]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA991B]
        r12 = baseModuleAddr + 0x4AB6;          //lea r12, [0xFFFFFFFFF4CAE3C6]
        rcx = r12;              //mov rcx, r12
        rcx = ~rcx;             //not rcx
        rcx ^= rbx;             //xor rcx, rbx
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x11;           //shr rcx, 0x11
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x22;           //shr rcx, 0x22
        rax ^= rcx;             //xor rax, rcx
        rax += r11;             //add rax, r11
        rcx = rbx;              //mov rcx, rbx
        rcx -= r11;             //sub rcx, r11
        rcx += 0xFFFFFFFF9A74DEE4;              //add rcx, 0xFFFFFFFF9A74DEE4
        rax += rcx;             //add rax, rcx
        rcx = 0x5CD87638E3565B73;               //mov rcx, 0x5CD87638E3565B73
        rax *= rcx;             //imul rax, rcx
        rcx = 0xE32E844EFE6B6AF2;               //mov rcx, 0xE32E844EFE6B6AF2
        rax ^= rcx;             //xor rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rcx = *(uintptr_t*)(rcx + 0x9);                 //mov rcx, [rcx+0x09]
        rax *= rcx;             //imul rax, rcx
        rcx = 0x6C090F2BDB9D15C3;               //mov rcx, 0x6C090F2BDB9D15C3
        rax += rcx;             //add rax, rcx
        return rax;
    }
    case 7:
    {
        //failed to translate: pop rbx
        r11 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r11, [0x000000000299D635]
        r13 = baseModuleAddr + 0x454FEED5;              //lea r13, [0x000000003A1A83BC]
        rdx = baseModuleAddr + 0x5AA1D32E;              //lea rdx, [0x000000004F6C67AD]
        rax ^= rbx;             //xor rax, rbx
        rax ^= r13;             //xor rax, r13
        r8 = 0;                 //and r8, 0xFFFFFFFFC0000000
        rcx = rbx;              //mov rcx, rbx
        rcx ^= rdx;             //xor rcx, rdx
        r8 = _rotl64(r8, 0x10);                 //rol r8, 0x10
        rdx = rax;              //mov rdx, rax
        r8 ^= r11;              //xor r8, r11
        rdx -= rcx;             //sub rdx, rcx
        r8 = ~r8;               //not r8
        rax = *(uintptr_t*)(r8 + 0x9);          //mov rax, [r8+0x09]
        rax *= rdx;             //imul rax, rdx
        rax ^= rbx;             //xor rax, rbx
        rcx = 0x63DE07A3AFCB6B1A;               //mov rcx, 0x63DE07A3AFCB6B1A
        rax -= rcx;             //sub rax, rcx
        rcx = 0x5FDDA3BDA6032573;               //mov rcx, 0x5FDDA3BDA6032573
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x6;            //shr rcx, 0x06
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xC;            //shr rcx, 0x0C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x18;           //shr rcx, 0x18
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x30;           //shr rcx, 0x30
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xBCC2B28792FB25D5;               //mov rcx, 0xBCC2B28792FB25D5
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    case 8:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299D08B]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA8F3D]
        r12 = baseModuleAddr + 0x35D77EE7;              //lea r12, [0x000000002AA20E19]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x21;           //shr rcx, 0x21
        rax ^= rcx;             //xor rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx = ~rcx;             //not rcx
        rax ^= rcx;             //xor rax, rcx
        rax ^= r12;             //xor rax, r12
        rdx = rbx;              //mov rdx, rbx
        rdx -= r11;             //sub rdx, r11
        rcx = rax + r11 * 1;            //lea rcx, [rax+r11*1]
        rax = rdx + 0xffffffffffff37b4;                 //lea rax, [rdx-0xC84C]
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rax = 0xA830E25B74D384A5;               //mov rax, 0xA830E25B74D384A5
        rcx *= rax;             //imul rcx, rax
        rax = baseModuleAddr + 0x3C686054;              //lea rax, [0x000000003132EEEE]
        rcx += rbx;             //add rcx, rbx
        rax = ~rax;             //not rax
        rax += rcx;             //add rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rcx = 0x112C075B696E9FC8;               //mov rcx, 0x112C075B696E9FC8
        rax += rcx;             //add rax, rcx
        return rax;
    }
    case 9:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299CC78]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA8B2A]
        r13 = baseModuleAddr + 0x704E2E7D;              //lea r13, [0x000000006518B99C]
        rdx = baseModuleAddr + 0x4C0A1E24;              //lea rdx, [0x0000000040D4A90C]
        rcx = rbx;              //mov rcx, rbx
        rcx *= rdx;             //imul rcx, rdx
        rax -= rcx;             //sub rax, rcx
        rax ^= rbx;             //xor rax, rbx
        rax ^= rbx;             //xor rax, rbx
        rax ^= r13;             //xor rax, r13
        rcx = 0x4EB1FE8FF2DB3173;               //mov rcx, 0x4EB1FE8FF2DB3173
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x18;           //shr rcx, 0x18
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x30;           //shr rcx, 0x30
        rax ^= rcx;             //xor rax, rcx
        rax += r11;             //add rax, r11
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rax ^= r11;             //xor rax, r11
        return rax;
    }
    case 10:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299C882]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA8734]
        r13 = baseModuleAddr + 0x151C;          //lea r13, [0xFFFFFFFFF4CA9C45]
        rdx = rbx;              //mov rdx, rbx
        rcx = rax;              //mov rcx, rax
        rcx -= r11;             //sub rcx, r11
        rdx *= r13;             //imul rdx, r13
        rax = rdx;              //mov rax, rdx
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x8C739DBB7BE3B7DE;               //mov rcx, 0x8C739DBB7BE3B7DE
        rax += rcx;             //add rax, rcx
        rcx = 0xED4B99114C277137;               //mov rcx, 0xED4B99114C277137
        rax ^= rcx;             //xor rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rcx = *(uintptr_t*)(rcx + 0x9);                 //mov rcx, [rcx+0x09]
        rax *= rcx;             //imul rax, rcx
        rax ^= r11;             //xor rax, r11
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x28;           //shr rcx, 0x28
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x595E85B3F431880B;               //mov rcx, 0x595E85B3F431880B
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    case 11:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299C3BA]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA826C]
        rcx = 0x291D85CC47C692D1;               //mov rcx, 0x291D85CC47C692D1
        rax -= rcx;             //sub rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x11;           //shr rcx, 0x11
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x22;           //shr rcx, 0x22
        rax ^= rcx;             //xor rax, rcx
        rdx = rbx;              //mov rdx, rbx
        rdx = ~rdx;             //not rdx
        rdx -= r11;             //sub rdx, r11
        rcx = rax;              //mov rcx, rax
        rax = 0x2E943C71EC9ED887;               //mov rax, 0x2E943C71EC9ED887
        rcx *= rax;             //imul rcx, rax
        rax = rdx + 0xffffffffffff016f;                 //lea rax, [rdx-0xFE91]
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x3;            //shr rcx, 0x03
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x6;            //shr rcx, 0x06
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xC;            //shr rcx, 0x0C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x18;           //shr rcx, 0x18
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x30;           //shr rcx, 0x30
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x45455F12787ECCC7;               //mov rcx, 0x45455F12787ECCC7
        rax += rcx;             //add rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        return rax;
    }
    case 12:
    {
        //failed to translate: pop rbx
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299BD8B]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA7C3D]
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rcx = *(uintptr_t*)(rcx + 0x9);                 //mov rcx, [rcx+0x09]
        rax *= rcx;             //imul rax, rcx
        rax -= rbx;             //sub rax, rbx
        rax -= r11;             //sub rax, r11
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x23;           //shr rcx, 0x23
        rcx ^= rbx;             //xor rcx, rbx
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xCC369E6C25CE53BD;               //mov rcx, 0xCC369E6C25CE53BD
        rax *= rcx;             //imul rax, rcx
        rcx = 0xD00F98BA2F9B44C5;               //mov rcx, 0xD00F98BA2F9B44C5
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        return rax;
    }
    case 13:
    {
        //failed to translate: pop rbx
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                 //mov r9, [0x000000000299B914]
        r15 = baseModuleAddr + 0x4EC0;          //lea r15, [0xFFFFFFFFF4CAC686]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xF;            //shr rcx, 0x0F
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1E;           //shr rcx, 0x1E
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x3C;           //shr rcx, 0x3C
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x8C38342BFF20E5C5;               //mov rcx, 0x8C38342BFF20E5C5
        rax -= rbx;             //sub rax, rbx
        rax *= rcx;             //imul rax, rcx
        rax ^= rbx;             //xor rax, rbx
        rax ^= r15;             //xor rax, r15
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xA;            //shr rcx, 0x0A
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x14;           //shr rcx, 0x14
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x28;           //shr rcx, 0x28
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x311A564CF063B0DB;               //mov rcx, 0x311A564CF063B0DB
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x10DEB1734981111F;               //mov rcx, 0x10DEB1734981111F
        rax += rcx;             //add rax, rcx
        return rax;
    }
    case 14:
    {
        //failed to translate: pop rax
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299B388]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA723A]
        r13 = baseModuleAddr + 0x4EA0;          //lea r13, [0xFFFFFFFFF4CAC0CF]
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0x9);                //imul rax, [rcx+0x09]
        rax ^= r11;             //xor rax, r11
        rcx = r13;              //mov rcx, r13
        rcx = ~rcx;             //not rcx
        rcx ^= rbx;             //xor rcx, rbx
        rax -= rcx;             //sub rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx -= r11;             //sub rcx, r11
        rcx -= 0xF9D2;          //sub rcx, 0xF9D2
        rax ^= rcx;             //xor rax, rcx
        rax -= r11;             //sub rax, r11
        rcx = 0x2D51D1F14DFDF0DD;               //mov rcx, 0x2D51D1F14DFDF0DD
        rax *= rcx;             //imul rax, rcx
        rcx = 0x43C565FC122AA94D;               //mov rcx, 0x43C565FC122AA94D
        rax -= rcx;             //sub rax, rcx
        return rax;
    }
    case 15:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDCF413B);                //mov r10, [0x000000000299AF99]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF4CA6E4B]
        r15 = baseModuleAddr + 0x1A886606;              //lea r15, [0x000000000F52D446]
        rcx = rbx + rax * 1;            //lea rcx, [rbx+rax*1]
        rax = baseModuleAddr + 0xE35D;          //lea rax, [0xFFFFFFFFF4CB4EB8]
        rcx += rax;             //add rcx, rax
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rdx ^= r10;             //xor rdx, r10
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0x9);                 //mov rax, [rdx+0x09]
        rax *= rcx;             //imul rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx ^= r15;             //xor rcx, r15
        rax -= rcx;             //sub rax, rcx
        rax += r11;             //add rax, r11
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1E;           //shr rcx, 0x1E
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x3C;           //shr rcx, 0x3C
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x9A94EEA98BD3246A;               //mov rcx, 0x9A94EEA98BD3246A
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xE39687EC7026B211;               //mov rcx, 0xE39687EC7026B211
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xC6082C24125570AD;               //mov rcx, 0xC6082C24125570AD
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    }
}


uintptr_t game_sdk::Decryption::CL_ActiveClient()
{
    auto peb = __readgsqword(0x60);
    auto baseModuleAddr = ctx::data::base;
    uint64_t rax = baseModuleAddr, rbx = baseModuleAddr, rcx = baseModuleAddr, rdx = baseModuleAddr, rdi = baseModuleAddr, rsi = baseModuleAddr, r8 = baseModuleAddr, r9 = baseModuleAddr, r10 = baseModuleAddr, r11 = baseModuleAddr, r12 = baseModuleAddr, r13 = baseModuleAddr, r14 = baseModuleAddr, r15 = baseModuleAddr;
    rax = *(uintptr_t*)(baseModuleAddr + 0x101E9FF8);
    if (!rax)
        return rax;
    rbx = peb;              //mov rbx, gs:[rcx]
    rcx = rbx;              //mov rcx, rbx
    //failed to translate: mov [rsp+0xC0], r12
    rcx = _rotl64(rcx, 0x24);               //rol rcx, 0x24
    rcx &= 0xF;
    switch (rcx) {
    case 0:
    {
        //failed to translate: pop rax
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x000000000744BC5B]
        r12 = baseModuleAddr + 0x51A1E2E5;              //lea r12, [0x000000004B13CDA6]
        rax -= rbx;             //sub rax, rbx
        uintptr_t RSP_0xFFFFFFFFFFFFFFF7;
        RSP_0xFFFFFFFFFFFFFFF7 = baseModuleAddr + 0xB798;               //lea rcx, [0xFFFFFFFFF972A229] : RBP+0xFFFFFFFFFFFFFFF7
        rax += RSP_0xFFFFFFFFFFFFFFF7;          //add rax, [rbp-0x09]
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rcx = *(uintptr_t*)(rcx + 0xf);                 //mov rcx, [rcx+0x0F]
        rcx *= rax;             //imul rcx, rax
        rax = rcx;              //mov rax, rcx
        rax >>= 0x23;           //shr rax, 0x23
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x37BD69660A314861;               //mov rcx, 0x37BD69660A314861
        rax -= rcx;             //sub rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx *= r12;             //imul rcx, r12
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x26;           //shr rcx, 0x26
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x1452291CC58E40A9;               //mov rcx, 0x1452291CC58E40A9
        rax *= rcx;             //imul rax, rcx
        rcx = 0x148A077EEB084E5F;               //mov rcx, 0x148A077EEB084E5F
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    case 1:
    {
        //failed to translate: pop rbx
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x000000000744B8A2]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971E708]
        r12 = baseModuleAddr + 0x8BF5;          //lea r12, [0xFFFFFFFFF97272F2]
        rcx = rbx;              //mov rcx, rbx
        rcx ^= r12;             //xor rcx, r12
        rax -= rcx;             //sub rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x28;           //shr rcx, 0x28
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x6612FCE0340E4453;               //mov rcx, 0x6612FCE0340E4453
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x19;           //shr rcx, 0x19
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x32;           //shr rcx, 0x32
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xB99AA5C843CAF77F;               //mov rcx, 0xB99AA5C843CAF77F
        rax *= rcx;             //imul rax, rcx
        rax += r11;             //add rax, r11
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        return rax;
    }
    case 2:
    {
        //failed to translate: pop rax
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x000000000744B441]
        r12 = baseModuleAddr + 0x47D41D47;              //lea r12, [0x000000004145FFE9]
        r15 = baseModuleAddr + 0x2EF560DC;              //lea r15, [0x0000000028674373]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xD;            //shr rcx, 0x0D
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1A;           //shr rcx, 0x1A
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x34;           //shr rcx, 0x34
        rax ^= rcx;             //xor rax, rcx
        rcx = r15;              //mov rcx, r15
        rcx = ~rcx;             //not rcx
        rcx ^= rbx;             //xor rcx, rbx
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x389A025D62152FEF;               //mov rcx, 0x389A025D62152FEF
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x15;           //shr rcx, 0x15
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x2A;           //shr rcx, 0x2A
        rax ^= rcx;             //xor rax, rcx
        uintptr_t RSP_0x77;
        RSP_0x77 = 0xDC1425B348BD21BB;          //mov rcx, 0xDC1425B348BD21BB : RBP+0x77
        rax *= RSP_0x77;                //imul rax, [rbp+0x77]
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rdx ^= r10;             //xor rdx, r10
        rcx = rbx;              //mov rcx, rbx
        rcx *= r12;             //imul rcx, r12
        rdx = ~rdx;             //not rdx
        rcx ^= rax;             //xor rcx, rax
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        rcx = 0xAEECF1988461B67;                //mov rcx, 0xAEECF1988461B67
        rax -= rcx;             //sub rax, rcx
        return rax;
    }
    case 3:
    {
        //failed to translate: pop rax
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x000000000744AF93]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971DDF9]
        r13 = baseModuleAddr + 0x18286398;              //lea r13, [0x00000000119A4186]
        rax ^= r11;             //xor rax, r11
        rcx = rbx;              //mov rcx, rbx
        rcx = ~rcx;             //not rcx
        rcx ^= r13;             //xor rcx, r13
        rax += rcx;             //add rax, rcx
        rcx = 0x431C785228682F45;               //mov rcx, 0x431C785228682F45
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x5;            //shr rcx, 0x05
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xA;            //shr rcx, 0x0A
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x14;           //shr rcx, 0x14
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x28;           //shr rcx, 0x28
        rax ^= rcx;             //xor rax, rcx
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rcx = rax;              //mov rcx, rax
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rax = 0xE2A31FF336327DD3;               //mov rax, 0xE2A31FF336327DD3
        rcx ^= rax;             //xor rcx, rax
        rdx ^= r10;             //xor rdx, r10
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        rax += rbx;             //add rax, rbx
        return rax;
    }
    case 4:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x000000000744AB00]
        rcx = baseModuleAddr + 0x91C2;          //lea rcx, [0xFFFFFFFFF972698F]
        rcx = ~rcx;             //not rcx
        rcx *= rbx;             //imul rcx, rbx
        rax += rcx;             //add rax, rcx
        rdx = rbx;              //mov rdx, rbx
        rcx = baseModuleAddr + 0x15041A6E;              //lea rcx, [0x000000000E75F0E3]
        rcx = ~rcx;             //not rcx
        rdx = ~rdx;             //not rdx
        rdx += rcx;             //add rdx, rcx
        rax ^= rdx;             //xor rax, rdx
        rcx = 0x307FD2F1D69A8DCA;               //mov rcx, 0x307FD2F1D69A8DCA
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1A;           //shr rcx, 0x1A
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x34;           //shr rcx, 0x34
        rax ^= rcx;             //xor rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r10;             //xor rcx, r10
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        rcx = 0x95E5E1573CC0BDAD;               //mov rcx, 0x95E5E1573CC0BDAD
        rax *= rcx;             //imul rax, rcx
        rcx = 0x138376EFF27CFD9D;               //mov rcx, 0x138376EFF27CFD9D
        rax *= rcx;             //imul rax, rcx
        rax += rbx;             //add rax, rbx
        return rax;
    }
    case 5:
    {
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                 //mov r9, [0x000000000744A61E]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971D484]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1D;           //shr rcx, 0x1D
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x3A;           //shr rcx, 0x3A
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x18E1CC56EB3B012B;               //mov rcx, 0x18E1CC56EB3B012B
        rax ^= r11;             //xor rax, r11
        rax *= rcx;             //imul rax, rcx
        rcx = 0xB7ABEE7C905DCF26;               //mov rcx, 0xB7ABEE7C905DCF26
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1;            //shr rcx, 0x01
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x2;            //shr rcx, 0x02
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x4;            //shr rcx, 0x04
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x1B042FBF4C87FEFC;               //mov rcx, 0x1B042FBF4C87FEFC
        rax ^= rcx;             //xor rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        return rax;
    }
    case 6:
    {
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                 //mov r9, [0x0000000007449FE7]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971CE48]
        rax -= rbx;             //sub rax, rbx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x5;            //shr rcx, 0x05
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xA;            //shr rcx, 0x0A
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x14;           //shr rcx, 0x14
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x28;           //shr rcx, 0x28
        rax ^= rcx;             //xor rax, rcx
        rax ^= rbx;             //xor rax, rbx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        rcx = 0xFFFFFFFFE2EB9D31;               //mov rcx, 0xFFFFFFFFE2EB9D31
        rcx -= rbx;             //sub rcx, rbx
        rcx -= r11;             //sub rcx, r11
        rax += rcx;             //add rax, rcx
        rcx = 0xF9B68EBA8F5B9F85;               //mov rcx, 0xF9B68EBA8F5B9F85
        rax *= rcx;             //imul rax, rcx
        rcx = 0x479C4C6741E98AEB;               //mov rcx, 0x479C4C6741E98AEB
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x5916AA12798F25B;                //mov rcx, 0x5916AA12798F25B
        rax -= rcx;             //sub rax, rcx
        return rax;
    }
    case 7:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x0000000007449B7B]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971C9E1]
        rcx = r11 + 0x44fc;             //lea rcx, [r11+0x44FC]
        rcx += rbx;             //add rcx, rbx
        rax += rcx;             //add rax, rcx
        rcx = 0x7063161D985C7581;               //mov rcx, 0x7063161D985C7581
        rax -= rcx;             //sub rax, rcx
        rax -= rbx;             //sub rax, rbx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x21;           //shr rcx, 0x21
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xD8A03AAB8DFAEC3;                //mov rcx, 0xD8A03AAB8DFAEC3
        rax *= rcx;             //imul rax, rcx
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rcx = rax;              //mov rcx, rax
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rcx -= r11;             //sub rcx, r11
        rdx ^= r10;             //xor rdx, r10
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        rcx = 0x2B3A1F4E6CC2F613;               //mov rcx, 0x2B3A1F4E6CC2F613
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    case 8:
    {
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                 //mov r9, [0x0000000007449628]
        r15 = baseModuleAddr + 0xB249;          //lea r15, [0xFFFFFFFFF97276D7]
        rcx = r15;              //mov rcx, r15
        rcx -= rbx;             //sub rcx, rbx
        rax ^= rcx;             //xor rax, rcx
        rcx = baseModuleAddr + 0x259DC51E;              //lea rcx, [0x000000001F0F8800]
        rax -= rbx;             //sub rax, rbx
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x2;            //shr rcx, 0x02
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x4;            //shr rcx, 0x04
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xB26AC9727D62A283;               //mov rcx, 0xB26AC9727D62A283
        rax *= rcx;             //imul rax, rcx
        rcx = 0x86B99554A2F4A5F0;               //mov rcx, 0x86B99554A2F4A5F0
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x4;            //shr rcx, 0x04
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x8;            //shr rcx, 0x08
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x10;           //shr rcx, 0x10
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x413A2176C8ECB8E0;               //mov rcx, 0x413A2176C8ECB8E0
        rax += rcx;             //add rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        return rax;
    }
    case 9:
    {
        //failed to translate: pop rax
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                 //mov r9, [0x0000000007449043]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971BEA9]
        r15 = baseModuleAddr + 0x7D4C184C;              //lea r15, [0x0000000076BDD6EA]
        rcx = 0xF681326FB1C232F1;               //mov rcx, 0xF681326FB1C232F1
        rax ^= rcx;             //xor rax, rcx
        rax ^= r11;             //xor rax, r11
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        rcx = r15;              //mov rcx, r15
        rcx *= rbx;             //imul rcx, rbx
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x684C0A2F8DDFF863;               //mov rcx, 0x684C0A2F8DDFF863
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x9;            //shr rcx, 0x09
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x12;           //shr rcx, 0x12
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x24;           //shr rcx, 0x24
        rax ^= rcx;             //xor rax, rcx
        rcx = 0xEE5A74905667430B;               //mov rcx, 0xEE5A74905667430B
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x6;            //shr rcx, 0x06
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xC;            //shr rcx, 0x0C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x18;           //shr rcx, 0x18
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x30;           //shr rcx, 0x30
        rax ^= rcx;             //xor rax, rcx
        return rax;
    }
    case 10:
    {
        //failed to translate: pop rbx
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x0000000007448A9E]
        r13 = baseModuleAddr + 0x7C8C0CE8;              //lea r13, [0x0000000075FDC5E7]
        r12 = baseModuleAddr + 0x3A276AEB;              //lea r12, [0x00000000339923DF]
        rcx = rbx + r12 * 1;            //lea rcx, [rbx+r12*1]
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x6D57BD432668FFD3;               //mov rcx, 0x6D57BD432668FFD3
        rax *= rcx;             //imul rax, rcx
        rcx = r13;              //mov rcx, r13
        rcx = ~rcx;             //not rcx
        rcx ^= rbx;             //xor rcx, rbx
        rax -= rcx;             //sub rax, rcx
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rcx = baseModuleAddr + 0x254B8F0F;              //lea rcx, [0x000000001EBD4420]
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rcx += rbx;             //add rcx, rbx
        rcx ^= rax;             //xor rcx, rax
        rdx ^= r10;             //xor rdx, r10
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x24;           //shr rcx, 0x24
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x24411B0DC5ACE4A7;               //mov rcx, 0x24411B0DC5ACE4A7
        rax -= rcx;             //sub rax, rcx
        return rax;
    }
    case 11:
    {
        //failed to translate: pop rax
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                 //mov r9, [0x0000000007448591]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971B3F7]
        rax += r11;             //add rax, r11
        rax -= rbx;             //sub rax, rbx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xE;            //shr rcx, 0x0E
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1C;           //shr rcx, 0x1C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x38;           //shr rcx, 0x38
        rax ^= rcx;             //xor rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        rcx = 0xE72200AA412876C7;               //mov rcx, 0xE72200AA412876C7
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x2EFDB3CD64A4A26D;               //mov rcx, 0x2EFDB3CD64A4A26D
        rax *= rcx;             //imul rax, rcx
        rax -= rbx;             //sub rax, rbx
        rcx = 0xABC9A2698AB2B686;               //mov rcx, 0xABC9A2698AB2B686
        rax ^= rcx;             //xor rax, rcx
        return rax;
    }
    case 12:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x000000000744811B]
        r12 = baseModuleAddr + 0xC323;          //lea r12, [0xFFFFFFFFF97272A4]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1E;           //shr rcx, 0x1E
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x3C;           //shr rcx, 0x3C
        rcx ^= rax;             //xor rcx, rax
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rdx ^= r10;             //xor rdx, r10
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xE;            //shr rcx, 0x0E
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1C;           //shr rcx, 0x1C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x38;           //shr rcx, 0x38
        rax ^= rcx;             //xor rax, rcx
        rax += rbx;             //add rax, rbx
        rcx = 0xD92CCEDADD6FDB6B;               //mov rcx, 0xD92CCEDADD6FDB6B
        rax *= rcx;             //imul rax, rcx
        rcx = 0x39CBE2A6C4408779;               //mov rcx, 0x39CBE2A6C4408779
        rax *= rcx;             //imul rax, rcx
        rdx = rbx;              //mov rdx, rbx
        rdx = ~rdx;             //not rdx
        rcx = rax;              //mov rcx, rax
        rax = 0xB75799D9B89F6F39;               //mov rax, 0xB75799D9B89F6F39
        rax *= rcx;             //imul rax, rcx
        rdx *= r12;             //imul rdx, r12
        rax += rdx;             //add rax, rdx
        return rax;
    }
    case 13:
    {
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x0000000007447BD8]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971AA3E]
        rcx = rbx;              //mov rcx, rbx
        rcx -= r11;             //sub rcx, r11
        rcx += 0xFFFFFFFFFFFF06E6;              //add rcx, 0xFFFFFFFFFFFF06E6
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xE;            //shr rcx, 0x0E
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x1C;           //shr rcx, 0x1C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x38;           //shr rcx, 0x38
        rax ^= rcx;             //xor rax, rcx
        rcx = rbx;              //mov rcx, rbx
        rcx ^= rax;             //xor rcx, rax
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rax = 0x6EDC82FF371D329D;               //mov rax, 0x6EDC82FF371D329D
        rcx *= rax;             //imul rcx, rax
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rax = 0xA655B808416895AF;               //mov rax, 0xA655B808416895AF
        rcx ^= rax;             //xor rcx, rax
        rdx ^= r10;             //xor rdx, r10
        rcx -= r11;             //sub rcx, r11
        rax = 0x2F57FF4488E86CBA;               //mov rax, 0x2F57FF4488E86CBA
        rcx += rax;             //add rcx, rax
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        return rax;
    }
    case 14:
    {
        //failed to translate: pop rbx
        r10 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                //mov r10, [0x0000000007447760]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971A5C6]
        r13 = baseModuleAddr + 0x660F387C;              //lea r13, [0x000000005F80DE37]
        rcx = r13;              //mov rcx, r13
        rcx ^= rbx;             //xor rcx, rbx
        rax += rcx;             //add rax, rcx
        rcx = 0x4AB4F8E476AEB292;               //mov rcx, 0x4AB4F8E476AEB292
        rax += rcx;             //add rax, rcx
        rdx = 0;                //and rdx, 0xFFFFFFFFC0000000
        rdx = _rotl64(rdx, 0x10);               //rol rdx, 0x10
        rcx = rax;              //mov rcx, rax
        rdx ^= r10;             //xor rdx, r10
        rcx -= r11;             //sub rcx, r11
        rdx = ~rdx;             //not rdx
        rax = *(uintptr_t*)(rdx + 0xf);                 //mov rax, [rdx+0x0F]
        rax *= rcx;             //imul rax, rcx
        rdx = baseModuleAddr + 0x91B7;          //lea rdx, [0xFFFFFFFFF9723729]
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x6;            //shr rcx, 0x06
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xC;            //shr rcx, 0x0C
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x18;           //shr rcx, 0x18
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x30;           //shr rcx, 0x30
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x75875EE5B2475EFD;               //mov rcx, 0x75875EE5B2475EFD
        rax *= rcx;             //imul rax, rcx
        rax -= rbx;             //sub rax, rbx
        rcx = rbx;              //mov rcx, rbx
        rcx = ~rcx;             //not rcx
        rcx ^= rdx;             //xor rcx, rdx
        rax -= rcx;             //sub rax, rcx
        return rax;
    }
    case 15:
    {
        //failed to translate: pop rbx
        r9 = *(uintptr_t*)(baseModuleAddr + 0xDD2D187);                 //mov r9, [0x000000000744726D]
        r11 = baseModuleAddr;           //lea r11, [0xFFFFFFFFF971A0D3]
        rcx = rbx;              //mov rcx, rbx
        rcx -= r11;             //sub rcx, r11
        rax += rcx;             //add rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x20;           //shr rcx, 0x20
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x5;            //shr rcx, 0x05
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0xA;            //shr rcx, 0x0A
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x14;           //shr rcx, 0x14
        rax ^= rcx;             //xor rax, rcx
        rcx = rax;              //mov rcx, rax
        rcx >>= 0x28;           //shr rcx, 0x28
        rax ^= rcx;             //xor rax, rcx
        rcx = 0x7778BC64809B2E73;               //mov rcx, 0x7778BC64809B2E73
        rax *= rcx;             //imul rax, rcx
        rcx = 0x496FB35290D562CF;               //mov rcx, 0x496FB35290D562CF
        rax -= rcx;             //sub rax, rcx
        rcx = 0x55C195008AE27F63;               //mov rcx, 0x55C195008AE27F63
        rax += rcx;             //add rax, rcx
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= r9;              //xor rcx, r9
        rcx = ~rcx;             //not rcx
        rax *= *(uintptr_t*)(rcx + 0xf);                //imul rax, [rcx+0x0F]
        return rax;
    }
    }
}



