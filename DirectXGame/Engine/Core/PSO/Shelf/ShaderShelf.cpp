#include "ShaderShelf.h"
#include <cassert>
#include <Function/MyString.h>
#include <Core/DXCommonFunction.h>

#pragma comment(lib, "dxcompiler.lib")

namespace fs = std::filesystem;
using namespace Logger;

ShaderShelf::ShaderShelf() {

    //dxcCompilerを初期化
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    //includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

	compileVersions_[0] = L"vs_6_0"; // Vertex Shader
    compileVersions_[1] = L"ps_6_0"; // Pixel Shader

	logger_ = getLogger("ShaderShelf");
}

ShaderShelf::~ShaderShelf() {
    
}

void ShaderShelf::CompileAllShader() {

	std::vector<std::wstring> shaderNames;

    //shaderNameBufferを削除するためのスコープ
    {
        std::vector<std::string> shaderNameBuffer = SearchFiles(basePath_, ".hlsl");
        for (const auto& name : shaderNameBuffer) {
            shaderNames.push_back(ConvertString(name));
        }
    }

    for (const auto& sn : shaderNames) {
        if (sn.find(L".hlsli") != std::string::npos) {
            continue;
        }
        //VertexShaderだったら
        else if (sn.find(L"VS") != std::string::npos) {
			RegistShaderByteCode(sn, ShaderType::VERTEX_SHADER);
        }
		//PixelShaderだったら
        else if (sn.find(L"PS") != std::string::npos) {
			RegistShaderByteCode(sn, ShaderType::PIXEL_SHADER);
        } 
        //これ以降も同じように追加する


		// 登録してない、または名前が間違っているShaderは見つかり次第エラーを出す
        else {
			throw std::runtime_error("Unknown shader type: " + ConvertString(sn));
        }
    }
}

std::list<D3D12_SHADER_BYTECODE> ShaderShelf::GetShaderBytecodes(ShaderType shaderType) {
    std::list<D3D12_SHADER_BYTECODE> ans;

    for (const auto& [name, bytecode] : shaderBytecodes_[static_cast<size_t>(shaderType)]) {
        ans.push_back(bytecode);
    }

	return ans;
}

std::list<std::string> ShaderShelf::GetShaderNames(ShaderType shaderType) {
    std::list<std::string> ans;

    for (const auto& [name, bytecode] : shaderBytecodes_[static_cast<size_t>(shaderType)]) {
        ans.push_back(name);
    }

    return ans;
}

D3D12_SHADER_BYTECODE ShaderShelf::GetShaderBytecode(ShaderType shaderType, std::string shaderName) {
    //登録されていなければCompileする
    if (shaderBytecodes_[static_cast<int>(shaderType)][shaderName].pShaderBytecode == D3D12_SHADER_BYTECODE().pShaderBytecode) {
		RegistShaderByteCode(ConvertString(shaderName), shaderType);
    }
	return shaderBytecodes_[static_cast<int>(shaderType)][shaderName];
}

void ShaderShelf::RegistShaderByteCode(std::wstring shaderName, ShaderType shaderType) {
    IDxcBlob* blobBuffer = CompileShader(ConvertString(basePath_.string()) + L"/" + shaderName, compileVersions_[int(shaderType)].c_str(),
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get(), logger_.get());

    shaderBlobs_.push_back(blobBuffer);

    shaderBytecodes_[int(shaderType)][ConvertString(shaderName)] = { blobBuffer->GetBufferPointer(), blobBuffer->GetBufferSize() };
}
