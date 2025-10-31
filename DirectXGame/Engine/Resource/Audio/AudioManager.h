#pragma once
#include "Data/AudioData.h"
#include <memory>
#include <wrl.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

class AudioManager {
public:

	AudioManager() = default;
	~AudioManager();

	void Initialize();

	/// <summary>
	/// 音声の読み込み(mp3, wav)
	/// </summary>
	/// <param name="filepath">ファイルパス(Assets/Audio/は記述する必要なし)</param>
	/// <returns></returns>
	int Load(std::filesystem::path filepath);
	/// <summary>
	/// 音の再生
	/// </summary>
	/// <param name="handle">Loadで取得した音のハンドル</param>
	/// <returns>再生ハンドル。流し終わるかStopを押すと破棄される</returns>
	int Play(int soundHandle, bool isLoop);

	//Volumeを変更(初期値1.0f)
	void SetVolume(int soundHandle, float volume);

	//再生されているかどうか
	bool IsPlay(int playHandle);
	//再生停止。もう一度再生する場合は最初からになり、handleは破棄される
	void Stop(int playHandle);

private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	const std::string basePath = "Assets/Audio/";

	std::vector<std::unique_ptr<AudioData>> audioData_{};
	std::vector<std::string> audioNames_{};

};

