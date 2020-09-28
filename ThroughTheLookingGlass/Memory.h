#pragma once

struct Memory;
//typedef void* Memory;

template <typename T> T* mem_alloc(Memory* memory);
void* memory_alloc(Memory* memory, int numBytes);
void memory_clear(Memory* memory);
void memory_destroy_annihalate_gone_not_even_the_pointer_will_remain(Memory* memory);
void memory_pop_stack(Memory*, void* point_to_clear);
Memory* memory_create(int numBytes);

struct StackMemory;

void* memory_stack_alloc(StackMemory* memory, int num_bytes);
