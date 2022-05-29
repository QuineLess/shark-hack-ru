#include "memory.h"
#include "../sdk/interfaces.h"

CCStrike15ItemSystem* memory_t::get_item_system() {
	static auto fn = reinterpret_cast<CCStrike15ItemSystem * (__stdcall*)()>(item_system);
	return fn();
}

CCStrike15ItemSchema* memory_t::get_item_schema() {
	return get_item_system()->get_item_schema_interface();
}