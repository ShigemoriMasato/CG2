#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <vector>
#include <Tools/Binary/BinaryManager.h>
#include <Core/PSO/PSOConfig.h>

//あらゆるPSOを管理するクラス
class PSOManager {
public:

	PSOManager(ID3D12Device* device, Logger* logger);
	~PSOManager();

	void Initialize();
	
	ID3D12PipelineState* GetPSO(const PSOConfig& config);

	ID3D12RootSignature* GetRootSignature(const RootSignatureID id) const;

	ShaderShelf* GetShaderShelf() const { return shaderShelf_.get(); }

private:

	/// <summary>
	/// ShaderDataが破損した時用
	/// </summary>
	void CreateAllPSO();

	ID3D12Device* device_ = nullptr;

	std::unordered_map<PSOConfig, ID3D12PipelineState*> psoMap_;

	std::unique_ptr<ShaderShelf> shaderShelf_{};
	std::unique_ptr<BlendStateShelf> blendStateShelf_{};
	std::unique_ptr<DepthStencilShelf> depthStencilShelf_{};
	std::unique_ptr<RasterizerShelf> rasterizerShelf_{};
	std::unique_ptr<RootSignatureShelf> rootSignatureShelf_{};
	std::unique_ptr<InputLayoutShelf> inputLayoutShelf_{};

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	const std::string shaderDataFile = "ShaderEditData.bin";
	std::vector<ShaderData> shaderData_{};

	Logger* logger_ = nullptr;
};
