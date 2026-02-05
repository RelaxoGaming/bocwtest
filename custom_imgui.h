#pragma once


struct KeyBind;
namespace imgui_custom
{
	void hotkey(const char* label, KeyBind& key, float samelineOffset, const ImVec2& size);

}
