#pragma once
inline void**& GetVTable(void* instance)
{
	return *reinterpret_cast<void***>((size_t)instance);
}

inline void** GetVTable(const void* instance)
{
	return *reinterpret_cast<void***>((size_t)instance);
}

template<typename T>
inline T CallVFunction(const void* instance, size_t index)
{
	return reinterpret_cast<T> (GetVTable(instance)[index]);
}