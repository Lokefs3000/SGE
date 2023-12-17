#pragma once

#include <array>

class MemoryUtilities {
private:

public:
	template<typename T>
	static char* GetBytesOf(T* value);

	template<typename T, typename Ptr>
	static T ConvertTo(Ptr* pointer, size_t& idx);
};

template<typename T>
inline char* MemoryUtilities::GetBytesOf(T* value)
{
	char* ptr = (char*)value;
	return ptr;
}

template<typename T, typename Ptr>
inline T MemoryUtilities::ConvertTo(Ptr* pointer, size_t& idx)
{
	T data;
	memcpy(&data, pointer + idx, sizeof(T));
	idx += sizeof(T);
	return data;
}
