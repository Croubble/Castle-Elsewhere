#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>

const unsigned int alignBoundary = 8;

struct Memory
{
	int currentOffset;
	int size;
	void* memoryStart;
};

void* memory_alloc(Memory* memory, int numBytes)
{
	int bytesToAddToMemoryToConserveAlignment = numBytes % alignBoundary;
	int finalBytes = numBytes + bytesToAddToMemoryToConserveAlignment;

	//check for overflow memory.
	if (finalBytes + memory->currentOffset >= memory->size)
	{
		std::cout << "we have overflowed our memory. Rats!" << std::endl;
		abort();
		return nullptr;
	}
	void* result = (void*) (((char*) memory->memoryStart) + memory->currentOffset);
	memory->currentOffset += finalBytes;
	return result;
}
void memory_clear(Memory* memory)
{
	memory->currentOffset = 0;
}
Memory* memory_create(int numBytes)
{
	int size = sizeof(Memory);
	Memory* memory = (Memory*) malloc(size);
	memory->currentOffset = 0;
	memory->size = numBytes;
	memory->memoryStart = malloc(numBytes);
	return memory;
}