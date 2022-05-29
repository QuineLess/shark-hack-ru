#pragma once

#include <array>

struct EconAttirbute {
	char data[16];
	int size;
	int type;
	int id;
};

class CEconItem {
public:
	uint64_t* GetItemID()
	{
		return reinterpret_cast<uint64_t*>(this + 0x8);
	}

	uint64_t* GetOriginalID()
	{
		return reinterpret_cast<uint64_t*>(this + 0x10);
	}

	uint32_t* GetAccountID()
	{
		return reinterpret_cast<uint32_t*>(this + 0x1C);
	}

	uint32_t* GetInventory()
	{
		return reinterpret_cast<uint32_t*>(this + 0x20);
	}

	unsigned char* GetFlags()
	{
		return reinterpret_cast<unsigned char*>(this + 0x30);
	}

	uint16_t* GetDefinitionIndex()
	{
		return reinterpret_cast<uint16_t*>(this + 0x24);
	}

	unsigned short* GetEconItemData()
	{
		return reinterpret_cast<unsigned short*>(this + 0x26);
	}

	void UpdateEquippedState(unsigned int state)
	{
		static auto fnUpdateEquippedState = reinterpret_cast<int(__thiscall*)(CEconItem*, unsigned int)>(utils::find_signature("client.dll", "55 8B EC 8B 45 08 8B D0 C1 EA 10"));

		fnUpdateEquippedState(this, state);
	}

	template<typename TYPE>
	void SetAttributeValue(int index, TYPE val) {
		auto v15 = (DWORD*)memory.get_item_system();
		auto v16 = *(DWORD*)(v15[72] + 0x4 * index);
		static auto fnSetDynamicAttributeValue = reinterpret_cast<int(__thiscall*)(CEconItem*, DWORD, void*)>(memory.set_dynamic_attribute_value);
		fnSetDynamicAttributeValue(this, v16, &val);
	}

	void AddSticker(const int index, const int kit, const float wear, const float scale, const float rotation) {
		SetAttributeValue<int>(113 + 4 * index, kit);
		SetAttributeValue<float>(114 + 4 * index, wear);
		SetAttributeValue<float>(115 + 4 * index, scale);
		SetAttributeValue<float>(116 + 4 * index, rotation);
	}

	void SetPaintKit(const int kit) {
		SetAttributeValue<float>(6, float(kit));
	}

	void SetPaintSeed(const int seed) {
		SetAttributeValue<float>(7, float(seed));
	}

	void SetPaintWear(const float wear) {
		SetAttributeValue<float>(8, wear);
	}

	void SetQuality(const int quality) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data ^ (data ^ 32 * quality) & 0x1E0;
	}

	void SetRarity(const int rarity) {
		auto data = *GetEconItemData();
		*GetEconItemData() = (data ^ (rarity << 11)) & 0x7800 ^ data;
	}

	void SetOrigin(const int origin) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data ^ (static_cast<unsigned __int8>(data) ^ static_cast<unsigned __int8>(origin)) & 0x1F;
	}

	void SetLevel(const int level) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data ^ (data ^ (level << 9)) & 0x600;
	}

	void SetStatTrak(int val) {
		SetAttributeValue<int>(80, val);
		SetAttributeValue<int>(81, 0);
		SetQuality(9);
	}

	std::vector<EconAttirbute> GetAttributes()
	{
		std::vector<EconAttirbute> attributes;

		auto v16 = *(WORD**)((uintptr_t)this + 0x18);
		if (v16)
		{
			auto v17 = v16 + 1;
			auto v48 = (unsigned int)&v16[4 * ((unsigned int)*v16 >> 10) + 1];
			if ((unsigned int)(v16 + 1) < v48)
			{
				do
				{
					auto v18 = (DWORD*)memory.get_item_system();
					auto v19 = *v17;
					if (v19 < v18[75])
					{
						auto v20 = *(DWORD*)(v18[72] + 4 * v19);
						if (v20)
						{
							auto attrib = EconAttirbute();
							attrib.id = v19;

							auto attrib_pointer = &attrib;

							*(DWORD*)((uintptr_t)attrib_pointer + 20) = 15;
							*(DWORD*)((uintptr_t)attrib_pointer + 16) = 0;
							*(BYTE*)(uintptr_t)attrib_pointer = 0;

							auto v45 = *(DWORD*)(v20 + 0xC);

							(*(void(__thiscall **)(DWORD, int, DWORD))(*(DWORD*)v45 + 12))(v45, (int)(v17 + 2), (uintptr_t)attrib_pointer);

							attributes.push_back(attrib);
						}
					}

					v17 += 4;
				} while ((unsigned int)v17 < v48);
			}
		}

		return attributes;
	}

	std::array<uint32_t, 2> GetEquippedState()
	{
		auto eq_st = *reinterpret_cast<uintptr_t*>((uintptr_t)this + 0x18);
		std::array<uint32_t, 2> EquippedState{ 0,0 };

		if (eq_st)
		{
			if ((**(BYTE**)((uintptr_t)this + 0x18) & 0x3F) != 63)
			{
				auto v31 = **(WORD**)((uintptr_t)this + 0x18);
				unsigned short v32 = (v31 >> 6) & 7; //2
				unsigned short v33 = v31 & 0x3F; //3

				unsigned int final = ((v33 << 16) | (v32));

				EquippedState[0] = final;

				if (**(WORD **)((uintptr_t)this + 24) & 0x200 && (**(WORD **)((uintptr_t)this + 24) & 0x1C0) == 128)
				{
					unsigned short v1 = 3;
					unsigned short v2 = v31 & 0x3F; //3

					unsigned int final2 = ((v2 << 16) | (v1));

					EquippedState[1] = final2;
				}
			}
		}

		return EquippedState;
	}

	void SetInUse(const bool in_use) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data & 0x7FFF | (in_use << 15);
	}

	void SetCustomName(const char* name) {
		static auto address = memory.custom_econ_name;
		reinterpret_cast<CEconItem*(__thiscall*)(void*, const char*)>(*reinterpret_cast<uintptr_t*>(address + 1) + address + 5)(this, name);
	}
};