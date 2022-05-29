#pragma once

#include "../../utils/entity.h"

inline ITexture* preview_texture;
inline float_t preview_matrix = 180.f;
inline CMergedMdl* preview_model;
inline bool draw_model = false;

namespace preview {
	void on_init();
}
