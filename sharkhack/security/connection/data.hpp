#pragma once
#include <windows.h>

class c_data
{
public:
	BOOL set_registry(char* keyPath, char* keyName, const char* keyData);
	BOOL get_registry(char* keyPath, char* keyName, char* keyData);
};

