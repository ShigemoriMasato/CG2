#pragma once
#include "Data/BaseResource.h"
#include "Data/PostEffectData.h"
#include <Transform/Transform.h>
#include <Render/OffScreen/OffScreenManager.h>

// 新しいポストエフェクトの追加手順 -================================================================
//
// 1.enumの追加
// 2.shaderの追加(paddingに気を付けること)
// 3.Engine/Render/Resource/Data/PostEffectData.hにGPUに送る構造体の追加
// 4.DrawReadyに分岐を作成し、InfoForGPUに値をセットする処理を追加
// 5.SetJobsで追加したenumをセット
// 
// ==============================================================================================

enum class PostEffectJob : uint32_t {
	None = 0,
	Blur = 1 << 0,
	Grayscale = 1 << 1,
	Fade = 1 << 2,
	GridTransition = 1 << 3,
	SlowMotion = 1 << 4,
	Glitch = 1 << 5,
	ConstantScanline = 1 << 6,  // 常時走査線エフェクト

};

uint32_t operator|(PostEffectJob a, PostEffectJob b);
uint32_t operator|(uint32_t a, PostEffectJob b);
uint32_t operator&(uint32_t a, PostEffectJob b);
uint32_t operator~(PostEffectJob a);

class PostEffectResource : public BaseResource {
public:

	PostEffectResource();
	~PostEffectResource() = default;

	void Initialize();

	void SetJobs(PostEffectJob jobs);
	void SetJobs(uint32_t jobs);

	void DrawReady() override;
	void SwapPS();
	void SimpleDrawReady();
	bool IsContinue() const;
	void DrawFinish() { task_ = jobs_; };

	//描画したいOffScreenのSRVGPUHandle
	ScreenID input_;
	//描画先のOffScreenのSRVGPUHandle
	ScreenID output_;

	Blur blur_;
	Grayscale grayscale_;
	Fade fade_;
	GridTransition gridTransition_;
	SlowMotion slowMotion_;
	Glitch glitch_;
	ConstantScanline constantScanline_;

private:

	VertexData* vertex_ = nullptr;

	//このリソースに登録されているエフェクト
	uint32_t jobs_;
	//描画時、何のエフェクトをかけるかを確認するもの
	uint32_t task_;

	Blur* blurData_ = nullptr;
	Grayscale* grayscaleData_ = nullptr;
	Fade* fadeData_ = nullptr;
	GridTransition* gridTransitionData_ = nullptr;
	SlowMotion* slowMotionData_ = nullptr;
	Glitch* glitchData_ = nullptr;
	ConstantScanline* constantScanlineData_ = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE infoGPUHandle_{};

	const std::string shaderBasePath_ = "PostEffect/";
};
