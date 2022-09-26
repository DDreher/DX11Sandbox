#pragma once

struct StaticMesh;
class Material;

enum RenderQueueSortType
{
    FrontToBack,
    BackToFront
};

struct RenderWorkItem
{
    // TODO: For now we only sort by depth.
    // A better solution would be to generate a sort key which also considers materials, render states, etc.
    // See: https://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
    // http://realtimecollisiondetection.net/blog/?p=86
    float sort_key = 0.0f;
    StaticMesh* mesh = nullptr; // TODO: handle?
};

class RenderQueue
{
public:
    RenderQueue(RenderQueueSortType sort_type) : sort_type_(sort_type) {}

    void Add(const RenderWorkItem& item);
    void Sort();
    void Submit();

private:
    RenderQueueSortType sort_type_;
    std::vector<RenderWorkItem> items_;
    std::vector<size_t> item_indices_;
};