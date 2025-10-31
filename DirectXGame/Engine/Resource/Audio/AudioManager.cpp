#include "AudioManager.h"
#include "Data/MP3Data.h"
#include "Data/WAVData.h"

#include <cassert>

#pragma comment(lib,"xaudio2.lib")

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace fs = std::filesystem;

AudioManager::~AudioManager() {
	xAudio2_.Reset();
	MFShutdown();
}

void AudioManager::Initialize() {

	// MediaFoundationの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	// XAudioエンジンのインスタンスを生成
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	// マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
}

int AudioManager::Load(fs::path filePath) {
	for (int i = 0; i < audioNames_.size(); ++i) {
		if(audioNames_[i] == filePath.string()) {
			return i;
		}
	}

	//拡張子を取り出す
	std::string extension = filePath.extension().string();

	//拡張子によって作成する型を変更する
	if (extension == ".wav") {
		audioData_.emplace_back(std::make_unique<WAVData>());
	} else if (extension == ".mp3") {
		audioData_.emplace_back(std::make_unique<MP3Data>());
	}

	//読み込み
	audioData_.back()->Load(basePath + filePath.string());
	audioNames_.emplace_back(filePath.string());

	return static_cast<int>(audioData_.size() - 1);
}

int AudioManager::Play(int handle, bool isLoop) {
	if(handle < 0 || handle >= audioData_.size()) {
		return -1;
	}

	int playHandle = audioData_[handle]->Play(xAudio2_.Get(), isLoop);
	return handle * 10 + playHandle;
}

void AudioManager::SetVolume(int soundHandle, float volume) {
	if (soundHandle < 0 || soundHandle >= audioData_.size()) {
		return;
	}

	audioData_[soundHandle]->SetVolume(volume);
}

bool AudioManager::IsPlay(int playHandle) {
	int soundHandle = playHandle / 10;
	int handle = playHandle % 10;
	if (soundHandle < 0 || soundHandle >= audioData_.size()) {
		return false;
	}

	return audioData_[soundHandle]->IsPlay(handle);
}

void AudioManager::Stop(int playHandle) {
	int soundHandle = playHandle / 10;
	int handle = playHandle % 10;

	if (soundHandle < 0 || soundHandle >= audioData_.size()) {
		return;
	}

	audioData_[soundHandle]->Stop(handle);
}
