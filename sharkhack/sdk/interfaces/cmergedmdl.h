#pragma once

struct CMdl {
	std::byte pad_0[0x58];
	int32_t sequence;
	std::byte pad_1[0x8];
	float_t time;
	float_t current_anim_end_time;
	std::byte pad_2[0x611];
	bool use_sequence_playback_fps;
	std::byte pad_3[0x67];
	float_t time_basis_adjustment;
};

class CMergedMdl {
public:
	CMdl root_mdl;

	get_virtual_fn(0, draw(const matrix3x4_t& mat_root_to_world), void(__thiscall*)(void*, const matrix3x4_t&), mat_root_to_world);
	get_virtual_fn(4, set_mdl(const char* model_path, void* custom_material_owner = nullptr, void* proxy_data = nullptr), void(__thiscall*)(void*, const char*, void*, void*), model_path, custom_material_owner, proxy_data);

	void setup_bones_for_attachment_queries() {
		reinterpret_cast<void(__thiscall*)(void*)>(memory.setup_bones_for_attachment_queries)(this);
	}

	void set_merge_mdl(const char* model_path, void* custom_material_owner = nullptr, void* proxy_data = nullptr, const bool request_bone_merge_takeover = false) {
		reinterpret_cast<void(__thiscall*)(void*, const char*, void*, void*, bool)>(memory.set_merge_mdl)(this, model_path, custom_material_owner, proxy_data, request_bone_merge_takeover);
	}

	void set_sequence(const int32_t sequence, const bool use_sequence_playback_fps) {
		root_mdl.sequence = sequence;
		root_mdl.use_sequence_playback_fps = use_sequence_playback_fps;
		root_mdl.time_basis_adjustment = root_mdl.time;
	}
};