#pragma once

template<typename T>
class Singleton
{
public:
    static T& Instance()
    {
        CHECK(instance_ != nullptr);
        return *instance_;
    }

    static void Init()
    {
        CHECK(instance_ == nullptr);
        instance_ = MakeUnique<T>();
    }

    static void Destroy()
    {
        CHECK(instance_ != nullptr);
        instance_.reset();
    }

protected:
    static inline UniquePtr<T> instance_ = nullptr;
};
