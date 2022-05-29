#pragma once
#include "../../utils/entity.h"
#include "../../utils/render.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

namespace radar {
	struct map_info {
		Vector pos = Vector(0, 0, 0);
		Vector size = Vector(0, 0, 0);
	};
	void calc_radar_point(C_BasePlayer* local_player, Vector origin, float& x, float& y);
	radar::map_info get_map();
	void draw_map(C_BasePlayer* local_player);
	void load_map(long map_overview);
	void draw_player(C_BasePlayer* local_player);
	void render(C_BasePlayer* local_player);
	inline IDirect3DTexture9* map_image;
	inline Vector map_origin;
	inline double map_scale;
	inline bool is_valid() {
		return !(map_origin.x == 0.f && map_origin.y == 0.f && map_scale == 1.f);
	}
	inline bool is_grenade(C_BaseEntity* entity) {

		const model_t* model = entity->GetModel();

		if (!model)
			return false;

		studiohdr_t* hdr = g_mdl_info->get_studio_model(model);

		if (!hdr)
			return false;

		std::string name = hdr->szName;

		if (name.find(XORSTR("dropped")) != std::string::npos || name.find(XORSTR("thrown")) != std::string::npos) {
			if (name.find(XORSTR("flashbang")) != std::string::npos ||
				name.find(XORSTR("incendiarygrenade")) != std::string::npos ||
				name.find(XORSTR("molotov")) != std::string::npos ||
				name.find(XORSTR("fraggrenade")) != std::string::npos ||
				name.find(XORSTR("smokegrenade")) != std::string::npos ||
				name.find(XORSTR("decoy")) != std::string::npos)
				return true;
		}

		return false;
	}
}