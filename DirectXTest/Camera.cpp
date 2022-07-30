#include "Camera.h"

#include "XMFLOATHelper.h"
#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "Device.h"

//  カメラの上ベクトル
static const XMFLOAT3 UP_VEC = { 0.0f, 1.0f, 0.0f };

//  注視点を原点としたときのカメラの座標
static const XMFLOAT3 CAMERA_POS_FROM_TARGET = { 0.0f,140.0f,-105.0f };
//  注視点調整用
static const XMFLOAT3 FIX_TARGET_POS = { 0.0f,0.0f,-15.0f };

//  カメラの初期座標
static const XMFLOAT3 FIRST_CAMERA_POS_FROM_TARGET = { 0.0f,30.0f,-40.0f };
//  カメラの移動速度
static const float CAMERA_SPEED = 0.7f;

Camera::Camera()
    :m_device(Singleton<Device>::GetInstance())
    , m_pos(0, 0, 0)
    , m_targetPos(0, 0, 0)
    , m_fixedPos(0, 0, 0)
    , m_targetFixedPos(0, 0, 0)
    , m_animationCameraSpeed(0, 0, 0)
    , m_isFinishAnimation(false)
{
    //  処理なし
}

Camera::~Camera()
{
    //  処理なし
}

//  カメラの設定
void Camera::SetUpCamera(const XMFLOAT3& _pos)
{
    //  カメラの初期座標
    m_pos = { _pos.x + FIRST_CAMERA_POS_FROM_TARGET.x, _pos.y + FIRST_CAMERA_POS_FROM_TARGET.y, _pos.z + FIRST_CAMERA_POS_FROM_TARGET.z };
    //  注視点の初期座標
    m_targetPos = _pos;

    //  注視点の定位置の座標
    m_targetFixedPos = XMFLOATHelper::XMFLOAT3Add(m_targetPos, FIX_TARGET_POS);
    //  カメラの定位置の座標
    m_fixedPos = { m_targetFixedPos.x + CAMERA_POS_FROM_TARGET.x, m_targetFixedPos.y + CAMERA_POS_FROM_TARGET.y, m_targetFixedPos.z + CAMERA_POS_FROM_TARGET.z };

    //  カメラの定位置までの単位ベクトルを算出し、
    //  アニメーション時のカメラの移動量とする
    m_animationCameraSpeed = XMFLOATHelper::XMFLOAT3Normalize(m_pos, m_fixedPos);
    //  注視点までの単位ベクトル算出
    XMFLOAT3 targetUnitVecForFixedPos = XMFLOATHelper::XMFLOAT3Normalize(m_targetPos, m_targetFixedPos);

    //  カメラの現在位置から定位置までの距離算出
    float distanceFixedPos = XMFLOATHelper::XMFLOAT3Distance(m_pos, m_fixedPos);
    //  注視点の現在位置から定位置までの距離算出
    float distanceTargetFixedPos = XMFLOATHelper::XMFLOAT3Distance(m_targetPos, m_targetFixedPos);

    //  注視点の移動量算出
    //  NOTE: 注視点の移動量の割合をカメラの移動量の割合と合わせる
    m_animationTargetSpeed = targetUnitVecForFixedPos.z * (distanceTargetFixedPos / distanceFixedPos);

    //  フラグの初期化
    m_isFinishAnimation = false;

    //  カメラの初期パラメータを設定
    m_device.dx12->UpdateCameraPos(m_pos, m_targetPos, UP_VEC);
}

//  カメラのアニメーション
void Camera::PlayAnimation(const float _deltaTime)
{
    //  カメラの移動量
    XMFLOAT3 speed(0,0,0);
    speed.y = m_animationCameraSpeed.y * CAMERA_SPEED * _deltaTime;
    speed.z = m_animationCameraSpeed.z * CAMERA_SPEED * _deltaTime;

    //  注視点の移動量
    float targetSpeedZ = m_animationTargetSpeed * CAMERA_SPEED * _deltaTime;

    //  カメラの移動量を現在の座標に加算
    m_pos.y += speed.y;
    m_pos.z += speed.z;

    //  注視点の移動量を現在の座標に加算
    m_targetPos.z += targetSpeedZ;

    //  移動後のカメラの位置が定位置を超える場合はカメラを定位置で固定
    //  NOTE: 単位ベクトルに速度を乗算して移動量を決定しており、
    //        どの座標も1ループにおける移動量の割合は同じであるため、
    //        カメラのy座標のみで定位置を超えたか判定しております
    if (m_pos.y >= m_fixedPos.y)
    {
        m_pos = m_fixedPos;
        m_targetPos = m_targetFixedPos;
        //  アニメーション終了
        m_isFinishAnimation = true;
    }

    //  カメラの更新
    m_device.dx12->UpdateCameraPos(m_pos, m_targetPos, UP_VEC);
}