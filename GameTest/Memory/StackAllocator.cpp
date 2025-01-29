#include "stdafx.h"

#include "StackAllocator.h"

#include <stdlib.h> /* size_t */
#include <assert.h> /* assert */

StackAllocator::StackAllocator(const std::size_t total_size) : Allocator(total_size) {}

StackAllocator::~StackAllocator()
{
	delete[] m_stack; // free the entire allocated memory when deconstructed
}

void StackAllocator::Init()
{
	m_stack = new char[total_size]; // char is 1 byte, so allocating total_size * 1 byte
}

void* StackAllocator::Allocate(const std::size_t size)
{
	assert(size + m_allocatedSize < total_size);

	void* ptr = m_stack + m_allocatedSize; // the pointer to start of the empty address
	m_allocatedSize += size; // update the size of allocated memory in the stack
	return ptr;
}

void StackAllocator::Deallocate(void* ptr)
{

}