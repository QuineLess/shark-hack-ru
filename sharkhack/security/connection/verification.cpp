#include "get_hwid.hpp"
#include "security json fix.hpp"
#include "sec connection.h"
#include <fstream>
using json = nlohmann::json;


size_t writer(const void* indata, const size_t size, const size_t count, void* out) {
	(*(std::string*)out).append((const char*)indata, size * count);
	return size * count;
}
bool c_api::auth(std::string username, std::string password) {

	CURL* curl;
	curl = curl_easy_init();
	auto gk = new get_hwid();
	json all;
	std::string d = security_xor("a=auth&u=") + username + security_xor("&p=") + password + security_xor("&h=") + gk->GetSerial();

	bool ret = false;
	if (curl)
	{
		std::string response;
		curl_easy_setopt(curl, CURLOPT_URL, security_xor("https://shark-hack.ru/api/loader"));
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, d.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		auto res = curl_easy_perform(curl);
		
		if (CURLE_OK == res)
		{
			all = json::parse(response);
			auto link = all[security_xor("response")][security_xor("type")].get<std::string>();
			if (strstr(link.c_str(), security_xor("success")))
				ret = true;

		}
		curl_easy_cleanup(curl);


	}
	delete gk;
	curl_global_cleanup();
	return ret;

}