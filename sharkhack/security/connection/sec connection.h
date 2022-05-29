#pragma once
#include <Windows.h>
#include <string.h>
#define CURL_STATICLIB
#include "../curl/curl.h"


#pragma comment( lib, "libcurl.lib" )

class c_api {
public:
	bool auth(std::string username, std::string password);
};