#include "stdafx.h"

#include "MemoryPool.h"


MemoryPool::MemoryPool(int maxEntities)
{
	m_tags = std::vector<std::string>(maxEntities, "N/A");
	m_active = std::vector<bool>(maxEntities, false);

	std::vector<CTransform> trans(maxEntities);
	std::vector<CLifeSpan> life(maxEntities);
	std::vector<CInput> input(maxEntities);
	std::vector<CBoundingBox> box(maxEntities);
	std::vector<CState> state(maxEntities);
	std::vector<CMesh> mesh(maxEntities);
	std::vector<CRigidBody> rb(maxEntities);
	std::vector<CDamage> damage(maxEntities);


	m_data = std::make_tuple(trans, life, input, box, state, mesh, rb, damage);
}

size_t MemoryPool::m_getEmptyIndex() const
{
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		if (!m_active[i]) { return i; }
	}

	return -1;
}

size_t MemoryPool::addEntity(std::string tag)
{
	size_t ID = m_getEmptyIndex();
	m_lastAssignedID = ID;
	m_numEntities++;
	// set all tag[index], active[index] = defaults
	m_tags[ID] = tag;
	m_active[ID] = true;
	return ID;
}

void MemoryPool::removeEntity(size_t ID)
{
	m_numEntities--;
	m_active[ID] = false;
	m_tags[ID] = "N/A";
	getComponent<CTransform>(ID).has = false;
	getComponent<CLifeSpan>(ID).has = false;
	getComponent<CInput>(ID).has = false;
	getComponent<CBoundingBox>(ID).has = false;
	getComponent<CState>(ID).has = false;
	getComponent<CMesh>(ID).has = false;
	getComponent<CRigidBody>(ID).has = false;
	getComponent<CDamage>(ID).has = false;
}

void MemoryPool::clear()
{
	for (int ID = 0; ID < MAX_ENTITIES; ID++)
	{
		removeEntity(ID);
	}
}