#pragma once
#include <vector>

template <typename T>
class ObjectPool
{
private:
    std::vector<T*> m_pool;
    std::vector<T*> m_available;

public:
    ObjectPool(size_t initialSize);
    ~ObjectPool();

    T* Get();
    void Return(T* object);
    void Reset();
};

template <typename T>
ObjectPool<T>::ObjectPool(size_t initialSize)
{
    m_pool.reserve(initialSize);
    m_available.reserve(initialSize);
    for (size_t i = 0; i < initialSize; ++i)
    {
        T* object = new T();
        m_pool.push_back(object);
        m_available.push_back(object);
    }
}

template <typename T>
ObjectPool<T>::~ObjectPool()
{
    for (T* object : m_pool)
    {
        delete object;
    }
}

template <typename T>
T* ObjectPool<T>::Get()
{
    if (m_available.empty())
    {
        T* object = new T();
        m_pool.push_back(object);
        return object;
    }
    else
    {
        T* object = m_available.back();
        m_available.pop_back();
        return object;
    }
}

template <typename T>
void ObjectPool<T>::Return(T* object)
{
    m_available.push_back(object);
}

template <typename T>
void ObjectPool<T>::Reset()
{
    m_available.clear();
    for (T* object : m_pool)
    {
        m_available.push_back(object);
    }
}