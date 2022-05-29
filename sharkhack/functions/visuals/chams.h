#pragma once
#include "../../utils/entity.h"
#include "../../utils/hooks.h"

inline IMaterial* material_texture;
inline IMaterial* material_flat;
inline IMaterial* material_animated;
inline IMaterial* material_glow;
inline IMaterial* material_glow_overlay;
inline IMaterial* material_gloss;
inline IMaterial* material_crystal;
inline IMaterial* material_glass;
inline IMaterial* material_pearlescent;
inline IMaterial* material_ghost;
inline IMaterial* material;

enum chams_type {
	chams_type_flat,
	chams_type_texture,
	chams_type_animated,
	chams_type_pearlescent,
	chams_type_ghost
};

enum chams_more_type {
	chams_more_type_none,
	chams_more_type_glow,
	chams_more_type_glow_overlay,
	chams_more_type_glow_pulse,
	chams_more_type_gloss,
	chams_more_type_crystal,
	chams_more_type_glass
};

namespace chams {
	void on_init();
	void get_material(IMaterial* material, bool ignorez, bool wireframe, float r, float g, float b, float alpha);
	void player(C_BasePlayer* local_player, uintptr_t ecx, uintptr_t edx, void* results, DrawModelInfo_t* info, matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, Vector& model_origin, int flags);
	void arms(C_BasePlayer* local_player, uintptr_t ecx, uintptr_t edx, void* results, DrawModelInfo_t* info, matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, Vector& model_origin, int flags);
	void weapon(C_BasePlayer* local_player, uintptr_t ecx, uintptr_t edx, void* results, DrawModelInfo_t* info, matrix3x4_t* bone_to_world, float* flex_weights, float* flex_delayed_weights, Vector& model_origin, int flags);
};
