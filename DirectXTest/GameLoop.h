#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  ゲームの処理をループさせるクラス
class GameLoop
{
public:
    GameLoop();
    ~GameLoop();

    //  初期化
    void Init();

    //  ループ
    void Run();

    //  ループの終了
    void FinishRun();
private:
    //  デルタタイム
    float m_deltaTime;

    //  ループが終了するか
    bool m_isFinishRun;

    //  クラスへのポインタ
    class StringDrawer* m_string; //  文字の描画
    class Timer* m_gameLoopTimer;   //  ループ時間の計測

    //  シングルトンクラスへの参照
    class Device& m_device;         //  デバイスの管理
    class EffekseerManager& m_effect;   //  Effekseerから読み込んだエフェクトの管理
    class UserInputHandler& m_userInputHandler; //  入力検知
    class CheckError& m_error;
};