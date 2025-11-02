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

	//Audioの削除
	audioData_.clear();

	// MediaFoundationの初期化
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

	//_ XAudioエンジンのインスタンスを生成
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	// マスターボイスを生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
}

void AudioManager::Load(fs::path filePath, uint32_t id) {
	//拡張子を取り出す
	std::string extension = filePath.extension().string();

	//拡張子によって作成する型を変更する
	if (extension == ".wav") {
		audioData_[id] = std::make_unique<WAVData>();
	} else if (extension == ".mp3") {
		audioData_[id] = std::make_unique<MP3Data>();
	}

	//読み込み
	audioData_[id]->Load(filePath.string());
}

uint32_t AudioManager::Play(uint32_t handle, bool isLoop) {
	if(handle < 0 || handle >= audioData_.size()) {
		return 0xffffffff;
	}

	uint32_t playHandle = audioData_[handle]->Play(xAudio2_.Get(), isLoop);
	return handle * 10 + playHandle;
}

void AudioManager::SetVolume(uint32_t soundHandle, float volume) {
	if (soundHandle < 0 || soundHandle >= audioData_.size()) {
		return;
	}

	audioData_[soundHandle]->SetVolume(volume);
}

bool AudioManager::IsPlay(uint32_t playHandle) {
	uint32_t soundHandle = playHandle / 10;
	uint32_t handle = playHandle % 10;
	if (soundHandle < 0 || soundHandle >= audioData_.size()) {
		return false;
	}

	return audioData_[soundHandle]->IsPlay(handle);
}

void AudioManager::Stop(uint32_t playHandle) {
	uint32_t soundHandle = playHandle / 10;
	uint32_t handle = playHandle % 10;

	if (soundHandle < 0 || soundHandle >= audioData_.size()) {
		return;
	}

	audioData_[soundHandle]->Stop(handle);
}

AudioData* AudioManager::GetAudioData(uint32_t id) {
	if (audioData_.find(id) != audioData_.end()) {
		return audioData_[id].get();
	}
	return nullptr;
}