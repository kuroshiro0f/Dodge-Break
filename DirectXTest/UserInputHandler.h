#pragma once
#include <functional>
#include <unordered_map>
#include <GamePad.h>
#include <Keyboard.h>

using namespace DirectX;

//  ユーザーの入力を検知し、対応した処理を行うクラス
//  シングルトンで扱う
class UserInputHandler
{
public:
    UserInputHandler();
    ~UserInputHandler();

    //  命令の種類
    enum class OperationType
    {
        MovePlayer,     //  プレイヤーの移動

        ToNext,         //  次へ進む

        Retry,          //  やり直し
#if _DEBUG
        //  デバッグ用
        Pause,      //  一時停止
        Restart,    //  再開

        RestartNextPoint,       //  次のチェックポイントへ
        RestartPreviousPoint,   //  前のチェックポイントへ

        ReLoadFile,             //  外部ファイルの再読み込み
#endif

        FinishGame      //  ゲーム終了
    };

    //  初期化
    void Init();

    //  命令を行う関数ポインタのリセット
    //  NOTE: シーン更新時にリセットし、前のシーンの関数が呼ばれないようにする。
    //  NOTE: ゲーム終了処理はゲームを通して共通のため、リセットを行わない。
    void ResetOperationFunc();

    //  入力の更新
    void UpdateInput();

    //  命令の登録
    void RegisterOperation(const OperationType _type, const std::function<void()> _operationFunc);

    //  入力の強さを取得
    float GetInputStrengthX();  //  横方向
    float GetInputStrengthY();  //  縦方向
private:
    //  ゲームパッドの入力を検知
    void HandleInputForGamePad();
    //  キーボードの入力を検知
    //  NOTE: キー入力が実行された際に、ゲームパッドの入力の検知を飛ばすため、
    //        bool型にして、内部で処理が行われたかを返す。
    bool HandleInputForKeyboard();

    //  ゲームパッド入力
    std::function<void()> m_controlLeftStick; //  左スティックを動かす
    std::function<void()> m_pressAButton;     //  Aボタン押し込み
    std::function<void()> m_pressBButton;     //  Bボタン押し込み
    std::function<void()> m_pressStartButton; //  Startボタン押し込み
#if _DEBUG
    std::function<void()> m_pressLButton;   //  Lボタン押し込み
    std::function<void()> m_pressRButton;   //  Rボタン押し込み
    std::function<void()> m_pressDpadLeftButton;    //  Dpad左押し込み
    std::function<void()> m_pressDpadRightButton;   //  Dpad右押し込み
    std::function<void()> m_pressDpadUpButton;      //  Dpad上押し込み
#endif

    //  キーボード入力
    std::function<void()> m_pressWASDKey;    //  WASDキー押し込み
    std::function<void()> m_pressEnterKey;   //  Enterキー押し込み
    std::function<void()> m_pressSpaceKey;   //  Spaceキー押し込み
    std::function<void()> m_pressEscapeKey;  //  Escapeキー押し込み
#if _DEBUG
    std::function<void()> m_pressPKey;      //  Pキー押し込み
    std::function<void()> m_pressLKey;      //  Lキー押し込み
    std::function<void()> m_pressLeftKey;   //  ←キー押し込み
    std::function<void()> m_pressRightKey;  //  →キー押し込み
    std::function<void()> m_pressZeroKey;   //  0キー押し込み
#endif

    //  各デバイスの状態を取得するクラスへのポインタ
    std::unique_ptr<GamePad> m_gamePad;     //  ゲームパッド
    std::unique_ptr<Keyboard> m_keyboard;   //  キーボード
};