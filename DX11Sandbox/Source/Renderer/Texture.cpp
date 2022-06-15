#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"

Texture::Texture(const TextureDesc& desc)
{
    file_path_ = desc.file_path;
    stbi_uc* tex_data = stbi_load(file_path_.c_str(), &width_, &height_, &num_channels_, STBI_rgb_alpha);
    Create((void*)tex_data);
    stbi_image_free(tex_data);
}

void Texture::Create(void* data)
{
    D3D11_TEXTURE2D_DESC texture_desc = {};
    texture_desc.Width = width_;
    texture_desc.Height = height_;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  // DXGI_FORMAT_R8G8B8A8_UNORM -> Texture already in linear color space
                                                            // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB -> Texture in SRGB space, i.e. we have to convert to linear first!
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA texture_subresource_data = {};
    texture_subresource_data.pSysMem = data;
    texture_subresource_data.SysMemPitch = width_ * 4 * sizeof(uint8);;
    DX11_VERIFY(gfx::device->CreateTexture2D(&texture_desc, &texture_subresource_data, &handle_));
    SetDebugName(handle_.Get(), file_path_);
    DX11_VERIFY(gfx::device->CreateShaderResourceView(handle_.Get(), nullptr, &srv_));
}
