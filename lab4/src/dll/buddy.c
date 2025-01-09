#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>

#define MIN_BLOCK_SIZE 32
#define MAX_BLOCK_INDEX 24

typedef struct Block {
    size_t size;
    struct Block* next;
} Block;

typedef struct TwonAllocator {
    void* memory;
    size_t total_size;
    Block* free_lists[MAX_BLOCK_INDEX];
} TwonAllocator;

size_t round_to_power_of_two(size_t size) {
    size_t power = MIN_BLOCK_SIZE;
    while (power < size) {
        power <<= 1;
    }
    return power;
}

int get_power_of_two(size_t size) {
    return (int)(log2(size));
}

void* allocator_create(void* const memory, const size_t size) {
    if (!memory || size < MIN_BLOCK_SIZE) {
        return NULL;
    }

    TwonAllocator* allocator = (TwonAllocator*)mmap(
        NULL, sizeof(TwonAllocator), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (allocator == MAP_FAILED) {
        return NULL;
    }

    size_t total_size = round_to_power_of_two(size);
    size_t max_size = 1 << (MAX_BLOCK_INDEX - 1);

    if (total_size > max_size) {
        munmap(allocator, sizeof(TwonAllocator));
        return NULL;
    }

    allocator->memory = memory;
    allocator->total_size = total_size;

    for (size_t i = 0; i < MAX_BLOCK_INDEX; i++) {
        allocator->free_lists[i] = NULL;
    }

    Block* initial_block = (Block*)allocator->memory;
    initial_block->size = total_size;
    initial_block->next = NULL;

    int index = get_power_of_two(total_size) - get_power_of_two(MIN_BLOCK_SIZE);
    allocator->free_lists[index] = initial_block;

    return allocator;
}

void allocator_destroy(void* const twon_allocator) {
    if (!twon_allocator) return;

    TwonAllocator* allocator = (TwonAllocator*)twon_allocator;
    munmap(allocator, sizeof(TwonAllocator));
}

void* allocator_alloc(void* const twon_allocator, const size_t size) {
    if (!twon_allocator || size == 0) return NULL;

    TwonAllocator* allocator = (TwonAllocator*)twon_allocator;

    size_t block_size = round_to_power_of_two(size);
    size_t max_size = 1 << (MAX_BLOCK_INDEX - 1);

    if (block_size > max_size) {
        return NULL;
    }

    int index = get_power_of_two(block_size) - get_power_of_two(MIN_BLOCK_SIZE);
    if (index < 0 || index >= MAX_BLOCK_INDEX) {
        return NULL;
    }

    while (index < MAX_BLOCK_INDEX && !allocator->free_lists[index]) {
        index++;
    }

    if (index >= MAX_BLOCK_INDEX) {
        return NULL;
    }

    Block* block = allocator->free_lists[index];
    allocator->free_lists[index] = block->next;

    while (block->size > block_size) {
        size_t new_size = block->size >> 1;
        Block* buddy = (Block*)((char*)block + new_size);
        buddy->size = new_size;
        buddy->next =
            allocator->free_lists[get_power_of_two(new_size) - get_power_of_two(MIN_BLOCK_SIZE)];
        allocator->free_lists[get_power_of_two(new_size) - get_power_of_two(MIN_BLOCK_SIZE)] =
            buddy;
        block->size = new_size;
    }

    return (void*)((char*)block + sizeof(Block));
}

void allocator_free(void* const twon_allocator, void* const memory) {
    if (!twon_allocator || !memory) return;

    TwonAllocator* allocator = (TwonAllocator*)twon_allocator;

    Block* block = (Block*)((char*)memory - sizeof(Block));

    size_t block_size = block->size;
    int index = get_power_of_two(block_size) - get_power_of_two(MIN_BLOCK_SIZE);
    if (index < 0 || index >= MAX_BLOCK_INDEX) return;

    block->next = allocator->free_lists[index];
    allocator->free_lists[index] = block;
}
