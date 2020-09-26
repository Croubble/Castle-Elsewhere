#pragma once

struct Memory;
//typedef void* Memory;

template <typename T> T* mem_alloc(Memory* memory);
void* memory_alloc(Memory* memory, int numBytes);
void memory_clear(Memory* memory);
void memory_destroy_annihalate_gone_not_even_the_pointer_will_remain(Memory* memory);
Memory* memory_create(int numBytes);

template<typename T>
inline T* mem_alloc(Memory* memory)
{
	return memory_alloc(memory, sizeof(T));
}
template<typename T>
inline T* mem_alloc_arr(Memory* memory, int num_elements)
{
	return memory_alloc(memory, sizeof(T) * num_elements);
}
