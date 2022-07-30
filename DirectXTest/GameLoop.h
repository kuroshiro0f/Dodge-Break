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
private:
    //  デルタタイム
    float m_deltaTime;

    //  クラスへのポインタ
    class StringDrawer* m_string; //  文字の描画
    class Timer* m_gameLoopTimer;   //  ループ時間の計測

    //  シングルトンクラスへの参照
    class KeyBoard& m_keyBoard;     //  キーの入力検知
    class Device& m_device;         //  デバイスの管理
    class EffekseerManager& m_effect;   //  Effekseerから読み込んだエフェクトの管理
    class CheckError& m_error;
};