#include "AudioData.h"

AudioData::~AudioData() {
	for(const auto& resource : playResource_) {
		if (resource) {
			delete pBuffer_;
			pBuffer_ = nullptr;
			bufferSize_ = 0;
			wfex_ = {};
		}
	}
}

void AudioData::SetVolume(float volume) {
	volume_ = volume;
}

bool AudioData::IsPlay(int handle) {
	XAUDIO2_VOICE_STATE state{};
	IXAudio2SourceVoice* pSourceVoice = nullptr;

	if (handle == 9) {
		pSourceVoice = loopPlayResource_;
	} else {
		// resourceNum_ = 8 なので、範囲外アクセス防止
		if (handle < 0 || handle >= resourceNum_) {
			return false;
		}
		pSourceVoice = playResource_[handle];
	}

	if (!pSourceVoice) {
		return false;
	}

	pSourceVoice->GetState(&state);
	return state.BuffersQueued != 0;
}

void AudioData::Stop(int handle) {
	if (!IsPlay(handle)) {
		return;
	}

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	if (handle == 9) {
		pSourceVoice = loopPlayResource_;
	} else {
		pSourceVoice = playResource_[handle];
	}

	pSourceVoice->Stop(0);
	pSourceVoice->FlushSourceBuffers();
	pSourceVoice->DestroyVoice();

}
