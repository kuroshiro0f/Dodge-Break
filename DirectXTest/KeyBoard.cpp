#include "KeyBoard.h"

KeyBoard::KeyBoard()
{
    for (int i = 0; i < KeyBindMax; i++)
    {
        m_keyInfoState.keyBind[i] = -1;
    }
    m_keyInfoState.keyState = 0;
    m_keyInfoState.keyStateOld = 0;

    //  キーバインドの設定
    BindKey(KeyBind::Select, 0x01);      //  決定(左クリック)

    //  ゲームシーン
    BindKey(KeyBind::ChangeTitle, 0x01);        //  タイトルへ（左クリック）
    BindKey(KeyBind::ChangeGameScene, 0x02);    //  ゲームシーンへ（右クリック）

#if _DEBUG
    BindKey(KeyBind::Pause, 0x20);      //  停止(スペースキー)
    BindKey(KeyBind::Restart, 0x0D);    //  再開(Enterキー)

    BindKey(KeyBind::RestartNextPoint, 0x27);       //  次のチェックポイント(→キー)
    BindKey(KeyBind::RestartPreviousPoint, 0x25);   //  前のチェックポイント(←キー)

    BindKey(KeyBind::ReLoadFile, 0x30);             //  外部ファイルの再読み込み(0キー)
#endif

    BindKey(KeyBind::End,   0x1B);       //  ゲーム終了(ESCキー)
}

KeyBoard::~KeyBoard()
{
    //  処理なし
}

//  キーの状態を取得
bool KeyBoard::GetKeyStatus(const int _keyCord)
{
    bool isKeyPress;
    if (GetKeyState(_keyCord))
    {
        isKeyPress = true;
    }
    else
    {
        isKeyPress = false;
    }
    return isKeyPress;
}

//  指定キーが押されて続けているか
bool KeyBoard::IsOnKey(const KeyBind _key)
{
    int bit = (1 << _key);
    //  単純に現在のステートから判定
    int state = m_keyInfoState.keyState;
    return (state & bit)? true: false;
}

//  指定キーが今押されたか
bool KeyBoard::IsPressKey(const KeyBind _key)
{
    int bit = (1 << _key);
    //  今回と前回とのステートの差(1 & ~0)で判定
    int state = m_keyInfoState.keyState & ~m_keyInfoState.keyStateOld;
    return (state & bit) ? true : false;
}

//  指定キーが離されたか
bool KeyBoard::IsReleaseKey(const KeyBind _key)
{
    int bit = (1 << _key);
    //  前回と今回とのステートの差(1 & ~0)で判定
    int state = m_keyInfoState.keyStateOld & ~m_keyInfoState.keyState;
    return (state & bit) ? true : false;
}

//  キーの状態の更新
void KeyBoard::UpdateInput()
{
    BYTE keyboardState[256];

    //  キーボード状態の取得
    GetKeyboardState(keyboardState);

    int keyState = 0;
    for (int i = 0; i < KeyBindMax; i++)
    {
        //  キーがバインドされていなければスルーする
        int key = m_keyInfoState.keyBind[i];
        if (key == -1)continue;

        //  入力されている場合はキーのビットを立てる
        if (keyboardState[key] & 0x80)
        {
            keyState |= (1 << i);
        }
    }

    //  ステートの更新
    m_keyInfoState.keyStateOld = m_keyInfoState.keyState;
    m_keyInfoState.keyState = keyState;
}

//  キーコードをバインド
void KeyBoard::BindKey(const KeyBind _key, const int _keyCode)
{
    m_keyInfoState.keyBind[_key] = _keyCode;
}