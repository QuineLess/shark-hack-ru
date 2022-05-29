#pragma once

#include "../../utils/entity.h"
#include "../../utils/render.h"
#include "../../utils/math.h"

struct trace_info {
	trace_info(Vector src, Vector dst, float time, Color color) {
		this->src = src;
		this->dst = dst;
		this->time = time;
		this->color = color;
	}

	Vector src, dst;
	float time;
	Color color;
};

inline std::vector<trace_info> logs;

namespace bullet_tracers {
	void on_events(IGameEvent * event);
}