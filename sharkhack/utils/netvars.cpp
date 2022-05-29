#include "netvars.h"
#include <fstream>
#include <utility>

std::vector<netvar_table>  database;

void netvars::on_init() {
	database.clear();

	for (auto clientclass = g_client->GetAllClasses();
		clientclass != nullptr;
		clientclass = clientclass->m_pNext) {
		if (clientclass->m_pRecvTable) {
			database.emplace_back(load_table(clientclass->m_pRecvTable));
		}
	}
}

netvar_table netvars::load_table(RecvTable* recvTable) {
	auto table = netvar_table{};

	table.offset = 0;
	table.name = recvTable->m_pNetTableName;

	for (auto i = 0; i < recvTable->m_nProps; ++i) {
		auto prop = &recvTable->m_pProps[i];

		if (!prop || isdigit(prop->m_pVarName[0]))
			continue;
		if (strcmp(XORSTR("baseclass"), prop->m_pVarName) == 0)
			continue;

		if (prop->m_RecvType == DPT_DataTable && prop->m_pDataTable) {
			table.child_tables.emplace_back(load_table(prop->m_pDataTable));
			table.child_tables.back().offset = prop->m_Offset;
			table.child_tables.back().prop = prop;
		}
		else {
			table.child_props.emplace_back(prop);
		}
	}
	return table;
}

void netvars::dump() {
	auto outfile = std::ofstream(XORSTR("netvar_dump.txt"));

	dump(outfile);
}

void netvars::dump(std::ostream& stream) {
	for (const auto& table : database) {
		if (table.child_props.empty() && table.child_tables.empty())
			continue;
		stream << table.name << '\n';
		dump_table(stream, table, 1);
		stream << '\n';
	}

	stream << std::endl;
}

void netvars::dump_table(std::ostream& stream, const netvar_table& table, uint32_t indentation) {
	char line_buffer[1024];

	for (const auto& prop : table.child_props) {
		sprintf_s(line_buffer, XORSTR("%*c%*s: 0x%08X"), indentation * 4, ' ', -(50 - (int)indentation * 4), prop->m_pVarName, table.offset + prop->m_Offset);
		stream << line_buffer << '\n';
	}
	for (const auto& child : table.child_tables) {
		sprintf_s(line_buffer, XORSTR("%*c%*s: 0x%08X"), indentation * 4, ' ', -(50 - (int)indentation * 4), child.prop->m_pVarName, table.offset + child.offset);
		stream << line_buffer << '\n';
		dump_table(stream, child, indentation + 1);
	}
}

uint32_t netvars::get_offset(const std::string& tableName, const std::string& propName) {
	auto result = 0u;
	for (const auto& table : database) {
		if (table.name == tableName) {
			result = get_offset(table, propName);
			if (result != 0)
				return result;
		}
	}
	return 0;
}

uint32_t netvars::get_offset(const netvar_table& table, const std::string& propName) {
	for (const auto& prop : table.child_props) {
		if (strncmp(prop->m_pVarName, propName.data(), propName.size()) == 0) {
			return table.offset + prop->m_Offset;
		}
	}
	for (const auto& child : table.child_tables) {
		auto prop_offset = get_offset(child, propName);
		if (prop_offset != 0)
			return table.offset + prop_offset;
	}
	for (const auto& child : table.child_tables) {
		if (strncmp(child.prop->m_pVarName, propName.data(), propName.size()) == 0) {
			return table.offset + child.offset;
		}
	}
	return 0;
}

RecvProp* netvars::get_netvar_prop(const std::string& tableName, const std::string& propName) {
	RecvProp* result = nullptr;
	for (const auto& table : database) {
		if (table.name == tableName) {
			result = get_netvar_prop(table, propName);
		}
	}
	return result;
}

RecvProp* netvars::get_netvar_prop(const netvar_table& table, const std::string& propName) {
	for (const auto& prop : table.child_props) {
		if (strncmp(prop->m_pVarName, propName.data(), propName.size()) == 0) {
			return prop;
		}
	}
	for (const auto& child : table.child_tables) {
		auto prop = get_netvar_prop(child, propName);
		if (prop != 0)
			return prop;
	}
	for (const auto& child : table.child_tables) {
		if (strncmp(child.prop->m_pVarName, propName.data(), propName.size()) == 0) {
			return child.prop;
		}
	}
	return nullptr;
}
