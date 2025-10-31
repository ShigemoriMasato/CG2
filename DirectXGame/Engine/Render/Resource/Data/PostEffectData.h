#pragma once
#include <Transform/Vector.h>
#include <string>

//今なんか挙動がちょっとおかしい
struct Blur {
	const std::string shaderFile = "Blur.PS.hlsl";

	float intensity = 0.0f; // ブラー強度
	float kernelSize = 0.0f; // カーネルサイズ
};

struct Grayscale {
	const std::string shaderFile = "Grayscale.PS.hlsl";

	float intensity = 0.0f; // グレースケール強度
};

enum class FadeType : int {
	Black = 0,      // ブラックフェード
	White = 1,// ホワイトフェード
	Radial = 2,     // 放射状フェード
	Wipe = 3,       // ワイプフェード
	Dissolve = 4,   // ディゾルブフェード
};

struct Fade {
	const std::string shaderFile = "Fade.PS.hlsl";
	float alpha = 0.0f;     // フェードの透明度 (0.0 - 1.0)
	FadeType type = FadeType::Black;  // フェードのタイプ
	Vector3 color = { 0.0f, 0.0f, 0.0f };  // フェードカラー（White時は{1,1,1}）
};

struct GridTransition {
	const std::string shaderFile = "GridTransition.PS.hlsl";
	float progress = 0.0f;      // 遷移の進行度 (0.0 - 1.0)
	float gridSize = 16.0f;     // グリッドのサイズ（1辺のタイル数）
	float fadeColor = 0.0f;     // フェード色 (0=黒, 1=白)
	float pattern = 0.0f;       // パターン (0=波紋状, 1=ランダム, 2=左から右, 3=上から下, 4=チェッカーボード)
};

struct SlowMotion {
	const std::string shaderFile = "SlowMotion.PS.hlsl";
	float chromaticAberration = 0.0f;  // 色収差強度 (0.0 - 1.0)
	float vignetteStrength = 0.0f;     // ビネット強度 (0.0 - 1.0)
	float saturation = 1.0f;           // 彩度 (0.0=モノクロ, 1.0=通常)
	float intensity = 0.0f;            // 全体の強度 (0.0 - 1.0)
};

struct Glitch {
	const std::string shaderFile = "Glitch.PS.hlsl";
	float intensity = 0.0f;            // 全体の強度 (0.0 - 1.0)
	float rgbSplit = 0.0f;             // RGB色収差の強度 (0.0 - 1.0)
	float scanlineIntensity = 0.0f;    // スキャンライン強度 (0.0 - 1.0)
	float blockIntensity = 0.0f;       // ブロックノイズ強度 (0.0 - 1.0)
	float time = 0.0f;                 // 時間パラメータ（アニメーション用）
};

struct ConstantScanline {
	const std::string shaderFile = "ConstantScanline.PS.hlsl";
	float intensity = 0.3f;            // 走査線の強度 (0.0 - 1.0)
	float speed = 1.0f;                // 走査線の移動速度
	float lineWidth = 0.002f;          // 走査線の幅
	float time = 0.0f;                 // 時間パラメータ（アニメーション用）
};

struct PostEffectData {
	Blur blur;
	Grayscale grayscale;
	Fade fade;
	GridTransition gridTransition;
	SlowMotion slowMotion;
	Glitch glitch;
	ConstantScanline constantScanline;

};
