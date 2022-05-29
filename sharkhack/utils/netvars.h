#pragma once
#include "../sdk/interfaces.h"

#include <memory>
#include <iostream>
#include <unordered_map>

using namespace std;

struct netvar_table {
	std::string               name;
	RecvProp*                 prop;
	uint32_t                  offset;
	std::vector<RecvProp*>    child_props;
	std::vector<netvar_table> child_tables;
};

namespace netvars {
	extern void on_init();

	extern void dump();
	extern void dump(std::ostream& stream);

	extern uint32_t      get_offset(const std::string& tableName, const std::string& propName);
	extern RecvProp*     get_netvar_prop(const std::string& tableName, const std::string& propName);

	extern netvar_table  load_table(RecvTable* clientClass);

	extern void          dump_table(std::ostream& stream, const netvar_table& table, uint32_t indentation);
	extern uint32_t      get_offset(const netvar_table& table, const std::string& propName);
	extern RecvProp*     get_netvar_prop(const netvar_table& table, const std::string& propName);
};
