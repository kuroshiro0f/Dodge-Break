#include "UserInputHandler.h"

//  入力の強さの最大値
const float INPUT_STRENGTH_MAX = 1.0f;
//  入力の強さの最小値
const float INPUT_STRENGTH_MIN = -1.0f;

UserInputHandler::UserInputHandler()
    :m_controlLeftStick(nullptr)
    , m_pressAButton(nullptr)
    , m_pressStartButton(nullptr)
    , m_pressWASDKey(nullptr)
    , m_pressEnterKey(nullptr)
    , m_pressEscapeKey(nullptr)
    , m_gamePad(nullptr)
    , m_keyboard(nullptr)
{
    //  処理なし
}

UserInputHandler::~UserInputHandler()
{
    //  処理なし
}

//  初期化
void UserInputHandler::Init()
{
    //  各デバイスのインスタンスを得る
    m_gamePad = std::make_unique<GamePad>();
    m_keyboard = std::make_unique<Keyboard>();
}

//  命令を行う関数ポインタのリセット
//  NOTE: シーン更新時にリセットし、前のシーンの関数が呼ばれないようにする。
//  NOTE: ゲーム終了処理はゲームを通して共通のため、リセットを行わない。
void UserInputHandler::ResetOperationFunc()
{
    //  ゲームパッドの入力時の命令をリセット
    m_controlLeftStick = nullptr;
    m_pressAButton = nullptr;
    m_pressBButton = nullptr;
#if _DEBUG
    m_pressLButton = nullptr;
    m_pressRButton = nullptr;
    m_pressDpadLeftButton = nullptr;
    m_pressDpadRightButton = nullptr;
    m_pressDpadUpButton = nullptr;
#endif

    //  キーボードの入力時の命令をリセット
    m_pressWASDKey = nullptr;
    m_pressEnterKey = nullptr;
    m_pressSpaceKey = nullptr;
#if _DEBUG
    m_pressPKey = nullptr;
    m_pressLKey = nullptr;
    m_pressLeftKey = nullptr;
    m_pressRightKey = nullptr;
    m_pressZeroKey = nullptr;
#endif
}

//  入力の更新
void UserInputHandler::UpdateInput()
{
    //  キー入力されたか
    bool isInputKey = false;

    //  キー入力を判定
    isInputKey = HandleInputForKeyboard();

    //  キー入力がされていなければゲームパッドの操作を判定する
    if (!isInputKey)
    {
        HandleInputForGamePad();
    }
}

//  命令の登録
void UserInputHandler::RegisterOperation(const OperationType _type, const std::function<void()> _operationFunc)
{
    //  指定されたタイプごとに関数を設定
    switch (_type)
    {
    case OperationType::MovePlayer:
        m_controlLeftStick = _operationFunc;
        m_pressWASDKey = _operationFunc;
        break;
    case OperationType::ToNext:
        m_pressAButton = _operationFunc;
        m_pressEnterKey = _operationFunc;
        break;
    case OperationType::Retry:
        m_pressBButton = _operationFunc;
        m_pressSpaceKey = _operationFunc;
        break;
#if _DEBUG
    case OperationType::Pause:
        m_pressLButton = _operationFunc;
        m_pressPKey = _operationFunc;
        break;
    case OperationType::Restart:
        m_pressRButton = _operationFunc;
        m_pressLKey = _operationFunc;
        break;
    case OperationType::RestartNextPoint:
        m_pressDpadLeftButton = _operationFunc;
        m_pressLeftKey = _operationFunc;
        break;
    case OperationType::RestartPreviousPoint:
        m_pressDpadRightButton = _operationFunc;
        m_pressRightKey = _operationFunc;
        break;
    case OperationType::ReLoadFile:
        m_pressDpadUpButton = _operationFunc;
        m_pressZeroKey = _operationFunc;
        break;
#endif
    case OperationType::FinishGame:
        m_pressStartButton = _operationFunc;
        m_pressEscapeKey = _operationFunc;
        break;
    default:
        break;
    }
}

//  ゲームパッドの入力を検知
void UserInputHandler::HandleInputForGamePad()
{
    //  ゲームパッドの状態を取得する変数を定義
    //  NOTE:: GetStateの引数には0から始まるコントローラーの番号を指定。
    //         今回は1人プレイなので0のみを指定
    auto gamePadState = m_gamePad->GetState(0, GamePad::DEAD_ZONE_CIRCULAR);
    GamePad::ButtonStateTracker stateTracker;
    //  ゲームパッドの入力状態を更新
    stateTracker.Update(gamePadState);

    //  左スティックを動かしたときの操作
    if (m_controlLeftStick && (gamePadState.IsLeftThumbStickUp() || gamePadState.IsLeftThumbStickDown() || gamePadState.IsLeftThumbStickLeft() || gamePadState.IsLeftThumbStickRight()))
    {
        m_controlLeftStick();
    }

    //  Aボタン押し込み時の操作
    if (m_pressAButton && stateTracker.a == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressAButton();
    }

    //  Bボタン押し込み時の操作
    if (m_pressBButton && stateTracker.b == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressBButton();
    }

#if _DEBUG
    //  Lボタン押し込み時の操作
    if (m_pressLButton && stateTracker.leftShoulder == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressLButton();
    }

    //  Rボタン押し込み時の操作
    if (m_pressRButton && stateTracker.rightShoulder == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressRButton();
    }

    //  Dpad左押し込み時の操作
    if (m_pressDpadLeftButton && stateTracker.dpadLeft == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressDpadLeftButton();
    }

    //  Dpad右押し込み時の操作
    if (m_pressDpadRightButton && stateTracker.dpadRight == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressDpadRightButton();
    }

    //  Dpad上押し込み時の操作
    if (m_pressDpadUpButton && stateTracker.dpadUp == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressDpadUpButton();
    }
#endif

    //  Startボタン押し込み時の操作
    if (m_pressStartButton && stateTracker.start == GamePad::ButtonStateTracker::PRESSED)
    {
        m_pressStartButton();
    }
}

//  キーボードの入力を検知
//  NOTE: キー入力が実行された際に、ゲームパッドの入力の検知を飛ばすため、
//        bool型にして、内部で処理が行われたかを返す。
bool UserInputHandler::HandleInputForKeyboard()
{
    //  キーボードの状態を取得する変数を定義
    auto keyboardState = m_keyboard->GetState();
    Keyboard::KeyboardStateTracker stateTracker;
    //  キー入力の状態を更新
    stateTracker.Update(keyboardState);

    //  キー入力されたか
    bool isInputKey = false;

    //  WASDキー押し込み時の操作
    if (m_pressWASDKey && (keyboardState.W || keyboardState.A || keyboardState.S || keyboardState.D))
    {
        m_pressWASDKey();
        isInputKey = true;
    }

    //  Enterキー押し込み時の操作
    if (m_pressEnterKey && stateTracker.pressed.Enter)
    {
        m_pressEnterKey();
        isInputKey = true;
    }

    //  Spaceキー押し込み時の操作
    if (m_pressSpaceKey && stateTracker.pressed.Space)
    {
        m_pressSpaceKey();
        isInputKey = true;
    }

#if _DEBUG
    //  Pキー押し込み時の操作
    if (m_pressPKey && stateTracker.pressed.P)
    {
        m_pressPKey();
        isInputKey = true;
    }
    //  Lキー押し込み時の操作
    if (m_pressLKey && stateTracker.pressed.L)
    {
        m_pressLKey();
        isInputKey = true;
    }
    //  ←キー押し込み時の操作
    if (m_pressLeftKey && stateTracker.pressed.Left)
    {
        m_pressLeftKey();
        isInputKey = true;
    }
    //  →キー押し込み時の操作
    if (m_pressRightKey && stateTracker.pressed.Right)
    {
        m_pressRightKey();
        isInputKey = true;
    }
    //  0キー押し込み時の操作
    if (m_pressZeroKey && (stateTracker.pressed.NumPad0 || keyboardState.D0))
    {
        m_pressZeroKey();
        isInputKey = true;
    }
#endif

    //  Escapeキー押し込み時の操作
    if (m_pressEscapeKey && stateTracker.pressed.Escape)
    {
        m_pressEscapeKey();
        isInputKey = true;
    }

    return isInputKey;
}

//  横方向の入力の強さを取得
float UserInputHandler::GetInputStrengthX()
{
    //  コントローラーの状態を取得する変数を定義
    //  NOTE:: GetStateの引数には0から始まるコントローラーの番号を指定。
    //         今回は1人プレイなので0のみを指定
    auto gamePadState = m_gamePad->GetState(0, GamePad::DEAD_ZONE_CIRCULAR);
    //  キーボードの状態を取得する変数を定義
    auto keyboardState = m_keyboard->GetState();
    //  使いまわす値を変数に格納
    bool key_A = keyboardState.A;
    bool key_D = keyboardState.D;

    //  返す値
    float retValue = 0;

    //  ゲームパッドでの入力の強さを得る
    //  NOTE: -1.0f ～ 1.0fに正規化されている値が返されます
    retValue = gamePadState.thumbSticks.leftX;

    //  キーボード押し込み時は固定の値にする
    //  NOTE: AとDが同時に押し込まれた場合は相殺して0にする
    if (key_A && key_D)
    {
        retValue = 0;
    }
    //  Aキーなら-1.0fに
    else if (key_A)
    {
        retValue = INPUT_STRENGTH_MIN;
    }
    //  Dキーなら1.0fに
    else if (key_D)
    {
        retValue = INPUT_STRENGTH_MAX;
    }

    return retValue;
}
//  縦方向の入力の強さを取得
float UserInputHandler::GetInputStrengthY()
{
    //  コントローラーの状態を取得する変数を定義
    //  NOTE:: GetStateの引数には0から始まるコントローラーの番号を指定。
    //         今回は1人プレイなので0のみを指定
    auto gamePadState = m_gamePad->GetState(0, GamePad::DEAD_ZONE_CIRCULAR);
    //  キーボードの状態を取得する変数を定義
    auto keyboardState = m_keyboard->GetState();
    //  使いまわす値を変数に格納
    bool key_S = keyboardState.S;
    bool key_W = keyboardState.W;

    //  返す値
    float retValue = 0;

    //  ゲームパッドでの入力の強さを得る
    //  NOTE: -1.0f ～ 1.0fに正規化されている値が返されます
    retValue = gamePadState.thumbSticks.leftY;

    //  キーボード押し込み時は固定の値にする
    //  NOTE: SとWが同時に押し込まれた場合は相殺して0にする
    if (key_S && key_W)
    {
        retValue = 0;
    }
    //  Sキーなら-1.0fに
    else if (key_S)
    {
        retValue = INPUT_STRENGTH_MIN;
    }
    //  Wキーなら1.0fに
    else if (key_W)
    {
        retValue = INPUT_STRENGTH_MAX;
    }

    return retValue;
}
