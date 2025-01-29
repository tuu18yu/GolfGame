#include "stdafx.h"

#include "AABBTree.h"
#include "../ECS/EntityManager.h"

#include <algorithm>
#include <limits>

// equation of ray: r(t) = o + t⋅d
bool AABB::intersects(const Ray& ray, float& tMin, float& tMax) const {
    tMin = 0.0f;
    tMax = std::numeric_limits<float>::max();

    bool isInside = true;

    // point in ray is in the box, if the point is within all 3 axes, else not in the box
    for (int i = 0; i < 3; ++i) 
    {
        // if direction on axis is 0, ray is parallel
        if (ray.direction[i] == 0.0f) 
        {
            // ray is outside of AABB on one of the axis, does not intersect
            if (ray.origin[i] < min_corner[i] || ray.origin[i] > max_corner[i]) 
            {
                return false;
            }

            continue; // skip the rest as it is inside the box
        }

        // find the intersection with the aabb on the axis
        float t0 = (min_corner[i] - ray.origin[i]) / ray.direction[i];
        float t1 = (max_corner[i] - ray.origin[i]) / ray.direction[i];

        // swap if t0 is greaterm means shooting ray in opposite direction
        if (t0 > t1) std::swap(t0, t1);

        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);

        // If tMax < tMin, the ray does not intersect the AABB
        if (tMax < tMin)
        {
            return false;
        }

        // Check if the ray is still inside the AABB
        isInside = isInside && (ray.origin[i] >= min_corner[i] && ray.origin[i] <= max_corner[i]);
    }

    // if ray is inside scalar for min is just 0, but as I want to find ray that's not inside aabb i am setting maz
    if (isInside)
    {
        tMin = 0.0f;
        //tMin = std::numeric_limits<float>::max() - 1.0f;
    }

    return true;
}

bool BVH::RayCast(const Ray& ray, size_t& closestEntityID, float& closestT, const size_t entityShootingFrom) const {
    closestT = std::numeric_limits<float>::max();
    closestEntityID = -1;

    return rayCastRecursive(m_root, ray, closestEntityID, closestT, entityShootingFrom);
}

bool BVH::rayCastRecursive(size_t nodeIndex, const Ray& ray, size_t& closestEntityID, float& closestT, const size_t entityShootingFrom) const {
    if (nodeIndex == -1) { return false; }

    const BVHNode& node = nodes[nodeIndex];

    float tMin, tMax;

    // skip this node if the ray doesn't intersect
    if (!node.aabb.intersects(ray, tMin, tMax) || tMin > closestT)
    {
        return false;
    }

    // if the ray hits aabb, check if it's a node
    if (node.isLeaf())
    {
        if (node.ID == entityShootingFrom) { return false; } // skip the entity shooting the ray from

        if (tMin < closestT)
        {
            closestT = tMin;
            closestEntityID = node.ID;
            return true;
        }
        return false;
    }


    bool hitLeft = rayCastRecursive(node.left, ray, closestEntityID, closestT, entityShootingFrom);
    bool hitRight = rayCastRecursive(node.right, ray, closestEntityID, closestT, entityShootingFrom);

    return hitLeft || hitRight;
}



int BVH::longestAxis(AABB aabb)
{
    // Get the distance between box's max corner and min corner
    Vec3 distance = aabb.max_corner - aabb.min_corner;

    // Choose the axis with the longest distance
    // x is longer than y and z
    if (distance.x > distance.y && distance.x > distance.z) { return 0; }
    // y is greater than x and z
    if (distance.y > distance.z) { return 1; }
    // z is greater than x and y
    else { return 2; }
}


void BVH::InsertEntity(size_t entityID, const EntityManager& em)
{
    CBoundingBox& box = em.getComponentVector<CBoundingBox>()[entityID];
    AABB entityAABB(box.min_corner(), box.max_corner());

    //m_entityIDs.push_back(entityID);

    if (m_root == -1) {
        m_root = allocateNode();
        BVHNode& rootNode = nodes[m_root];

        rootNode.aabb = entityAABB;
        rootNode.ID = entityID;
        rootNode.parent = -1;
        rootNode.left = -1;
        rootNode.right = -1;

        box.nodeID = m_root;
    }
    else {
        // Insert the new entity into the tree
        insertRecursive(m_root, entityID, entityAABB, em);
    }
}

void BVH::insertRecursive(size_t currNodeIndex, size_t entityID, AABB entityAABB, const EntityManager& em)
{
   
    // if the currNode, inserting the entity is a leaf (the actual object)
    // create an internal node with currNode and newNode as children
    if (nodes[currNodeIndex].isLeaf()) {
        size_t oldEntityID = nodes[currNodeIndex].ID;

        AABB combinedAABB(entityAABB.min_corner, entityAABB.max_corner);
        combinedAABB.expand(nodes[currNodeIndex].aabb);
        int axis = longestAxis(combinedAABB);
       
        // create left and right child
        size_t leftChildIndex = allocateNode();
        size_t rightChildIndex = allocateNode();

        CBoundingBox& addedEntityBox = em.getComponentVector<CBoundingBox>()[entityID];
        CBoundingBox& currNodeBox = em.getComponentVector<CBoundingBox>()[oldEntityID];

        // new Entity is the left child
        if (entityAABB.min_corner[axis] < nodes[currNodeIndex].aabb.min_corner[axis])
        {
            nodes[leftChildIndex].aabb = entityAABB;
            nodes[leftChildIndex].ID = entityID;
            nodes[leftChildIndex].parent = currNodeIndex;
            addedEntityBox.nodeID = leftChildIndex;
       
            nodes[rightChildIndex].aabb = nodes[currNodeIndex].aabb;
            nodes[rightChildIndex].ID = oldEntityID;
            nodes[rightChildIndex].parent = currNodeIndex;
            currNodeBox.nodeID = rightChildIndex;

        }
        else
        {
            nodes[leftChildIndex].aabb = nodes[currNodeIndex].aabb;
            nodes[leftChildIndex].ID = oldEntityID;
            nodes[leftChildIndex].parent = currNodeIndex;
            currNodeBox.nodeID = leftChildIndex;

            nodes[rightChildIndex].aabb = entityAABB;
            nodes[rightChildIndex].ID = entityID;
            nodes[rightChildIndex].parent = currNodeIndex;
            addedEntityBox.nodeID = rightChildIndex;

        }

        // currNode is now internal node containing two children and parent is still the same
        nodes[currNodeIndex].ID = -1;
        nodes[currNodeIndex].left = leftChildIndex;
        nodes[currNodeIndex].right = rightChildIndex;
        nodes[currNodeIndex].aabb = combinedAABB;
      
    }
    else
    {
        // if this is an internal node, find the best child to insert the entity
        BVHNode& leftChild = nodes[nodes[currNodeIndex].left];
        BVHNode& rightChild = nodes[nodes[currNodeIndex].right];

        // determine which child to insert into based on median split
        int axis = longestAxis(nodes[currNodeIndex].aabb);

        // find the midpoint on the axis for new entity adding, left child, right child
        float entityMidpoint = entityAABB.min_corner[axis] + (entityAABB.max_corner[axis] - entityAABB.min_corner[axis]) / 2.0f;
        float leftMidpoint = leftChild.aabb.min_corner[axis] + (leftChild.aabb.max_corner[axis] - leftChild.aabb.min_corner[axis]) / 2.0f;
        float rightMidpoint = rightChild.aabb.min_corner[axis] + (rightChild.aabb.max_corner[axis] - rightChild.aabb.min_corner[axis]) / 2.0f;

        // determine whether entity is close to left or right, add to the one closer
        if (std::abs(entityMidpoint - leftMidpoint) < std::abs(entityMidpoint - rightMidpoint)) {
            insertRecursive(nodes[currNodeIndex].left, entityID, entityAABB, em);
        }
        else {
            insertRecursive(nodes[currNodeIndex].right, entityID, entityAABB, em);
        }

        // Update the current node's AABB to include the new entity
        nodes[currNodeIndex].aabb.expand(entityAABB);
    }
}


void BVH::Query(size_t queryNodeIndex, std::vector<size_t>& results, size_t nodeIndex) const 
{
    if (queryNodeIndex == nodeIndex) return; // skip same node

    const AABB& queryBox = nodes[queryNodeIndex].aabb;
    const BVHNode& node = nodes[nodeIndex];

    // Check if the queryBox intersects the node's AABB
    if (!node.aabb.intersects(queryBox)) {
        // If there's no intersection skip the entire branch, as it will not intersect with any of the children
        return;
    }

    // If this is a leaf node, add the entity ID to the results
    if (node.isLeaf()) {
        results.push_back(node.ID);
    }

    else {
        // Recursively check the left and right children until reaches to leaves that intersect
        if (node.left != -1) { Query(queryNodeIndex, results, node.left); }
        if (node.right != -1) { Query(queryNodeIndex, results, node.right); }
    }
}

void BVH::DetectAllCollisions(std::vector<std::pair<size_t, size_t>>& collisionPairs, size_t nodeIndex) const {
    if (nodeIndex == -1) return;

    const BVHNode& node = nodes[nodeIndex];

    if (node.isLeaf()) {

        // find all the collision with the node
        std::vector<size_t> results;
        Query(nodeIndex, results, m_root);

        for (size_t resultID : results) {
            if (resultID != node.ID) {
                // get all the entityIDs
                collisionPairs.emplace_back(node.ID, resultID);
            }
        }
    }
    else {
        // Recursively check left and right children
        DetectAllCollisions(collisionPairs, node.left);
        DetectAllCollisions(collisionPairs, node.right);
    }
}

// Initially, I just deleted entity from the entityIDs list and rebuilt the entire tree
//void BVH::RemoveEntity(size_t entityID, const EntityManager& em)
//{
//    m_entityIDs.erase(std::remove(m_entityIDs.begin(), m_entityIDs.end(), entityID), m_entityIDs.end());
//    Build(em);
//}

// Dynamic approach
void BVH::RemoveEntity(size_t entityID, const EntityManager& em)
{
    
    // remove the entity from m_entityIDs
    //m_entityIDs.erase(std::remove(m_entityIDs.begin(), m_entityIDs.end(), entityID), m_entityIDs.end());

    // get the BVH node of the wanted entity
    CBoundingBox& entityBox = em.getComponentVector<CBoundingBox>()[entityID];
    size_t nodeIndex = entityBox.nodeID;

    if (nodeIndex == -1) return;
    BVHNode& node = nodes[nodeIndex];

    // if the node wanted to delete is root, clear the tree
    // node the wanted node will always be a leaf
    if (node.parent == -1)
    {
        nodes.clear();
        //m_entityIDs.clear();
        freeList.clear();
        m_root = -1;
        return;
    }

    // get parent and sibling node
    size_t parentIndex = node.parent;
    BVHNode& parent = nodes[parentIndex];

    size_t siblingIndex = (parent.left == nodeIndex) ? parent.right : parent.left;
    BVHNode& sibling = nodes[siblingIndex];

    // since, it's a binary tree where leaves are always the objects, we need to delete the parent node
    // and let sibling node take the parent's spot in the tree
    if (parent.parent == -1)
    {
        // if the parent is the root, make the sibling the new root
        sibling.parent = -1;
        m_root = siblingIndex;
    }
    else
    {
        // update the grandparent to point to the sibling, not the parent
        size_t grandParentIndex = parent.parent;
        BVHNode& grandParent = nodes[grandParentIndex];

        if (grandParent.left == parentIndex) { grandParent.left = siblingIndex; }
        else { grandParent.right = siblingIndex; }

        sibling.parent = grandParentIndex;
        updateAABB(grandParentIndex);
    }

    // free the parent and the node
    freeNode(parentIndex);
    freeNode(nodeIndex);

    // freeing memory in the memory pool is dealt in EntityManager
}

void BVH::UpdateEntityPosition(size_t entityID, const EntityManager& em)
{
    RemoveEntity(entityID, em);
    InsertEntity(entityID, em);
}

size_t BVH::allocateNode() {
    // if there is a free node, reuse that node
    if (!freeList.empty())
    {
        size_t nodeIndex = freeList.back();
        freeList.pop_back();
        return nodeIndex;
    }
    // else add a node
    nodes.emplace_back();
    return nodes.size() - 1;
}

void BVH::freeNode(size_t nodeIndex) {
    BVHNode& node = nodes[nodeIndex];
    node = BVHNode(); // clear the node to default
    freeList.push_back(nodeIndex); // add the index to the free list
}

void BVH::updateAABB(size_t currNodeIndex) {
    if (currNodeIndex == -1) return; // reaches the root

    BVHNode& node = nodes[currNodeIndex];

    // all the parent nodes should be internal nodes
    if (!node.isLeaf()) {
        AABB newAABB;

        // internal node should always have left and right children, but ensure
        if (node.left != -1) { newAABB = nodes[node.left].aabb;}
        if (node.right != -1) { newAABB.expand(nodes[node.right].aabb); }

        node.aabb = newAABB;

        // keep moving upwards
        updateAABB(node.parent);
    }
}



void BVH::Clear()
{
    nodes.clear();
    freeList.clear();
    m_root = -1;
}

//void BVH::Build(std::vector<size_t> entityIndices, const EntityManager &em)
//{
//    m_entityIDs = entityIndices;
//    nodes.clear();
//    buildRecursive(0, entityIndices.size(), em);
//}
//
//void BVH::Build(const EntityManager& em)
//{
//    nodes.clear();
//    freeList.clear();
//    buildRecursive(0, m_entityIDs.size(), em);
//}
//
//// inspired by https://www.haroldserrano.com/blog/visualizing-the-boundary-volume-hierarchy-collision-algorithm
//size_t BVH::buildRecursive(size_t start, size_t end, const EntityManager &em) {
//
//    // Case 0: There is no object
//    if (start >= end) { return -1; }
//
//    // insert default node to the vector
//    size_t nodeIndex = nodes.size();
//    BVHNode node;
//    nodes.push_back(node);
//
//    nodes[nodeIndex].left = -1;
//    nodes[nodeIndex].right = -1;
//    nodes[nodeIndex].parent = -1;
//
//
//    // Case 1: 1 object, always a leaf 
//    if (end - start == 1) {
//        if (nodeIndex == 0)
//        {
//            m_root = 0;
//        }
//        CBoundingBox &box = em.getComponentVector<CBoundingBox>()[m_entityIDs[start]];
//
//        nodes[nodeIndex].aabb = AABB(box.min_corner(), box.max_corner());
//        nodes[nodeIndex].ID = m_entityIDs[start];
//        box.nodeID = nodeIndex;
//
//        return nodeIndex;
//    }
//
//    // Case 2: At least 2 objects (size_ternal node)
//   
//    // Create an AABB that contains all the object from start to end
//    CBoundingBox box = em.getComponentVector<CBoundingBox>()[m_entityIDs[start]];
//    AABB combinedAABB = AABB(box.min_corner(), box.max_corner());
//
//    for (size_t i = start + 1; i < end; ++i) {
//        const CBoundingBox& other = em.getComponentVector<CBoundingBox>()[m_entityIDs[i]];
//
//        // Keep expanding the box to include all the objects from start to end
//        combinedAABB.expand(AABB(other.min_corner(), other.max_corner()));
//    }
//
//    // Set the default node to newly created AABB
//    nodes[nodeIndex].aabb = combinedAABB;
//    nodes[nodeIndex].ID = -1;
//
//    // get the axis with the longest length
//    int axis = longestAxis(combinedAABB);
//
//    // sort the m_entityIDs vector such that objects are in order at longest axis
//    std::sort(m_entityIDs.begin() + start, m_entityIDs.begin() + end, [axis, &em](size_t a, size_t b)
//        {
//            CBoundingBox box_a = em.getComponentVector<CBoundingBox>()[a];
//            CBoundingBox box_b = em.getComponentVector<CBoundingBox>()[b];
//            return box_a.min_corner()[axis] < box_b.min_corner()[axis];
//        });
//
//    // partition the object at the median
//    size_t mid = (start + end) / 2;
//
//    // recursively Build
//    nodes[nodeIndex].left = buildRecursive(start, mid, em);
//    nodes[nodeIndex].right = buildRecursive(mid, end, em);
//
//    // assign the current node as the parent of its children
//    if (nodes[nodeIndex].left != -1) {
//        nodes[nodes[nodeIndex].left].parent = nodeIndex;
//    }
//    if (nodes[nodeIndex].right != -1) {
//        nodes[nodes[nodeIndex].right].parent = nodeIndex;
//    }
//
//    return nodeIndex;
//}