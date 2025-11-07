#pragma once
#include <Core/PSO/PSOManager.h>

//現在設定されてるPSOを変更するクラス
class PSOEditor {
public:

	PSOEditor(ID3D12Device* device);
	~PSOEditor() = default;

	void Initialize();

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


