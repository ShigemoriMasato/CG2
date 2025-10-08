#pragma once
#include <Render/Resource/ParticleResource.h>
#include <Render/Render.h>
#include <random>
#include <cstdint>
#include <Tools/FPS/FPSObserver.h>
#include <Tools/Binary/BinaryManager.h>

enum class ParticleJob : uint32_t {
	ToTransparent = 1 << 0,		//だんだん透明になっていく(死ぬときに完全に透明になるようになる)
	Scatter = 1 << 1,			//四方に散らばる
	Gravity = 1 << 2,			//重力がかかる
	ToRotate = 1 << 3,			//回転する
};

/*
ParticleJobとValueの早見表

ParticleJob		Value(Vector3)				Value2(Vector3)
---------------------------------------------------------------------------------------------------------------------
ToTransparent	使用しない						使用しない
Scatter			速度の最大値					速度の最小値
Gravity			加速度						使用しない
ToRotate		回転速度						なんか入れる->50%でランダムに反転　何も入れない->反転しない

*/

uint32_t operator|(ParticleJob a, ParticleJob b);

uint32_t operator|(uint32_t a, ParticleJob b);

uint32_t operator&(uint32_t a, ParticleJob b);

class DefaultEmitter {
public:

	DefaultEmitter(int maxNum);
	~DefaultEmitter();

	//初期化
	void Initialize(Camera* camera);
	//更新
	void Update();
	//描画
	void Draw(Render* render);

	void DrawImGui();
	
	//パーティクルの寿命を設定
	void SetLifeTime(float time);
	/// <summary>
	/// パーティクルの動作の設定
	/// </summary>
	/// <param name="jobs">ParticleJobをORで組み合わせて使用</param>
	void SetJobs(uint32_t jobs);
	//パーティクルの動作の設定(一個しかない場合、型変換が行われないのでその対策関数)
	void SetJobs(ParticleJob jobs);
	//エミッタの位置を設定
	void SetPosition(Vector3 pos);

	/// <summary>
	/// パーティクルの動作に関する設定を行う
	/// </summary>
	/// <param name="jobs">どの動作か</param>
	/// <param name="value">動作に対応した値を挿入(詳しくはEmitter.hを参照)</param>
	void SetConfig(ParticleJob jobs, Vector3 value, Vector3 value2);

	void SetActive(bool active) { isActive_ = active; }

	void SetGenerateNum(int num) { generateNum_ = num; }

	// 設定を保存する際に使う名前の定義
	void SetName(const std::string& name);

	void Save();
	void Load();

private:

	void SetParticleJobs(ParticleJob job, bool isUse);

	const int maxNum_;

	//Emitterに登録されているパーティクルの寿命
	float lifeTime_ = 2.0f;

	std::mt19937 randEngine_;

	std::unique_ptr<ParticleResource> info_ = nullptr;

	int particleIndex_ = 0;

	//パーティクルの動き
	std::vector<Vector3> velocity_{};
	std::vector<Vector3> acceleration_{};
	std::vector<Vector3> rotation_{};
	
	//パーティクル一個ごとの生存時間
	std::vector<float> life_{};
	//パーティクル一個ごとの寿命
	std::vector<float> lifeSpan_{};

	//Emitterに登録されているパーティクルの動作
	uint32_t emitterJobs_ = 0;
	//パーティクル一個ずつに登録されているパーティクルの動作
	std::vector<uint32_t> particleJobs_{};

	//発生場所
	Vector3 position_{ 0.0f, 0.0f, 0.0f };

	//動作に関するパラメータ
	Vector3 scatterMax_{ 1.0f, 1.0f, 1.0f };
	Vector3 scatterMin_{ -1.0f, -1.0f, -1.0f };
	Vector3 gravity_{ 0.0f, -9.8f, 0.0f };
	Vector3 rotateSpeed_{ 0.0f, 0.0f, 0.0f };
	bool isRotateInverse_ = false;

	float timer_;
	float coolTime_ = 0.1f;

	bool isActive_ = true;

	//一度に生成するパーティクルの数
	int generateNum_ = 1;

	//Emitterの状態保存
	std::string name_ = "Default";


	//ファイル保存
	static std::unique_ptr<BinaryManager> binaryManager_;
	static const inline std::string saveFileFolder = "EmitterConfig/";

	//ImGui用変数
	static int idCounter_;
	int id_ = 0;
	char nameBuf[128] = "Default";
	int particleNum_ = 0;
};
