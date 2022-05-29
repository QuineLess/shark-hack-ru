#pragma once

class C_BasePlayer;

class CMoveHelper {
public:
    void set_host(C_BasePlayer* local_player) {
        return CallVFunction<void(__thiscall*)(void*, C_BasePlayer*)>(this, 1)(this, local_player);
    }
};
