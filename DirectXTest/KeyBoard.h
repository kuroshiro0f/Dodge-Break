#pragma once
#include <Windows.h>

//  キーの入力判定を行うクラス
//  シングルトンで扱う
class KeyBoard
{
public:
    //  キーコードのバインド先
    enum KeyBind
    {
        Select,      //  決定

        //  ゲームシーン
        ChangeTitle,        //  タイトルへ
        ChangeGameScene,    //  ゲームシーンへ

#if _DEBUG
        Pause,      //  一時停止
        Restart,    //  再開
       
        //  再開するチェックポイントの指定
        RestartNextPoint,       //  次
        RestartPreviousPoint,   //  前

        //  外部ファイルの再読み込み
        ReLoadFile,
#endif

        End,        //  ゲーム終了

        KeyBindMax
    };

    KeyBoard();
    ~KeyBoard();

    //  指定キーの状態を判定
    bool IsOnKey(const KeyBind _key);         //  押され続けているか
    bool IsPressKey(const KeyBind _key);      //  今押されたか
    bool IsReleaseKey(const KeyBind _key);    //  離されたか

    //  キー入力の更新
    void UpdateInput();
private:
    //  キーの状態を得る
    bool GetKeyStatus(const int _keyCode);

    //  キーコードをバインド
    void BindKey(const KeyBind _key, const int _keyCode);

    //  キーの情報
    struct KeyInfo
    {
        int keyBind[KeyBindMax];   //  キーバインド情報
        int keyState;              //  キーステート
        int keyStateOld;           //  前回のキーステート
    };
    KeyInfo m_keyInfoState;
};