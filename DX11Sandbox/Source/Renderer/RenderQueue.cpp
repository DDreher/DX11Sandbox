#include "RenderQueue.h"
#include "Mesh.h"

void RenderQueue::Add(const RenderWorkItem& item)
{
    item_indices_.push_back(items_.size());
    items_.push_back(item);
}

void RenderQueue::Sort()
{
    switch (sort_type_)
    {
    case RenderQueueSortType::FrontToBack:
        std::sort(item_indices_.begin(), item_indices_.end(), [this](size_t& idx_a, size_t& idx_b) -> bool
        {
            const RenderWorkItem& a = items_[idx_a];
            const RenderWorkItem& b = items_[idx_b];
            return a.sort_key < b.sort_key;
        });
        break;
    case RenderQueueSortType::BackToFront:
        std::sort(item_indices_.begin(), item_indices_.end(), [this](size_t& idx_a, size_t& idx_b) -> bool
        {
            const RenderWorkItem& a = items_[idx_a];
            const RenderWorkItem& b = items_[idx_b];
            return a.sort_key > b.sort_key;
        });
        break;
    default:
        CHECK_NO_ENTRY();
    }
}

void RenderQueue::Clear()
{
    items_.clear();
    item_indices_.clear();
}
