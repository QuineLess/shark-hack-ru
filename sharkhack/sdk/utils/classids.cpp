#include "classids.h"

uint32_t classids::get(const char* name) {
	int cur_id = 0;
	auto server_class_list = g_server->GetAllServerClasses();

	while (server_class_list) {
		if (!strcmp(server_class_list->m_pNetworkName, name))
			return cur_id;
		server_class_list = server_class_list->m_pNext, cur_id++;
	}

	return -1;
}