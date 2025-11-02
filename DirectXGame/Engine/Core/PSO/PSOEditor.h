#pragma once
#include <Core/PSO/PSOManager.h>

//現在設定されてるPSOを変更するクラス
class PSOEditor {
public:

	PSOEditor(ID3D12Device* device);
	~PSOEditor() = default;

	void Initialize();

	void SetPixcelShader(const std::string& name);

	void SetVertexShader(const std::string& name);

	void SetBlendState(BlendStateID blendID);

	void SetDepthStencilState(DepthStencilID depthStencilID);

	void SetRasterizerState(RasterizerID rasterizerID);

	void SetRootSignature(RootSignatureID rootID);

	void SetInputLayout(InputLayoutID inputLayoutID);

	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);

	void SetOffScreen(bool isOffScreen);

	void SetPSOConfig(const PSOConfig& config) { nextConfig_ = config; }

	PSOManager* GetPSOManager() { return psoManager_.get(); }

	/// <summary>
	/// 設定した内容でPSOを設定し、内部の設定をデフォルトに戻す。(デフォルトはEngine/Core/PSO/PSOConfigへ)
	/// </summary>
	/// <param name="commandList"></param>
	void Setting(ID3D12GraphicsCommandList* commandList);

	void FrameInitialize(ID3D12GraphicsCommandList* commandList);

private:

	std::unique_ptr<PSOManager> psoManager_;
	
	PSOConfig nextConfig_{};
	PSOConfig nowConfig_{};

	bool isFirst_ = true;

};


