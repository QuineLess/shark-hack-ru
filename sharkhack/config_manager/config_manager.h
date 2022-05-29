#pragma once

namespace config_manager {
	void read_directory(const std::string & name, std::vector<std::string>& v);
	void save(std::string config_file);
	void load(std::string config_file);
	int upload_share_settings(std::string file);
	int download_share_settings(std::string file);
}

#define save_value(group_name, name, value) WritePrivateProfileStringA(group_name, name, value, file.c_str());

#define load_value(group_name, name, value, type, ...) GetPrivateProfileStringA(group_name, name, "", value_l, 32, file.c_str()); \
			value = type(value_l, __VA_ARGS__);

#define load_string(group_name, name, value) GetPrivateProfileStringA(group_name, name, "", value_l, 32, file.c_str()); \
			value = value_l;
