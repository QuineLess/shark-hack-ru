#pragma once
struct DrawModelInfo_t
{
	studiohdr_t* studiohdr;
	void* hardware_data;
	void* decals;
	int32_t skin;
	int32_t body;
	int32_t hitbox_set;
	IClientRenderable* client_entity;
	int32_t lod;
	void* color_meshes;
	bool static_lighting;
};

enum class OverrideType_t {
	OVERRIDE_NORMAL,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE = 4
};

class IStudioRender {
	std::byte pad_0[0x250];
	IMaterial* material_override;
	std::byte pad_1[0xC];
	OverrideType_t override_type;
public:
	get_virtual_fn(20, set_ambient_light_colors(const Vector4D* ambient_only_colors), void(__thiscall*)(void*, const Vector4D*), ambient_only_colors);
	get_virtual_fn(22, set_local_lights(int32_t num_lights, const void* lights), void(__thiscall*)(void*, int32_t, const void*), num_lights, lights);
	get_virtual_fn(33, ForcedMaterialOverride(IMaterial* material, OverrideType_t type = OverrideType_t::OVERRIDE_NORMAL, int32_t index = -1), void(__thiscall*)(void*, IMaterial*, OverrideType_t, int32_t), material, type, index);

	bool IsForcedMaterialOverride() {
		if (!material_override)
			return override_type == OverrideType_t::OVERRIDE_DEPTH_WRITE || override_type == OverrideType_t::OVERRIDE_SSAO_DEPTH_WRITE;
		return strstr(material_override->GetName(), "dev/glow");
	}
};