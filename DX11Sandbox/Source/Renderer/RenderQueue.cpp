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
            return a.distance_from_camera < b.distance_from_camera;
        });
        break;
    case RenderQueueSortType::BackToFront:
        std::sort(item_indices_.begin(), item_indices_.end(), [this](size_t& idx_a, size_t& idx_b) -> bool
        {
            const RenderWorkItem& a = items_[idx_a];
            const RenderWorkItem& b = items_[idx_b];
            return a.distance_from_camera > b.distance_from_camera;
        });
        break;
    default:
        CHECK_NO_ENTRY();
    }
}

void RenderQueue::Submit()
{
    Sort();

    for(size_t idx : item_indices_)
    {
        const RenderWorkItem& item = items_[idx];

        // TODO: How to handle per-object cbuffer binding? Currently it's in the model but this doesn't feel right...
        item.mesh->model->Bind();
        item.mesh->Bind();
        item.mesh->Render();
    }

    items_.clear();
    item_indices_.clear();
}
