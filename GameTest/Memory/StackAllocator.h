#pragma once

#include "Allocator.h"

// Stack Allocator allocates memory like stack but on the heap
// Planning to use this allocator to store all the entities in the game
// that has known lifespan (or not being destroyed)

struct StackHeader
{
    char address;
};

class StackAllocator : public Allocator
{
private:
    char* m_stack = nullptr;        // pointer to the heap allocated memory of the entire stack
    size_t m_allocatedSize = 0;     // keeps track of how much memory has been allocated in the heap

public:
    // constructor and descontructor
    StackAllocator(const size_t total_size);
    ~StackAllocator();

    // Init func to initialize before using the stack allocator
    void Init();

protected:
    void* Allocate(const size_t size) override;

    // StackAllocator cannot choose a specific ptr to deallocate
    // always deallocates the top of the stack (most recently allocated memory)
    void Deallocate(void* ptr=nullptr) override;
};