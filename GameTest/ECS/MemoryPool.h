#pragma once

#include <tuple>
#include <vector>

#include "Component.h"

constexpr int MAX_ENTITIES = 100000;
constexpr int NUM_COMPONENTS = 8;

// MemoryPool where it contains 'rows' or vector of each Component where each 'column' or index of the tuple is an Entity
typedef std::tuple<
	std::vector<CTransform>,
	std::vector<CLifeSpan>,
	std::vector<CInput>,
	std::vector<CBoundingBox>,
	std::vector<CState>,
	std::vector<CMesh>,
	std::vector<CRigidBody>,
	std::vector<CDamage>
> ComponentVecTuple;

// MemoryPool plays the role of storing, deleting, and managing Entities

class MemoryPool
{
private:

	size_t						m_numEntities = 0;
	size_t						m_lastAssignedID = 0;
	ComponentVecTuple			m_data;
	std::vector<std::string>	m_tags;
	std::vector<bool>			m_active;

	// MemoryPool class should only be instantized once as game only needs one Pool => constructor is private
	MemoryPool(int maxEntities);

	size_t m_getEmptyIndex() const;

public:
	// By declaring pool static it creates only one instance when it is first called
	// else it simply returns pool that has already been created
	// This ensures that there is only one memory pool exists through out the program
	static MemoryPool& getInstance()
	{
		static MemoryPool pool(MAX_ENTITIES); // static local variable
		return pool;
	}

	template <typename T> std::vector<T>& getComponentVector()
	{
		return std::get<std::vector<T>>(m_data);
	}

	template <typename T> T& getComponent(size_t entityID)
	{
		return std::get<std::vector<T>>(m_data)[entityID];
	}

	template <typename T> bool hasComponent(size_t entityID)
	{
		return std::get<std::vector<T>>(m_data)[entityID].has;
	}

	std::vector<bool>& getActiveVector()
	{
		return m_active;
	}

	std::string& getTag(size_t entityID)
	{
		return m_tags[entityID];
	}

	size_t addEntity(std::string tag);

	void removeEntity(size_t entityID);
	void clear();

};