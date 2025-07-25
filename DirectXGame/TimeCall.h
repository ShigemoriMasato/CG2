#pragma once
#include "TimeCallData.h"
#include <vector>

class TimeCall {
public:

	TimeCall();
	~TimeCall() = default;

	void Initialize();
	void Update();

	/// <summary>
	/// 関数の設定
	/// </summary>
	/// <param name="">実行する関数</param>
	/// <param name="afterFrame">今から何フレーム後に実行するか</param>
	int Register(std::function<void()> func, int afterFrame = 0, bool repete = false);

	void Clear();

	void Delete(int id);

private:

	std::vector<TimeCallData> timeCallData_;

	int frame_;
	int id = 0;

};
