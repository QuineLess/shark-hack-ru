#pragma once
#include "../../sdk/interfaces.h"

struct snake_t {
	Vector2D pos[MAX_PATH];
	int length = 4;
	int side = 0;
	int tail = 0;
};

struct food_t {
	bool active = false;
	Vector2D pos{ -1, -1 };
	ULONGLONG last = GetTickCount64();
};

struct pool_t {
	float width = 34.f;
	float height = 15.f;
};

namespace snake_game {
	void restart();
	void move();
	void game_scripts();
	inline snake_t snake;
	inline food_t food;
	inline pool_t pool;
	inline ULONGLONG time_scale = GetTickCount64();
}