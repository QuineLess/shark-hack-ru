#pragma once

#include "../interfaces.h"
#include <map>

struct weaponids_t {
	void on_init();
	std::unordered_map<uint32_t, std::vector<CEconItemDefinition*>> m_types = {};
	std::unordered_map<uint32_t, CEconItemDefinition*> m_list = {};
};

extern weaponids_t weaponids;