#pragma once
#include "Core/Handle.h"

template<typename T, typename U>
class Pool
{
public:
    Pool()
    {
        static constexpr size_t DEFAULT_INITIAL_SIZE = 64;
        capacity_ = DEFAULT_INITIAL_SIZE;
        generations_.resize(capacity_, 0);
        handles_.resize(capacity_, Handle<U>());
        elements_ = operator new(sizeof(T) * capacity_);
        memset(elements_, 0, sizeof(T) * capacity_);
    }

    ~Pool()
    {
        for(const auto& handle : handles_)
        {
            T* element = Get(handle);
            if (element != nullptr)
            {
                Destroy(handle);
            }
        }

        operator delete(elements_);
        elements_ = nullptr;
    }

    template<typename... Args>
    Handle<U> Create(Args&&... args)
    {
        uint32 index;
        if(free_list_.size() > 0)
        {
            index = free_list_.back();
            free_list_.pop_back();
        }
        else
        {
            index = next_creation_index_++;
        }

        if(index >= capacity_)
        {
            // Grow
            uint32 grown_capacity = capacity_ * 2;
            void* grown_elements = operator new(sizeof(T) * grown_capacity);
            memset(grown_elements, 0, sizeof(sizeof(T) * grown_capacity));
            memcpy(grown_elements, elements_, sizeof(T) * capacity_);
            operator delete(elements_);
            capacity_ = grown_capacity;
            elements_ = grown_elements;

            generations_.resize(capacity_, 0);
            handles_.resize(capacity_, Handle<U>());
        }

        const uint32 generation = generations_[index];
        CHECK(generation != Handle<U>::INVALID_GENERATION); // TODO: Handle case where we run out of generations (which will probably never happen...)

        void* element_addr = &((T*) elements_)[index];
        new (element_addr) T(std::forward<Args>(args)...);

        CHECK(handles_[index].IsValid() == false);
        handles_[index] = Handle<U>(index, generation);

        CHECK(handles_[index].IsValid());
        return handles_[index];
    }

    T* Get(Handle<U> handle) const
    {
        if(handle.generation_ != Handle<U>::INVALID_GENERATION && generations_[handle.index_] == handle.generation_)
        {
            T* element = &((T*) elements_)[handle.index_];
            CHECK(element != nullptr);
            return element;
        }
        return nullptr;
    }

    void Destroy(Handle<U> handle)
    {
        CHECK(handle.IsValid());

        T* element = Get(handle);
        if(element != nullptr)
        {
            element->~T();
            memset(element, 0, sizeof(T));
            handles_[handle.index_].generation_ = Handle<U>::INVALID_GENERATION;
            generations_[handle.index_]++;
            free_list_.push_back(handle.index_);
        }
    }

private:
    uint32 capacity_;
    uint32 next_creation_index_ = 0;
    std::vector<uint32> generations_;
    std::vector<Handle<U>> handles_;
    std::deque<uint32> free_list_;

    void* elements_;
};
