#pragma once

struct IBaseFileSystem {
	get_virtual_fn(0, read(void* output, int size, void* file), int(__thiscall*)(void*, void*, int, void*), output, size, file);
	get_virtual_fn(2, open(const char* file_name, const char* options, const char* path_id = 0), void* (__thiscall*)(void*, const char*, const char*, const char*), file_name, options, path_id);
	get_virtual_fn(3, close(void* file), void(__thiscall*)(void*, void*), file);
	get_virtual_fn(7, size(void* file), unsigned int(__thiscall*)(void*, void*), file);
};


struct IFileSystem {
};