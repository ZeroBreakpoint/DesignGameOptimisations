#pragma once
#include <vector>

template <typename T>
class ObjectPool
{
private:
    std::vector<T*> m_pool;       // All created objects
    std::vector<T*> m_available;  // Currently available (unused) objects

public:
    // Create 'initialSize' objects and mark all as available
    ObjectPool(size_t initialSize);
    ~ObjectPool();

    // Acquire an object: either reuse or allocate a new one
    T* Get();

    // Return an object back to the pool for later reuse
    void Return(T* object);

    // Reset pool: mark every object as available again
    void Reset();
};

template <typename T>
ObjectPool<T>::ObjectPool(size_t initialSize)
{
    m_pool.reserve(initialSize);
    m_available.reserve(initialSize);

    for (size_t i = 0; i < initialSize; ++i)
    {
        T* object = new T();       // Allocate new T
        m_pool.push_back(object);
        m_available.push_back(object);
    }
}

template <typename T>
ObjectPool<T>::~ObjectPool()
{
    // Delete all allocated objects
    for (T* object : m_pool)
        delete object;
}

template <typename T>
T* ObjectPool<T>::Get()
{
    if (m_available.empty())
    {
        // Pool exhausted: allocate additional object
        T* object = new T();
        m_pool.push_back(object);
        return object;
    }
    else
    {
        // Reuse an available object
        T* object = m_available.back();
        m_available.pop_back();
        return object;
    }
}

template <typename T>
void ObjectPool<T>::Return(T* object)
{
    // Add it back to available list
    m_available.push_back(object);
}

template <typename T>
void ObjectPool<T>::Reset()
{
    // Make all objects available again
    m_available.clear();
    for (T* object : m_pool)
        m_available.push_back(object);
}