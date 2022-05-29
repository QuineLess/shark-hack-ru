#pragma once
#include "../interfaces.h"

typedef PVOID(__cdecl* oKeyValuesSystem)();

class KeyValues
{
public:
	PVOID operator new(size_t iAllocSize)
	{
		static oKeyValuesSystem KeyValuesSystemFn = (oKeyValuesSystem)GetProcAddress(GetModuleHandle("vstdlib.dll"), "KeyValuesSystem");
		auto KeyValuesSystem = KeyValuesSystemFn();

		typedef PVOID(__thiscall* oAllocKeyValuesMemory)(PVOID, int);
		return CallVFunction<oAllocKeyValuesMemory>(KeyValuesSystem, 1)(KeyValuesSystem, iAllocSize);
	}

	void operator delete(PVOID pMem)
	{
		static oKeyValuesSystem KeyValuesSystemFn = (oKeyValuesSystem)GetProcAddress(GetModuleHandle("vstdlib.dll"), "KeyValuesSystem");
		auto KeyValuesSystem = KeyValuesSystemFn();

		typedef void(__thiscall* oFreeKeyValuesMemory)(PVOID, PVOID);
		CallVFunction<oFreeKeyValuesMemory>(KeyValuesSystem, 2)(KeyValuesSystem, pMem);
	}

	const char* GetName()
	{
		static oKeyValuesSystem KeyValuesSystemFn = (oKeyValuesSystem)GetProcAddress(GetModuleHandle("vstdlib.dll"), "KeyValuesSystem");
		auto KeyValuesSystem = KeyValuesSystemFn();

		auto a2 = (DWORD)this;

		typedef const char* (__thiscall* oGetName)(PVOID, int);
		return CallVFunction<oGetName>(KeyValuesSystem, 4)(KeyValuesSystem, *(BYTE*)(a2 + 3) | (*(WORD*)(a2 + 18) << 8));
	}

	KeyValues* FindKey(const char* keyName, bool bCreate = false);
	void SetString(const char* keyName, const char* value);
	void InitKeyValues(const char* name);
	void SetUint64(const char* keyName, int value, int value2);
	const char* GetString(const char* keyName, const char* defaultValue);
	int GetInt(const char* keyName, int defaultValue);
	void SetInt(const char* keyName, int Value);
	static KeyValues* FromString(char const* szName, char const* szStringVal);
};