#pragma once

#include "studio.h"

struct IVModelInfo {
	get_virtual_fn(2, get_model_index(const char* name), int(__thiscall*)(void*, const char*), name);
	get_virtual_fn(19, get_model_materials(const model_t* model, int count, IMaterial** p_material), void(__thiscall*)(void*, const model_t*, int, IMaterial**), model, count, p_material);
	get_virtual_fn(32, get_studio_model(const model_t* model), studiohdr_t*(__thiscall*)(void*, const model_t*), model);
};
