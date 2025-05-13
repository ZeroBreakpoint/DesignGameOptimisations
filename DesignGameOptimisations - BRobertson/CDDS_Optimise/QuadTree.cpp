#include "QuadTree.h"
#include "Critter.h"

QuadTree::QuadTree(const AABB& region)
    : m_region(region)           // Copy in the region bounds
    , m_divided(false)           // No children yet
    , m_northWest(nullptr)       // Initialise child pointers to nullptr
    , m_northEast(nullptr)
    , m_southWest(nullptr)
    , m_southEast(nullptr)
{}

// Destructor

QuadTree::~QuadTree()
{
    Clear();  // Recursively delete all children and clear stored points
}

// Subdivide this node into four equal quadrants.  Splits m_region into NW, NE, SW and SE child nodes whenever the capacity threshold is exceeded.

void QuadTree::Subdivide()
{
    // Calculate half‐width and half‐height of this region
    float x = m_region.bounds.x;
    float y = m_region.bounds.y;
    float w = m_region.bounds.width * 0.5f;  // Half the width
    float h = m_region.bounds.height * 0.5f;  // Half the height

    // Define the four quadrants
    AABB nw{ { x,     y,     w, h } };           // North‐West
    AABB ne{ { x + w, y,     w, h } };           // North‐East
    AABB sw{ { x,     y + h, w, h } };           // South‐West
    AABB se{ { x + w, y + h, w, h } };           // South‐East

    // Allocate child quadtrees on the heap
    m_northWest = new QuadTree(nw);
    m_northEast = new QuadTree(ne);
    m_southWest = new QuadTree(sw);
    m_southEast = new QuadTree(se);

    m_divided = true;  // Mark that we have subdivided
}

bool QuadTree::Insert(Critter* critter, const Vector2& position)
{
    if (!m_region.Contains(position))
        return false;  // Outside this node’s bounds

    if (m_points.size() < CAPACITY) {
        m_points.push_back(critter);
        return true;
    }

    if (!m_divided)
        Subdivide();

    // Try inserting into children
    if (m_northWest->Insert(critter, position)) return true;
    if (m_northEast->Insert(critter, position)) return true;
    if (m_southWest->Insert(critter, position)) return true;
    if (m_southEast->Insert(critter, position)) return true;

    // (Should never happen)
    return false;
}

void QuadTree::Query(const AABB& range, std::vector<Critter*>& outResults) const
{
    // If query region doesn't intersect this node, bail out
    if (!m_region.Intersects(range))
        return;

    // Check points at this node
    for (auto* critter : m_points) {
        if (range.Contains(critter->GetPosition()))
            outResults.push_back(critter);
    }

    // If subdivided, query children
    if (m_divided) {
        m_northWest->Query(range, outResults);
        m_northEast->Query(range, outResults);
        m_southWest->Query(range, outResults);
        m_southEast->Query(range, outResults);
    }
}

void QuadTree::Clear()
{
    m_points.clear();
    if (m_divided) {
        delete m_northWest;
        delete m_northEast;
        delete m_southWest;
        delete m_southEast;
        m_northWest = m_northEast = m_southWest = m_southEast = nullptr;
        m_divided = false;
    }
}