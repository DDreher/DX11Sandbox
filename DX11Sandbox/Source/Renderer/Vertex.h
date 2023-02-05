#pragma once

struct VertexPos
{
    VertexPos() {}
    VertexPos(Vec3 in_pos)
        : pos(in_pos)
    {
    }

    VertexPos(float x, float y, float z)
        : pos(x, y, z)
    {
    }

    bool operator==(VertexPos const& other) const
    {
        return pos == other.pos;
    }

    Vec3 pos;

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
    {
        {
            "POSITION", // Just a name for the layout element. Used for mapping in the VS
            0,          // Additional index for mapping. Here we'll actually have POSITION0
            DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,    // Format of the element -> this will be a vec3f
            0,  // input slot index, range 0-15.
            0,  // Memory offset (bytes). Position is at the start of the memory block -> 0.
                // If we add color in same input slot, its offset would be sizeof(vec3f) = 12 bytes
            D3D11_INPUT_PER_VERTEX_DATA,    // InputSlotClass - Used for instancing (?)
            0   // InstanceDataStepRate - Also used for instancing (?)
        },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};
MAKE_HASHABLE(VertexPos, t.pos);

struct VertexPosUV
{
    bool operator==(VertexPosUV const& other) const
    {
        return pos == other.pos && uv == other.uv;
    }

    Vec3 pos;
    Vec2 uv;

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
    {
        { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*input slot*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 1 /*input slot*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};
MAKE_HASHABLE(VertexPosUV, t.pos, t.uv);

struct VertexPosUVNormals
{
    bool operator==(VertexPosUVNormals const& other) const
    {
        return pos == other.pos && uv == other.uv && normal == other.normal;
    }

    Vec3 pos;
    Vec3 normal;
    Vec2 uv;

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
    {
        { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 2 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};
MAKE_HASHABLE(VertexPosUVNormals, t.pos, t.normal, t.uv);

struct VertexPosUVNormalsTangents
{
    bool operator==(VertexPosUVNormalsTangents const& other) const
    {
        return pos == other.pos && uv == other.uv && normal == other.normal;
    }

    Vec3 pos;
    Vec3 normal;
    Vec2 uv;
    Vec3 tangent;

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
    {
        { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 2 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 3 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};
MAKE_HASHABLE(VertexPosUVNormalsTangents, t.pos, t.normal, t.uv, t.tangent);

struct VertexPosColor
{
    Vec3 pos;
    Vec3 color;

    bool operator==(VertexPosColor const& other) const
    {
        return pos == other.pos && color == other.color;
    }

    static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
    {
        { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1 /*slot_idx*/, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
};
MAKE_HASHABLE(VertexPosColor, t.pos, t.color);
