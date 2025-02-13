#pragma once

#include <vector>
#include "../Math/Vec3.h"

class EntityManager; // Forward declaration

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction) {
    }
};

struct AABB
{
    Vec3 min_corner; // bottom, left, far
    Vec3 max_corner; // top, right, near

    AABB() : min_corner(Vec3()), max_corner(Vec3()) {}
    AABB(const Vec3& min_corner, const Vec3& max_corner) : min_corner(min_corner), max_corner(max_corner) {}

    // Merge two AABBs
    inline void expand(const AABB& other)
    {
        min_corner = { (((min_corner.x) < (other.min_corner.x)) ? (min_corner.x) : (other.min_corner.x)),
            (((min_corner.y) < (other.min_corner.y)) ? (min_corner.y) : (other.min_corner.y)),
            (((min_corner.z) < (other.min_corner.z)) ? (min_corner.z) : (other.min_corner.z)) };

        max_corner = { (((max_corner.x) > (other.max_corner.x)) ? (max_corner.x) : (other.max_corner.x)),
            (((max_corner.y) > (other.max_corner.y)) ? (max_corner.y) : (other.max_corner.y)),
            (((max_corner.z) > (other.max_corner.z)) ? (max_corner.z) : (other.max_corner.z)) };

    }

    Vec3 getCorner(int i) const
    {
        return Vec3(
            (i & 1) ? max_corner.x : min_corner.x,
            (i & 2) ? max_corner.y : min_corner.y,
            (i & 4) ? max_corner.z : min_corner.z
        );
    }

    inline bool intersects(const AABB& other) const
    {
        return !(max_corner.x < other.min_corner.x || min_corner.x > other.max_corner.x ||
            max_corner.y < other.min_corner.y || min_corner.y > other.max_corner.y ||
            max_corner.z < other.min_corner.z || min_corner.z > other.max_corner.z);
    }

    bool intersects(const Ray& ray, float& tMin, float& tMax) const;

};

struct Plane
{
    Vec3 normal;
    float distance;

    bool isInside(const Vec3& point) const 
    {
        return normal.dot_product(point) + distance >= 0;
    }
};

struct Frustum
{
    Plane planes[6];

    // Check if an AABB is inside the frustum
    bool isInside(const AABB& aabb) const {
        for (const auto& plane : planes) {
            // Check if all 8 corners of the AABB are outside this plane
            bool outside = true;
            for (int i = 0; i < 8; i++) {
                if (plane.isInside(aabb.getCorner(i))) {
                    outside = false;
                    break;
                }
            }
            if (outside) return false;
        }
        return true;
    }
};


// BVH Node
struct BVHNode {
    AABB aabb;

    size_t parent = -1; // Index of pare (-1 if none)
    size_t left = -1;  // Index of left child (-1 if none)
    size_t right = -1; // Index of right child (-1 if none)

    size_t ID = -1; // Index of the object in the Memory Pool (-1 if node is internal)

    bool isLeaf() const { return (ID == -1) ? false : true; }
    BVHNode() {}
};



// BVH class
class BVH
{
private:
    //std::vector<size_t> m_entityIDs; // don't really need this, waste of memory
    size_t m_root = -1;
    std::vector<size_t> freeList; // list of nodes that are freed, used for recycling

    // aabb is the smallest bounding box that contains all the objects from start to end
    int longestAxis(AABB aabb);
    
    size_t allocateNode();
    void freeNode(size_t nodeIndex);

    //size_t buildRecursive(size_t start, size_t end, const EntityManager& em);
    void insertRecursive(size_t currNodeIndex, size_t entityID, AABB entityAABB, const EntityManager& em);
    void updateAABB(size_t currNodeIndex);

    bool rayCastRecursive(size_t nodeIndex, const Ray& ray, size_t& closestEntityID, float& closestT, const size_t entityShootingFrom) const;
    void frustumCullingRecursive(const Frustum& frustum, std::vector<size_t>& visibleEntities, size_t nodeIndex) const;

public:
    std::vector<BVHNode> nodes;

    BVH()
    {
        nodes.reserve(100000); // max size of tree is 2N-1 so setting to n is reasonable
        freeList.reserve(100); // there are probably going to be less than 100 dynamic entities in the game
    }

    // No longer building with given m_entityIDs using top down method (taking the entire scene and dividing), used for static tree
    //void Build(std::vector<size_t> entityIndices, const EntityManager &em);
    //void Build(const EntityManager& em);


    //Note: it's binary tree, so when it's well balanced it should be average of log n, but in worst case it's O(n)
    // To prevent worst case, I am taking the median of the longest axis but when the objects are clustured it can result in worst case
    // It works well for the games I am building as mostly it's quite evenly spaced out
    
    // Building bottom up by inserting each entity, works better for dynamic tree update
    
    // given the index of the newly created entity, create node and add to BVH
    void InsertEntity(size_t entityID, const EntityManager& em); // O(log n), when building the tree, called n times so O(nlogn)

    // retuns pairs of all the collisions in the scene
    void DetectAllCollisions(std::vector<std::pair<size_t, size_t>>& collisionPairs, size_t nodeIndex) const;

    // given queryNode find all the entitiy indices that intersects with query box
    void Query(size_t queryNodeIndex, std::vector<size_t>& results, size_t currNodeIndex = 0) const; // O(log n)
    void RemoveEntity(size_t nodeIndex, const EntityManager& em); // O(log n)
    void UpdateEntityPosition(size_t entityID, const EntityManager& em); // O(log n)
    bool RayCast(const Ray& ray, size_t& closestEntityID, float& closestT, const size_t entityShootingFrom = -1) const;
    void FrustumCulling(const Frustum& frustum, std::vector<size_t>& visibleEntities) const;

    void Clear();
};

