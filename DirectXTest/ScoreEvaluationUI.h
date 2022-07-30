#pragma once
#include <DirectXMath.h>

using namespace DirectX;

enum class ScoreType;
enum SpriteType;
//  評価を表示するUI
class ScoreEvaluationUI
{
public:
    ScoreEvaluationUI();
    ~ScoreEvaluationUI();

    //  リセット
    void Reset();

    //  設定
    void SetUp(class IScoreGetter* _score);

    //  更新
    void Update();

    //  画像描画
    void DrawSprite();

    //  アニメーションしているか
    bool IsPlayAnimation() { return m_isPlayAnimation; }
private:
    //  評価の決定
    void DecideEvaluate(const int _totalScore);

    //  評価の表示アニメーション
    void PlayAnimation();

    //  表示する画像の種類
    SpriteType m_spriteType;
    //  画像の座標
    XMFLOAT2 m_pos;
    //  画像の移動量
    float m_moveRate;
    //  画像の移動量の最大値
    float m_maxMoveRate;
    //  画像の拡大率
    float m_magnification;
    //  画像のアルファ値
    float m_alfa;

    //  アニメーションしているか
    bool m_isPlayAnimation;

    //  表示までの時間を測るクラスのインスタンスを保持するポインタ
    class Timer* m_displayTimer;
    //  色を設定するクラスのインスタンスを保持するポインタ
    class ColorValue* m_color;

    //  画像の描画クラスのインスタンスをシングルトンで取得
    class SpriteDrawer& m_sprite;
    //  サウンドクラスのインスタンスをシングルトンで取得
    class Sound& m_sound;
};