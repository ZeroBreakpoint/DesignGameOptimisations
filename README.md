# DesignGameOptimisations

This is my optimised version of a C++ raylib game featuring an octopus destroyer and multiple autonomous critters.  The project was originally provided as an unoptimised baseline.  I have vastly improved its performance and design through the implementation of object pooling, a texture resource manager and a spatial partitioning system using a quadtree.  These changes adhere to both the performance and modularity requirements of the original design brief.

## Optimisations Implemented

### 1. **Object Pooling**

Implemented via a templated `ObjectPool<T>` class. Rather than allocating and deallocating critters every frame, the application now reuses critter instances:

- Avoids constant heap allocation/deallocation
- Reduces memory fragmentation
- Improves runtime performance and stability

Critters are returned to the pool when destroyed and reinitialized on respawn.


### 2. **Texture Resource Manager (Hash Map)**

Added a singleton-like `TextureManager` that caches textures using an `unordered_map` (hash map):

- Prevents redundant loading of textures per respawn
- Ensures only one instance of each texture exists in memory
- Simplifies unloading and cleanup
  
### 3. **Quadtree Spatial Partitioning**
Introduced a custom QuadTree class for spatial partitioning and efficient collision detection:

Reduces O(n²) collision checks to O(n log n + k), where k is the number of local collisions.

Dynamically subdivides space and queries nearby critters only.

Greatly improves frame rate stability.
