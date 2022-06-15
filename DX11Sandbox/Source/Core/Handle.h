#pragma once

// References:
// https://floooh.github.io/2018/06/17/handles-vs-pointers.html
// https://blog.molecular-matters.com/2013/05/17/adventures-in-data-oriented-design-part-3b-internal-references/

template<typename T>
class Handle
{
    template<typename, typename>
    friend class Pool;

public:
    Handle() : index_(0), generation_(INVALID_GENERATION) {};
    bool IsValid() const { return generation_ != Handle::INVALID_GENERATION; }

    bool operator<(const Handle other) const { return id_ < other.id_; }
    bool operator>(const Handle other) const { return id_ > other.id_; }
    bool operator<=(const Handle other) const { return id_ <= other.id_; }
    bool operator>=(const Handle other) const { return id_ >= other.id_; }
    bool operator==(const Handle other) const { return id_ == other.id_; }
    bool operator!=(const Handle other) const { return id_ != other.id_; }

    static constexpr uint32 INVALID_GENERATION = 0xffffffff;

private:
    Handle(uint32 index, uint32 generation) : index_(index), generation_(generation) {}

    union
    {
        struct
        {
            uint32 generation_ ;  // gets incremented each time a data item is created
            uint32 index_;  // index into the allocated data
        };

        uint64 id_;
    };
};
