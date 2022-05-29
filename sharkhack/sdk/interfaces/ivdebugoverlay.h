#pragma once

struct IVDebugOverlay {
	get_virtual_fn(24, add_capsule_overlay(Vector& mins, Vector& maxs, float pillradius, Color color, float duration), void(__thiscall*)(void*, Vector&, Vector&, float&, int, int, int, int, float), mins, maxs, pillradius, color.r(), color.g(), color.b(), color.a(), duration);
};