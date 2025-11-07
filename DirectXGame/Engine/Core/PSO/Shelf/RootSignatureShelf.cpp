#include "RootSignatureShelf.h"
#include <cassert>

using namespace Logger;

RootSignatureShelf::RootSignatureShelf(ID3D12Device* device) {

	logger_ = getLogger("RootSignature");

#pragma region DescriptorTable

	//テクスチャ1枚用のDescriptorRange
    D3D12_DESCRIPTOR_RANGE textureDescriptor[1] = {};
    textureDescriptor[0].BaseShaderRegister = 0;
    textureDescriptor[0].NumDescriptors = 1;
    textureDescriptor[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    textureDescriptor[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

    D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
    staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//フィルタリングの方法
    staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//比較しない
    staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;    //ありったけのMipmapを使う
    staticSampler[0].ShaderRegister = 0;    //レジスタ番号0
    staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う

    // PostEffect用のCLAMPモードサンプラー
    D3D12_STATIC_SAMPLER_DESC clampSampler[1] = {};
    clampSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    clampSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;  // CLAMPに変更
    clampSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;  // CLAMPに変更
    clampSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;  // CLAMPに変更
    clampSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    clampSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
    clampSampler[0].ShaderRegister = 0;
    clampSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    //ありあり
    {
        //RootSignature作成
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        //RootParameter作成
        D3D12_ROOT_PARAMETER rootParameters[4] = {};

        //Material
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;     //PixelShaderで使う
        rootParameters[0].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド

        //Matrix
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;        //CBVを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;     //VertexShaderで使う
        rootParameters[1].Descriptor.ShaderRegister = 0;                        //レジスタ番号0とバインド

        //Texture
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//テーブルを使う
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[2].DescriptorTable.pDescriptorRanges = textureDescriptor;	//テーブルの中身
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(textureDescriptor);	//テーブルの数

        //DirectionalLight
        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//CBVを使う
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使う
        rootParameters[3].Descriptor.ShaderRegister = 1;	//レジスタ番号1とバインド

        descriptionRootSignature.pParameters = rootParameters;                  //ルートパラメータ配列へのポインタ
        descriptionRootSignature.NumParameters = _countof(rootParameters);      //配列の長さ

        descriptionRootSignature.pStaticSamplers = staticSampler;              //StaticSamplerの配列へのポインタ
        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);   //配列の長さ


        Microsoft::WRL::ComPtr<ID3D10Blob> signatureBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

        //シリアライズしてバイナリにする
        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
            D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (FAILED(hr)) {
            logger_->error("Error of Creating RootSignature");
            logger_->error(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
            assert(false);
        }

        //バイナリをもとに生成
        ID3D12RootSignature* rootSignature = nullptr;
        hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
            signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        RootSignatureID id;
        id.id = nextID_++;
        rootSignatures_[id] = rootSignature;
    }
}

RootSignatureShelf::~RootSignatureShelf() {
    for(auto& rootSignature : rootSignatures_) {
        if (rootSignature.second) {
            rootSignature.second->Release();
        }
	}
}

RootSignatureID RootSignatureShelf::CreateRootSignature(ID3D12Device* device, std::vector<std::pair<D3D12_GPU_VIRTUAL_ADDRESS, ShaderType>> cbv, std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> srv, std::vector<std::pair<D3D12_GPU_DESCRIPTOR_HANDLE, ShaderType>> uav) {
    
    //Configの作成
	RSConfig config;
    for (const auto& cbvPair : cbv) {
        config.views.emplace_back(ViewType::CBV, cbvPair.second);
    }
    for(const auto& srvPair : srv) {
        config.views.emplace_back(ViewType::SRV, srvPair.second);
	}
    for (const auto& uavPair : uav) {
        config.views.emplace_back(ViewType::UAV, uavPair.second);
    }

    if(configToIDMap_.find(config) != configToIDMap_.end()) {
		return configToIDMap_[config];
	}

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    std::vector<D3D12_ROOT_PARAMETER> rootParameters;


    //CBV設定 -======================================================================================

	int vsRegister = 0;
	int psRegister = 0;

    for(const auto& cbvPair : cbv) {

        D3D12_ROOT_PARAMETER param{};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

        if(cbvPair.second == ShaderType::VERTEX_SHADER) {
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
            param.Descriptor.ShaderRegister = vsRegister++;
        } else {
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
            param.Descriptor.ShaderRegister = psRegister++;
        }

        rootParameters.push_back(param);
	}

    //SRV設定 -======================================================================================

	vsRegister = 0;
	psRegister = 0;

    for (const auto& srvPair : srv) {
        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].NumDescriptors = 1;
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER param{};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

        if (srvPair.second == ShaderType::VERTEX_SHADER) {
            descriptorRange[0].BaseShaderRegister = vsRegister++;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        }
        else {
            descriptorRange[0].BaseShaderRegister = psRegister++;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        }

        param.DescriptorTable.pDescriptorRanges = descriptorRange;
		param.DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

		rootParameters.push_back(param);
    }

    //UAV設定 -======================================================================================

    for (const auto& uavPair : uav) {
        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].NumDescriptors = 1;
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER param{};
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        if (uavPair.second == ShaderType::VERTEX_SHADER) {
            descriptorRange[0].BaseShaderRegister = vsRegister++;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        }
        else {
            descriptorRange[0].BaseShaderRegister = psRegister++;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}

		param.DescriptorTable.pDescriptorRanges = descriptorRange;
		param.DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

		rootParameters.push_back(param);
    }

    //Sampler
    D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
    staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//フィルタリングの方法
    staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//テクスチャのアドレスの方法
    staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	//比較しない
    staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;    //ありったけのMipmapを使う
    staticSampler[0].ShaderRegister = 0;    //レジスタ番号0
    staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    //PixelShaderで使う

    //descにまとめる
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.pParameters = rootParameters.data();
	desc.NumParameters = static_cast<UINT>(rootParameters.size());
	desc.pStaticSamplers = staticSampler;
	desc.NumStaticSamplers = _countof(staticSampler);

    Microsoft::WRL::ComPtr<ID3D10Blob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    //シリアライズしてバイナリにする
    HRESULT hr = D3D12SerializeRootSignature(&desc,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        logger_->error("Error of Creating RootSignature");
        logger_->error(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    //バイナリをもとに生成
    ID3D12RootSignature* rootSignature = nullptr;
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    RootSignatureID id;
    id.id = nextID_++;
    rootSignatures_[id] = rootSignature;
	configToIDMap_[config] = id;

    return id;
}

ID3D12RootSignature* RootSignatureShelf::GetRootSignature(RootSignatureID id) const {
	return rootSignatures_.at(id);
}
