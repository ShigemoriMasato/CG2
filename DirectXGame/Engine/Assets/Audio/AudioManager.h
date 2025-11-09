#pragma once
#include "Data/AudioData.h"
#include <memory>
#include <wrl.h>
#include <map>

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
	void Load(std::filesystem::path filepath, uint32_t id);
	/// <summary>
	/// 音の再生
	/// </summary>
	/// <param name="handle">Loadで取得した音のハンドル</param>
	/// <returns>再生ハンドル。流し終わるかStopを押すと破棄される</returns>
	uint32_t Play(uint32_t soundHandle, bool isLoop);

	//Volumeを変更(初期値1.0f)
	void SetVolume(uint32_t soundHandle, float volume);

	//再生されているかどうか
	bool IsPlay(uint32_t playHandle);
	//再生停止。もう一度再生する場合は最初からになり、handleは破棄される
	void Stop(uint32_t playHandle);

	AudioData* GetAudioData(uint32_t id);

private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	std::map<uint32_t, std::unique_ptr<AudioData>> audioData_{};

	const std::string basePath = "Assets/Audio/";
};

