#include "TextureData.h"
#include <Core/DXCommonFunction.h>

int TextureData::readTextureCount_ = 0;

TextureData::TextureData(std::string filePath, DXDevice* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager) {
    //TextureResourceを作成
    DirectX::ScratchImage mipImages = CreateMipImages(filePath);
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

    ID3D12Resource* rawResource = nullptr;
    rawResource = CreateTextureResource(device->GetDevice(), metadata);
	textureResource_.Attach(rawResource);
	rawResource = UploadTextureData(textureResource_.Get(), mipImages, device->GetDevice(), commandList);
	intermadiateResource_.Attach(rawResource);

    //画像サイズの取得
	width_ = static_cast<int>(metadata.width);
	height_ = static_cast<int>(metadata.height);

    //metadataをもとにSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    //SRVを作成するDescriptorHeapの場所を決める
    //先頭はImGuiが使っているのでその次を使う
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvManager->GetCPUHandle(textureResource_.Get());
	textureGpuHandle_ = srvManager->GetGPUHandle(textureResource_.Get());

    //SRVを作成する
    device->GetDevice()->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU);
}
