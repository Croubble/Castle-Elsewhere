#pragma once

struct Memory;
//typedef void* Memory;

void* memory_alloc(Memory* memory, int numBytes);
void memory_clear(Memory* memory);
void memory_destroy_annihalate_gone_not_even_the_pointer_will_remain(Memory* memory);
Memory* memory_create(int numBytes);
