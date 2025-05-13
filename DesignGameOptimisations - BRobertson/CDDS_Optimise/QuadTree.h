#pragma once

#include "raylib.h"
#include <vector>

// Forward‐declare Critter so we can store pointers to them
class Critter;

// Axis‐aligned rectangle for region queries.

struct AABB {
    Rectangle bounds;
    // Check if a point lies within this box
    bool Contains(const Vector2& point) const {
        return CheckCollisionPointRec(point, bounds);
    }
    // Check if two boxes intersect
    bool Intersects(const AABB& other) const {
        return CheckCollisionRecs(bounds, other.bounds);
    }
};

//Quadtree node for spatial partitioning of Critters.

class QuadTree {
private:
    static const int CAPACITY = 4;   // Max critters before subdividing
    AABB        m_region;            // This node’s region in world‐space
    std::vector<Critter*> m_points;  // Critters contained directly here

    bool        m_divided;           // Have we subdivided yet
    QuadTree* m_northWest;
    QuadTree* m_northEast;
    QuadTree* m_southWest;
    QuadTree* m_southEast;

    // Subdivide this node into four children
    void Subdivide();

public:
    QuadTree(const AABB& region);
    ~QuadTree();

    // Insert a critter pointer if within bounds
    bool Insert(Critter* critter, const Vector2& position);

    // Gather all critters within a query region
    void Query(const AABB& range, std::vector<Critter*>& outResults) const;

    // Optional: clear and reset the tree
    void Clear();
};
