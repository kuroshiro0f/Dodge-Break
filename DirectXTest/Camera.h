#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  カメラの操作を行うクラス
class Camera
{
public:
    Camera();
    ~Camera();

    //  カメラの設定
    void SetUpCamera(const XMFLOAT3& _pos);

    //  カメラのアニメーション
    //  NOTE: ゲーム開始時にプレイヤーをズームした状態から定位置までカメラが移動する
    void PlayAnimation(const float _deltaTime);

    //  カメラのアニメーションが終了したか
    bool IsFinishAnimation()const { return m_isFinishAnimation; }
private:
    //  カメラの座標
    XMFLOAT3 m_pos;
    //  注視点となる座標
    XMFLOAT3 m_targetPos;

    //  カメラの定位置
    XMFLOAT3 m_fixedPos;
    //  注視点の定位置
    XMFLOAT3 m_targetFixedPos;

    //  アニメーション時のカメラの移動量
    XMFLOAT3 m_animationCameraSpeed;
    //  アニメーション時の注視点の移動量
    float m_animationTargetSpeed;

    //  カメラのアニメーションが終了したか
    bool m_isFinishAnimation;

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
};