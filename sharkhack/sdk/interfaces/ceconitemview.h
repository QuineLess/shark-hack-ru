#pragma once

class CEconItemView {
public:
	void ClearInventoryImageRgba() {
		static auto fn = reinterpret_cast<void(__thiscall*)(void*)>(memory.clear_inventory_image_rgba);
		return fn(this);
	}

	CEconItemDefinition* GetStaticData() {
		static auto fn = reinterpret_cast<CEconItemDefinition * (__thiscall*)(void*)>(memory.get_static_data);
		return fn(this);
	}

	CEconItem* GetSOCData() {
		static auto fn = reinterpret_cast<CEconItem * (__thiscall*)(CEconItemView*)>(memory.get_soc_data);
		return fn(this);
	}

	get_static_offset(uint32_t, get_inventory, 0x20C);

	struct unk_struct {
		int id;
		CEconItemView* item;
		int unk;
		int unk2;
		int unk3;
	};

	const char* GetCrateSeries() {
		auto v15 = reinterpret_cast<uintptr_t*>(memory.get_item_system());
		auto v16 = *reinterpret_cast<uintptr_t*>(v15[72] + 4 * 68);

		static auto address = memory.get_typed_attribute_value;
		static auto get_typed_attribute_value_fn = *reinterpret_cast<uintptr_t*>(address + 1) + address + 5;
		unk_struct unk{ 0, this, 4, 0, 0 };

		__asm
		{
			lea eax, [unk]
			push eax
			mov eax, this
			add eax, 0xC
			mov ecx, eax
			mov edx, v16
			call get_typed_attribute_value_fn
			add esp, 4
		};

		static auto unk_fn = reinterpret_cast<int(__thiscall*)(uintptr_t, int*)>(memory.get_crate_series_unk);

		auto id = unk_fn(uintptr_t(v15) + 0x17C, &unk.id);

		auto v11 = *reinterpret_cast<uintptr_t*>(uintptr_t(v15) + 0x17C + 4) + 24 * id;

		return *reinterpret_cast<const char**>(v11 + 0x14);
	}

	bool ToolCanApplyTo(CEconItemView* item) {
		static auto fnToolCanApplyTo = memory.tool_can_apply_to;
		bool ret_val;

		__asm
		{
			mov eax, this
			add eax, 0xC
			mov ecx, eax
			mov eax, item
			add eax, 0xC
			mov edx, eax
			push 0x4
			call fnToolCanApplyTo
			mov ret_val, al
			add esp, 4
		};

		return ret_val;
	}
};