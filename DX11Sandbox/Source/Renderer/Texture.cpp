#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"

Texture::Texture(const TextureDesc& desc)
    : file_path_(desc.file_path), texture_space_(desc.texture_space), hasMipMaps_(desc.generateMipMaps)
{
    stbi_uc* tex_data = stbi_load(file_path_.c_str(), &width_, &height_, &num_channels_, STBI_rgb_alpha);
    Create((uint8*) tex_data);
    stbi_image_free(tex_data);
}

uint32 Texture::CalcNumMipLevels(uint32 width, uint32 height)
{
    int num_levels = 1;
    while (width > 1 || height > 1)
    {
        width = std::max(width / 2, uint32(1));
        height = std::max(height / 2, uint32(1));
        ++num_levels;
    }

    return num_levels;
}

void Texture::Create(uint8* data)
{
    CHECK(data != nullptr);

    if(hasMipMaps_)
    {
        // Create staging buffer where we'll create the mip chain
        D3D11_TEXTURE2D_DESC staging_desc = {};
        staging_desc.Width = width_;
        staging_desc.Height = height_;
        staging_desc.ArraySize = 1;
        staging_desc.SampleDesc.Count = 1;
        staging_desc.Format = texture_space_ == TextureSpace::SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
        staging_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        staging_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        // Note: Not initial data! We have to copy the first mipmap level over ourselves

        //D3D11_SUBRESOURCE_DATA staging_subresource_data = {};
        //staging_subresource_data.pSysMem = (void*)data;
        //staging_subresource_data.SysMemPitch = width_ * 4 * sizeof(uint8);

        ComPtr<ID3D11Texture2D> staging_handle = nullptr;
        DX11_VERIFY(gfx::device->CreateTexture2D(&staging_desc, nullptr, &staging_handle));
        SetDebugName(staging_handle.Get(), fmt::format("[Staging] {0}", file_path_));

        // Copy first mip level of texture into staging buffer
        gfx::device_context->UpdateSubresource(staging_handle.Get(), 0, nullptr, (void*) data, width_ * 4 * sizeof(uint8), 0);

        D3D11_SHADER_RESOURCE_VIEW_DESC staging_srv_desc = {};
        staging_srv_desc.Format = staging_desc.Format;
        staging_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        staging_srv_desc.Texture2D.MostDetailedMip = 0;
        staging_srv_desc.Texture2D.MipLevels = -1;  // -1 => Create the entire mip chain

        ComPtr<ID3D11ShaderResourceView> staging_srv = nullptr;
        DX11_VERIFY(gfx::device->CreateShaderResourceView(staging_handle.Get(), &staging_srv_desc, &staging_srv));

        // Calculate mip maps on device
        gfx::device_context->GenerateMips(staging_srv.Get());

        // Finally copy the generated mips into a buffer with default usage
        // (can't use immutable as CopyResource doesn't work...)
        D3D11_TEXTURE2D_DESC texture_desc = {};
        texture_desc.Width = width_;
        texture_desc.Height = height_;
        texture_desc.MipLevels = 0;
        texture_desc.ArraySize = 1;

        // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB -> Texture in SRGB space, i.e. we have to convert to linear first!
        // DXGI_FORMAT_R8G8B8A8_UNORM -> Texture already in linear color space
        texture_desc.Format = texture_space_ == TextureSpace::SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
        texture_desc.SampleDesc.Count = 1;
        texture_desc.SampleDesc.Quality = 0;
        texture_desc.Usage = D3D11_USAGE_DEFAULT;
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        DX11_VERIFY(gfx::device->CreateTexture2D(&texture_desc, nullptr, &handle_));
        SetDebugName(handle_.Get(), file_path_);

        gfx::device_context->CopyResource(handle_.Get(), staging_handle.Get());

        DX11_VERIFY(gfx::device->CreateShaderResourceView(handle_.Get(), nullptr, &srv_));
    }
    else
    {
        D3D11_TEXTURE2D_DESC texture_desc = {};
        texture_desc.Width = width_;
        texture_desc.Height = height_;
        texture_desc.MipLevels = 1;
        texture_desc.ArraySize = 1;

        // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB -> Texture in SRGB space, i.e. we have to convert to linear first!
        // DXGI_FORMAT_R8G8B8A8_UNORM -> Texture already in linear color space
        texture_desc.Format = texture_space_ == TextureSpace::SRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
        texture_desc.SampleDesc.Count = 1;
        texture_desc.SampleDesc.Quality = 0;
        texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA texture_subresource_data = {};
        texture_subresource_data.pSysMem = (void*) data;
        texture_subresource_data.SysMemPitch = width_ * 4 * sizeof(uint8);

        DX11_VERIFY(gfx::device->CreateTexture2D(&texture_desc, &texture_subresource_data, &handle_));
        SetDebugName(handle_.Get(), file_path_);
        DX11_VERIFY(gfx::device->CreateShaderResourceView(handle_.Get(), nullptr, &srv_));
    }
}
