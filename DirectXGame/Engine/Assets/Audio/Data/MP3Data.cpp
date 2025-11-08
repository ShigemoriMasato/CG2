#include "MP3Data.h"
#include <cassert>
#include <Func/MyString.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

namespace fs = std::filesystem;

void MP3Data::Load(std::filesystem::path filepath) {
	// 音声データを登録
	type_ = AudioType::mp3;
	name_ = filepath.string();

	std::wstring path = ConvertString(filepath.string());

	// ソースリーダーの作成
	IMFSourceReader* pMFSourceReader{ nullptr };
	HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), NULL, &pMFSourceReader);
	assert(SUCCEEDED(hr));

	// メディアタイプを取得
	IMFMediaType* pMFMediaType{ nullptr };
	hr = MFCreateMediaType(&pMFMediaType);
	assert(SUCCEEDED(hr));
	
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;

	pMFSourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType);

	WAVEFORMATEX* waveFormat{ nullptr };
	hr = MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);
	assert(SUCCEEDED(hr));

	// データ読み込み
	std::vector<BYTE> bufferData;
	while (true) {
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags = 0;
		pMFSourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer = nullptr;
		DWORD cbCurrentLength = 0;
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);
		// データを一時バッファに追加
		bufferData.insert(bufferData.end(), pBuffer, pBuffer + cbCurrentLength);

		// 解放処理
		pMFMediaBuffer->Unlock();
		pMFMediaBuffer->Release();
		pMFSample->Release();

	}

	// 読み込んだデータをメモリにコピー
	BYTE* pAudioData = new BYTE[bufferData.size()];
	memcpy(pAudioData, bufferData.data(), bufferData.size());

	wfex_ = *waveFormat;
	pBuffer_ = pAudioData;
	bufferSize_ = static_cast<UINT32>(bufferData.size());

	// 解放処理
	CoTaskMemFree(waveFormat);
	pMFMediaType->Release();
	pMFSourceReader->Release();
}

int MP3Data::Play(IXAudio2* xAudio, bool isLoop) {
	IXAudio2SourceVoice* pSourceVoice{ nullptr };
	xAudio->CreateSourceVoice(&pSourceVoice, &wfex_);
	pSourceVoice->SetVolume(volume_);

	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = pBuffer_;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sizeof(BYTE) * static_cast<UINT32>(bufferSize_);
	buffer.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

	// 再生する
	HRESULT hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hr));


	hr = pSourceVoice->Start();
	assert(SUCCEEDED(hr));

	// 管理テーブルに登録
	if (!isLoop) {
		//保存場所の変更
		playResourceIndex_++;
		if (playResourceIndex_ >= resourceNum_) {
			playResourceIndex_ = 0;
		}

		playResource_[playResourceIndex_] = pSourceVoice;

		return playResourceIndex_;

	} else {
		loopPlayResource_ = pSourceVoice;
		return 9;
	}

}
