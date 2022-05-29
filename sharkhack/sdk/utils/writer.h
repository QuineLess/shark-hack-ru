#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "field.h"

class writer
{
	std::vector<std::vector<field>> fields;
public:
	writer() {}
	writer(size_t maxfields) {
		size_t vector_size = maxfields + 1;
		fields.resize(vector_size);
		fields.reserve(vector_size);
	}

	writer(void* data, size_t size, size_t maxfields)
		: writer(maxfields)
	{
		size_t vector_size = maxfields + 1,
			pos = 0,
			bytesRead;

		if (data == nullptr)
			return;
		// parse packet
		while (pos < size) {
			auto field = field::read((void*)((ptrdiff_t)data + pos), bytesRead);
			if (!bytesRead) break;

			auto index = field.tag.field;
			if (index >= vector_size) throw("fields range error: field[%i]", index);
			fields[index].push_back(field);
			pos += bytesRead;
		}
	}

	writer(std::string dataStr, size_t maxfields)
		: writer((void*)dataStr.data(), dataStr.size(), maxfields) {}

	std::string serialize() {
		std::string result;
		for (auto& f0 : fields) {
			for (auto& f1 : f0) {
				result += f1.full;
			}
		}
		return result;
	}

	void print() {
		auto data = serialize();
		void* mem = (void*)data.data();
		size_t size = data.size();
		int j = 0;
		for (size_t i = 0; i <= size; ++i) {
			printf("%.2X ", *(unsigned char*)((size_t)mem + i));
			j++;
			if (j == 16)
			{
				j = 0;
				printf("\n");
			}
		}
		printf("\n");
	}

	void add(field field) {
		fields[field.tag.field].push_back(field);
	}

	void replace(field field) {
		fields[field.tag.field].clear();
		fields[field.tag.field].push_back(field);
	}

	void replace(field field, uint32_t index) {
		fields[field.tag.field][index] = field;
	}

	void clear(unsigned fieldId) {
		return fields[fieldId].clear();
	}

	bool has(unsigned fieldId) {
		return fields[fieldId].size() > 0;
	}

	field get(unsigned fieldId)
	{
		return fields[fieldId][0];
	}

	std::vector<field> getAll(unsigned fieldId)
	{
		return fields[fieldId];
	}
};
