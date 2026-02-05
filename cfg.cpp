#include"includes.h"
namespace ctx
{


	namespace config
	{
		namespace esp
		{
			bool b_showTeam = false;
			bool b_showAI = true;
			bool b_showMissile = false;
			bool b_showItem = false;
			 bool b_showDistance = true;
			 bool b_showHealth = true;
			 bool b_showName = true;
			 bool b_showWeapon = true;
			 bool b_showBox = true;
			 bool b_showBones = true;

			 extern float fov = 100.f;
			float max_ai_distance = 200.f;
			 float max_distance = 1000.f;

		}

		namespace misc
		{
			float f_recoilslider{1.0f};
			bool noflash;
			bool nostun;
			bool rapidfire;
		}

		namespace colors
		{

			ImColor red(255, 0, 0);         // red
			ImColor green(0, 255, 0);       // green
			ImColor blue(0, 0, 255);        // blue
			ImColor white(255, 255, 255);   // white
			ImColor black(0, 0, 0);         // black
			ImColor yellow(255, 255, 0);    // yellow
			ImColor magenta(255, 0, 255);   // magenta
			ImColor cyan(0, 255, 255);      // cyan


			ImColor AI_color = yellow;
			ImColor Missile_color = white;
			ImColor item_color = white;

			 ImColor visible_team = blue;
			 ImColor not_visible_team = cyan;

			 ImColor visible_box = green;
			 ImColor not_visible_box = white;

			 ImColor visible_distance = green;
			 ImColor not_visible_distance = white;

			 ImColor visible_name =magenta;
			 ImColor not_visible_name = magenta;
			

			 ImColor visible_Weapon = magenta;
			 ImColor not_visible_Weapon= magenta;

			 ImColor visible_Bone = red;
			 ImColor not_visible_Bone = red;
		}


		namespace logging
		{
			 math::vec3_t _localpos;
			 math::vec3_t pos;
			 math::angle_t angle;
			 game_struct::trace_t* tr;
			 int id = 0;
			 uintptr_t exception;
		}


		BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
			char lpString[1024];
			sprintf(lpString, "%d", nInteger);
			return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
		}
		BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
			char lpString[1024];
			sprintf(lpString, "%f", nInteger);
			return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
		}
		float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
		{
			char szData[32];

			GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

			return (float)atof(szData);
		}


		void Save_Settings(std::string fileName) {
			char file_path[MAX_PATH];
			auto filepath = std::format("C:\\\\DefaultPath\\\\config\\\\{}{}", fileName.c_str(), XOR(".ini"));
			//sprintf_s(file_path, xorstr("C:\\DefaultPath\\config\\\\%s%s"), fileName.c_str(), xorstr(".ini"));

			WritePrivateProfileFloat(XOR("Misc"), XOR("Recoil_Slider"), misc::f_recoilslider, filepath.c_str());

		}

		void Load_Settings(std::string fileName)
		{
			auto filepath = std::format("C:\\\\DefaultPath\\\\config\\\\{}{}", fileName.c_str(), XOR(".ini"));

			misc::f_recoilslider = GetPrivateProfileFloat(XOR("Misc"), XOR("Recoil_Slider"), misc::f_recoilslider, filepath.c_str());
		}
	
	

		void Auto_Load()
		{
			if (!GetList().empty()) {
				Load_Settings(GetList().at(0));
			}
			else {
				CreateDirectoryA(XOR("C:\\DefaultPath"), NULL);
				CreateDirectoryA(XOR("C:\\DefaultPath\\config"), NULL);
			}
		}

		std::vector<std::string> GetList()
		{
			std::vector<std::string> configs;
			WIN32_FIND_DATA ffd;
			LPCSTR directory = XOR("C:\\DefaultPath\\config\\*");
			auto hFind = FindFirstFile(directory, &ffd);
			while (FindNextFile(hFind, &ffd))
			{
				if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					std::string file_name = ffd.cFileName;
					if (file_name.size() < 4) // .cfg
						continue;
					std::string end = file_name;
					end.erase(end.begin(), end.end() - 4);
					if (end != XOR(".ini"))
						continue;
					file_name.erase(file_name.end() - 4, file_name.end());
					configs.push_back(file_name);
				}
			}
			return configs;
		}

	}


}