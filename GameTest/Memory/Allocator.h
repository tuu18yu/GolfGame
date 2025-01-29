#pragma once

#include <stdlib.h> /* size_t */
#include <memory> /* new */

/* I decided to build my own custom memory allocator to avoid memory fragmentation
and lowering cache misses to improve efficiency. The game may be simple so it might not
be necessary for now, but thought it would be necessary when the game is added more complexity */

// The previous design doesn't encapsulate the code enough as I need to manually placement new
// returned memory address
// The new design has simpler API to use as it allocates and constructs a given object
// like the new keyword

class Allocator
{

public:
    Allocator(const size_t total) : total_size(total) {}
    virtual ~Allocator() = default;

    template <typename T, typename... Types>    // variadic template, as we don't know what object is being created
    T* Create(Types&&... args)       
    {
        // Allocate is pure virtual function, where each derived class have
        // different implmentaion
        void* memory = Allocate(sizeof(T));
        return new (memory) T(std::forward<Types>(args)...); // perfect forwarding
    }

    template <typename T>
    void Destroy(T* obj)
    {
        if (obj) // check if pointer is null
        {
            obj->~T();          // Call destructor
            Deallocate(obj);    // Free the memory
        }
    }

protected:
    // Initialize allocator before using the allocators,
    virtual void Init() = 0;

    // Allocate and return a pointrt to a memory block of given size
    virtual void* Allocate(const size_t size) = 0;

    // Deallocate given ptr (or deallocate recently allocated memory in case of stack allocator)
    virtual void Deallocate(void* ptr = nullptr) = 0;
    
    // Total size of the buffer
    std::size_t total_size = 0;
};


// pure virtual abstract class that acts as interface for different types of memory allocator
// any future custom allocators should also inherit from the Allocator class
//class Allocator {
//protected:
//    std::size_t total_size = 0;
//
//public:
//    Allocator(const size_t total) : total_size(total) {}
//    virtual ~Allocator() = default;
//
//    // Initialize allocator before using the allocators,
//    virtual void Init() = 0;
//
//    // Allocate and return a pointrt to a memory block of given size
//    virtual void* Allocate(const size_t size) = 0;
//
//    // Deallocate given ptr (or deallocate recently allocated memory in case of stack allocator)
//    virtual void Deallocate(void* ptr = nullptr) = 0;
//};