#pragma once

struct GlowObjectDefinition_t {

	void set(Color color) {
		red = color.r() / 255.0f;
		green = color.g() / 255.0f;
		blue = color.b() / 255.0f;
		alpha = color.a() / 255.0f;
		render_when_occluded = true;
		render_when_unoccluded = false;
		bloom_amount = 1.f;
	}

	C_BasePlayer* get_entity() {
		return entity;
	}

	bool is_empty() const {
		return next_free_slot != GlowObjectDefinition_t::entry_in_use;
	}

	int next_free_slot;

	C_BasePlayer* entity;
	union {
		Vector glow_color;
		struct {
			float red;           //0x0004
			float green;         //0x0008
			float blue;          //0x000C
		};
	};
	float alpha;

	char unknown[4];
	float unkwn;
	float bloom_amount;
	float localplayeriszeropoint3;

	bool render_when_occluded;
	bool render_when_unoccluded;
	bool bloom_render;
	char unknown1[1];

	int full_bloom_stencil_test_value;
	int glow_style;
	int split_screen_slot;

	static const int end_of_free_list = -1;
	static const int entry_in_use = -2;
};

struct CGlowObjectManager {
	CUtlVector < GlowObjectDefinition_t> glow_object_definitions;
	int first_free_slot;
};