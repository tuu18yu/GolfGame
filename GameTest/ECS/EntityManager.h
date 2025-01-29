#pragma once

#include "MemoryPool.h"
#include <Memory>
#include "../Physics/AABBTree.h"

// I was trying to avoid using pointers as much as I can but I had delceration issues with how I dealt with forward delceration in circular dependency
// between BVH and EntityManager, causing unexpected behaviour when running in release mode
// I resolved it by making entity manager refer to a pointer to BVH rather than BVH.
// => This allows contents of m_bvh to be modified without changing data in EntityManager (which could be a potential issue with decleration)

// Before in scene I called getBVH, modifying BVH constantly outside of entity manager, I also changed to design to completely encapsulate BVH
// so it's no longer directly accessed outside of entity manager

class EntityManager
{
private:
	std::unique_ptr<BVH> m_bvh; // used a unique pointer as EntityManager will be the only class holding the bvh
	//BVH m_bvh;

public:
	EntityManager() : m_bvh(std::make_unique<BVH>()) {}
	// EntityManager() {};

	template <typename T> std::vector<T>& getComponentVector() const
	{
		return MemoryPool::getInstance().getComponentVector<T>();
	}

	std::vector<bool>& getActiveVector() const
	{
		return MemoryPool::getInstance().getActiveVector();
	}

	const std::string& getTag(size_t ID) const
	{
		return MemoryPool::getInstance().getTag(ID);
	}

	void clear() const
	{
		m_bvh->Clear();
		MemoryPool::getInstance().clear();
	}

	size_t addEntity(const std::string tag) const
	{
		size_t ID = MemoryPool::getInstance().addEntity(tag);
		return ID;
	}

	void addEntityToBVH(size_t ID) const
	{
		m_bvh->InsertEntity(ID, *this);
	}

	void destoryEntity(size_t ID) const
	{
		m_bvh->RemoveEntity(ID, *this);
		MemoryPool::getInstance().removeEntity(ID);
	}


	BVH& getBVH() const { return *m_bvh; }
	//BVH& getBVH() { return *m_bvh; }
};