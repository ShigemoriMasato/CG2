#include "WAVData.h"
#include <cassert>


namespace fs = std::filesystem;

void WAVData::Load(fs::path filepath) {

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filepath, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVRかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	FormatChunk format = {};
	ChunkHeader chunk = {};

	// チャンクを順に読みながらfmtチャンクを探す
	while (file.read((char*)&chunk, sizeof(chunk))) {
		if (strncmp(chunk.id, "fmt ", 4) == 0) {
			// サイズが適正か確認
			assert(chunk.size <= sizeof(format.fmt));
			format.chunk = chunk;
			file.read((char*)&format.fmt, chunk.size);
			break;
		} else {
			// 必要のないチャンクは読み飛ばす
			file.seekg(chunk.size, std::ios_base::cur);
		}
	}

	// fmtチャンクが見つからなければエラー
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));

	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	// Waveファイルを閉じる
	file.close();

	fs::path path = filepath;
	
	// returnする為の音声データ
	wfex_ = format.fmt;
	pBuffer_ = reinterpret_cast<BYTE*>(pBuffer);
	bufferSize_ = data.size;
	name_ = path.string();
	type_ = AudioType::wav;
}

int WAVData::Play(IXAudio2* xAudio, bool isLoop) {

	// 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pPlayResource = nullptr;
	HRESULT hr = xAudio->CreateSourceVoice(&pPlayResource, &wfex_);
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = pBuffer_;
	buf.AudioBytes = bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

	pPlayResource->SetVolume(volume_);

	// 再生リソースをSourceVoiceに登録
	hr = pPlayResource->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));

	// 再生
	hr = pPlayResource->Start();
	assert(SUCCEEDED(hr));

	if (!isLoop) {
		//保存場所の変更
		playResourceIndex_++;
		if (playResourceIndex_ >= resourceNum_) {
			playResourceIndex_ = 0;
		}

		playResource_[playResourceIndex_] = pPlayResource;

		return playResourceIndex_;

	} else {
		loopPlayResource_ = pPlayResource;
		return 9;
	}

}
