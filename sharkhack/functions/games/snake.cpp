#include "snake.h"

void snake_game::restart() {
	snake.pos[0] = { 4, 8 };
	snake.pos[1] = { 3, 8 };
	snake.pos[2] = { 2, 8 };
	snake.pos[3] = { 1, 8 };

	snake.length = 4;
	snake.side = 0;

	return;
}

void snake_game::move() {
	if ((GetTickCount64() - time_scale) >= 75 && snake.side) {
		Vector2D prev{ snake.pos[0].x, snake.pos[0].y };
		Vector2D prev2{ snake.pos[0].x, snake.pos[0].y };

		switch (snake.side) {
		case 1:
			snake.pos[0].y -= 1;
			snake.tail = 3;
			break;
		case 2:
			snake.pos[0].x -= 1;
			snake.tail = 4;
			break;
		case 3:
			snake.pos[0].y += 1;
			snake.tail = 1;
			break;
		case 4:
			snake.pos[0].x += 1;
			snake.tail = 2;
			break;
		}

		for (int i = 1; i < snake.length; i++)
		{
			prev = prev2;
			prev2 = snake.pos[i];
			if (snake.pos[i].x < prev.x)
				snake.pos[i].x += 1;
			else if (snake.pos[i].x > prev.x)
				snake.pos[i].x -= 1;
			else if (snake.pos[i].y < prev.y)
				snake.pos[i].y += 1;
			else if (snake.pos[i].y > prev.y)
				snake.pos[i].y -= 1;
		}

		time_scale = GetTickCount64();
	}
	return;
}

void snake_game::game_scripts() {
	if ((GetAsyncKeyState(0x26) || GetAsyncKeyState(0x57)) && snake.side != 1 && snake.tail != 1)
		snake.side = 1;

	if ((GetAsyncKeyState(0x25) || GetAsyncKeyState(0x41)) && snake.side != 2 && snake.tail != 2)
		snake.side = 2;

	if ((GetAsyncKeyState(0x28) || GetAsyncKeyState(0x53)) && snake.side != 3 && snake.tail != 3)
		snake.side = 3;

	if ((GetAsyncKeyState(0x27) || GetAsyncKeyState(0x44)) && snake.side != 4 && snake.tail != 4)
		snake.side = 4;

	Vector2D last_tail = { 0, 0 };

	if (!food.active) {
		bool insnake{ true };
		Vector2D pos = { 0, 0 };
		while (insnake) {
			insnake = false;

			pos = { (float)(rand() % 32), (float)(rand() % 16) };
			for (int i = 0; i < snake.length; i++) {
				if (pos == snake.pos[i])
					insnake = true;
			}
		}

		food.pos = pos;
		food.active = true;
	}
	else if (food.active)
		last_tail = snake.pos[snake.length - 1];

	move();

	if (food.active && food.pos == snake.pos[0]) {
		snake.length += 1;
		snake.pos[snake.length - 1] = last_tail;
		food.active = false;
		food.pos = { 0, 0 };
		food.last = GetTickCount64();
	}

	for (int i = 0; i < snake.length; i++) {
		for (int k = 0; k < snake.length; k++) {
			if (i != k && snake.pos[i] == snake.pos[k]) {
				restart();
				return;
			}
		}
	}

	if (snake.pos[0].x <= -1 || snake.pos[0].y <= -1 || snake.pos[0].x >= 32 || snake.pos[0].y >= 16)
		restart();

	return;
}