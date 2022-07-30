#pragma once
#include <DirectXMath.h>

#include "SpriteType.h"

using namespace DirectX;

//  スコアの評価UIのデータ
namespace ScoreEvaluationUIData
{
    //  1段階目の評価
    //  画像
    const SpriteType FIRST_EVALUATE_SPRITE_TYPE = SpriteType::FirstEvaluate;
    //  基準となる点数
    const int FIRST_EVALUATE_SCORE = 0;
    //  座標
    const XMFLOAT2 FIRST_EVALUATE_POS = { 800,856 };
    //  最初の座標の移動割合
    //  NOTE: 最初は少しずらしたところから定位置に収まるようにアニメーションをする
    const float FIRST_POS_MOVE_RATE_MAX = 0.3f;

    //  2段階目の評価
    //  画像
    const SpriteType SECOND_EVALUATE_SPRITE_TYPE = SpriteType::SecondEvaluate;
    //  基準となる点数
    const int SECOND_EVALUATE_SCORE = 150000;
    //  座標
    const XMFLOAT2 SECOND_EVALUATE_POS = { 770,856 };
    //  最初の座標の移動割合
    const float SECOND_POS_MOVE_RATE_MAX = 0.4f;

    //  3段階目の評価
    //  画像
    const SpriteType THIRD_EVALUATE_SPRITE_TYPE = SpriteType::ThirdEvaluate;
    //  基準となる点数
    const int THIRD_EVALUATE_SCORE = 400000;
    //  座標
    const XMFLOAT2 THIRD_EVALUATE_POS = { 740,856 };
    //  最初の座標の移動割合
    const float THIRD_POS_MOVE_RATE_MAX = 0.5f;

    //  4段階目の評価
    //  画像
    const SpriteType FORTH_EVALUATE_SPRITE_TYPE = SpriteType::ForthEvaluate;
    //  基準となる点数
    const int FORTH_EVALUATE_SCORE = 650000;
    //  座標
    const XMFLOAT2 FORTH_EVALUATE_POS = { 680,856 };
    //  最初の座標の移動割合
    const float FORTH_POS_MOVE_RATE_MAX = 0.7f;

    //  アニメーションでの変化量
    //  NOTE: 二段階に分けるため、二つ定義
    const float FIRST_PLAY_ANIMATION_SPEED = 0.01f;
    const float SECOND_PLAY_ANIMATION_SPEED = 0.05f;

    //  拡大率の最大値
    const float MAGNIFICATION_MAX = 3.0f;
    //  最小値
    const float MAGNIFICATION_MIN = 1.0f;
    //  アニメーションの変化量が切り替わるタイミングの拡大率の値
    //  NOTE: 拡大率がこの定数で設定した値まで変動したときに、アニメーションの速度を変更します。
    const float ANIMATION_SPEED_CHANGE_POINT = 2.2f;

    //  アルファ値の最大値
    const float ALFA_MAX = 1.0f;
    //  最小値
    const float ALFA_MIN = 0.03f;

    //  座標の移動割合の最小値
    const float POS_MOVE_RATE_MIN = 0;
}