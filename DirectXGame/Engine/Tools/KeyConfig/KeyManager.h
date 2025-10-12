#pragma once
#include <Input/Input.h>
#include <unordered_map>
#include <Tools/Binary/BinaryManager.h>

enum class Key {
	Right,			//右に行くキー。Dとか→とか
	Left,			//左に行くキー。Aとか←とか
	Up,				//エイム専用。Wとか↑とか
	Down,			//エイム専用。Sとか↓とか
	Action,			//アクションキー。Jumpとかワイヤー確定とか。スペース
	Correct,		//確定キー。UI用。Enter,昔の人用にZとか入ってるといいかも
	Reverse,		//キャンセルキー。UI用。Esc,昔の人用にXとか入ってるといいかも

	Count
};

enum class KeyState {
	None,
	Trigger,
	Hold,
	Release,

	Count
};

//キーコンフィグの設定画面の作成は考慮してないです。よって削除はできません.
class KeyManager {
public:

	KeyManager() = default;
	~KeyManager() = default;

	void Initialize();
	void Update();

	std::unordered_map<Key, bool> GetKeyStates() const { return resultKeyFlugs_; }

	void SetKey(Key action, uint8_t DIK, KeyState state = KeyState::Release);
	void SetButton(Key action, XBoxController button, KeyState state);
	void SetStick(Key action, bool isLeft, bool isY, float toggleValue);

private:

	enum Direction {
		Right,
		Left
	};

	//Keyに割り当てられているDIKと状態
	std::unordered_map<Key, std::vector<std::pair<uint8_t, KeyState>>> keyMap_;
	std::unordered_map<Key, std::vector<std::pair<XBoxController, KeyState>>> buttonMap_;
	std::unordered_map<Key, std::pair<Direction, std::pair<bool, float>>> stickMap_;

	//キー入力の履歴を保存する。
	std::vector<std::unordered_map<uint8_t, bool>> keyHistory_;
	std::vector<std::unordered_map<XBoxController, bool>> buttonHistory_;
	std::vector<std::unordered_map<Direction, Vector2>> stickHistory_;
	//履歴の最大数
	static const int kMaxHistory_ = 10;

	//最終的な状態を格納する
	std::unordered_map<Key, bool> resultKeyFlugs_;
};
