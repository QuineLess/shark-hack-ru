#include "key_values.h"

KeyValues* KeyValues::FindKey(const char* keyName, bool bCreate)
{
	static auto key_values_find_key = reinterpret_cast<KeyValues * (__thiscall*)(void*, const char*, bool)>(memory.kv_find_key);
	return key_values_find_key(this, keyName, bCreate);
}

void KeyValues::SetString(const char* keyName, const char* value)
{
	auto key = FindKey(keyName, true);
	if (key)
	{
		static auto key_values_set_string = reinterpret_cast<void(__thiscall*)(void*, const char*)>(memory.kv_set_string);
		key_values_set_string(key, value);
	}
}

void KeyValues::InitKeyValues(const char* name)
{
	static auto key_values = reinterpret_cast<void(__thiscall*)(void*, const char*)>(memory.kv_init_key_values);
	key_values(this, name);
}

void KeyValues::SetUint64(const char* keyName, int value, int value2)
{
	static auto key_values_set_uint64 = reinterpret_cast<void(__thiscall*)(void*, const char*, int, int)>(memory.kv_set_uint64);
	key_values_set_uint64(this, keyName, value, value2);
}

const char* KeyValues::GetString(const char* keyName, const char* defaultValue)
{
	static auto key_values_get_string = reinterpret_cast<const char* (__thiscall*)(void*, const char*, const char*)>(memory.kv_get_string);
	return key_values_get_string(this, keyName, defaultValue);
}

int KeyValues::GetInt(const char* keyName, int defaultValue)
{
	static auto key_values_get_int = reinterpret_cast<int(__thiscall*)(void*, const char*, int)>(memory.kv_get_int);
	return key_values_get_int(this, keyName, defaultValue);
}

void KeyValues::SetInt(const char* keyName, int Value)
{
	auto key_int = FindKey(keyName, true);
	if (key_int)
	{
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(key_int) + 0xC) = Value;
		*reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(key_int) + 0x10) = 2;
	}
}

KeyValues* KeyValues::FromString(char const* szName, char const* szStringVal)
{
	KeyValues* kv;

	__asm
	{
		push 0
		mov edx, szStringVal
		mov ecx, szName
		call memory.kv_from_string
		add esp, 4
		mov kv, eax
	}

	return kv;
}
