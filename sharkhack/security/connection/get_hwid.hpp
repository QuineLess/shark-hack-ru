#pragma once
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <intrin.h>
#include "xor.hpp"


class get_hwid {
public:
	static std::string GetHwUID();
	static std::string GetCpuid();
	static std::string GetCompUserName(bool User);
	static DWORD GetVolumeID();
	static std::string StringToHex(const std::string input);
	static std::string GetSerialKey();
	static std::string GetHashText(const void* data, const size_t data_size);
	static std::string GetHashSerialKey();
	static std::string GetHashCpuid();
	static std::string GetSerial();
};