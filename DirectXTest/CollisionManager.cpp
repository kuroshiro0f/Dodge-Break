#include "CollisionManager.h"

#include "CollisionType.h"
#include "ObjectTagName.h"
#include "CollisionObject.h"

//  定数0.5
const float HALF = 0.5f;

//  定数2
const float TWO = 2.0f;

CollisionManager::CollisionManager()
    :m_player(nullptr)
    , m_dodgeRange(nullptr)
    , m_enemy({})
    , m_enemyAttack({})
    , m_beamAttack({})
    , m_enemyDefeater({})
    , m_enemyDefeaterActivateEffect({})
{
    //  処理なし
}

CollisionManager::~CollisionManager()
{
    //  処理なし
}

//  更新
void CollisionManager::Update()
{
    //  ループ回数カウント用
    int count = 0;

    for (auto enemyItr : m_enemy)
    {
        //  プレイヤーとエネミーの衝突判定
        DetectCollided(m_player, enemyItr);
        //  エネミーと回避範囲の衝突判定
        DetectCollided(enemyItr, m_dodgeRange);
    }
    for (auto enemyAttackItr : m_enemyAttack)
    {
        //  エネミーの攻撃とプレイヤーの衝突判定
        DetectCollided(enemyAttackItr, m_player);
        //  エネミーの攻撃と回避範囲の衝突判定
        DetectCollided(enemyAttackItr, m_dodgeRange);
    }
    for (auto beamAttackItr : m_beamAttack)
    {
        //  プレイヤーとビーム攻撃の衝突判定
        DetectCollided(m_player, beamAttackItr);
        //  回避範囲とビーム攻撃の衝突判定
        DetectCollided(m_dodgeRange, beamAttackItr);
    }
    for (auto EnemyDefeaterItr : m_enemyDefeater)
    {
        //  塔とプレイヤーの衝突判定
        DetectCollided(EnemyDefeaterItr, m_player);
        //  塔と塔を起動するエフェクトの衝突判定
        for (auto EnemyDefeaterActivaterItr : m_enemyDefeaterActivateEffect)
        {
            DetectCollided(EnemyDefeaterItr, EnemyDefeaterActivaterItr);
        }
    }
}

//  当たり判定のあるクラスを設定
void CollisionManager::SetUpCollision(CollisionObject* _class)
{
    switch (_class->GetObjectTagName())
    {
    case ObjectTagName::Player:
        m_player = _class;
        break;
    case ObjectTagName::Enemy:
        m_enemy.emplace_back(_class);
        break;
    case ObjectTagName::EnemyAttack:
        m_enemyAttack.emplace_back(_class);
        break;
    case ObjectTagName::BeamAttack:
        m_beamAttack.emplace_back(_class);
        break;
    case ObjectTagName::EnemyDefeater:
        m_enemyDefeater.emplace_back(_class);
        break;
    case ObjectTagName::DodgeRange:
        m_dodgeRange = _class;
        break;
    case ObjectTagName::EnemyDefeaterActivateEffect:
        m_enemyDefeaterActivateEffect.emplace_back(_class);
        break;
    default:
        break;
    }
}

//  衝突判定を行う
void CollisionManager::DetectCollided(CollisionObject* _firstClass, CollisionObject* _secondClass)
{
    //  非表示のオブジェクトは衝突判定を無視
    if (!(_firstClass->IsDisplay()) || !(_secondClass->IsDisplay()))
    {
        return;
    }

    //  二つとも球状の場合
    if (_firstClass->GetCollisionType() == CollisionType::Sphere && _secondClass->GetCollisionType() == CollisionType::Sphere)
    {
        if (IsCollide(_firstClass->GetSphereData(), _firstClass->GetPos(), _secondClass->GetSphereData(), _secondClass->GetPos()))
        {
            _firstClass->OnCollisionEnter(*_secondClass);
            _secondClass->OnCollisionEnter(*_firstClass);
        }
    }
    //  片方が球状で、片方が箱型の場合
    else if (_firstClass->GetCollisionType() == CollisionType::Sphere && _secondClass->GetCollisionType() == CollisionType::Box)
    {
        if (IsCollide(_firstClass->GetSphereData(), _firstClass->GetPos(), _secondClass->GetBoxData(), _secondClass->GetPos()))
        {
            _firstClass->OnCollisionEnter(*_secondClass);
            _secondClass->OnCollisionEnter(*_firstClass);
        }
    }
}

//  二つの球状オブジェクトが衝突したか
bool CollisionManager::IsCollide(const SphereData _firstSphere, const XMFLOAT3& _firstSpherePos, const SphereData _secondSphere, const XMFLOAT3& _secondSpherePos)
{
    //  一つ目の球の座標
    float firstCirclePosX = _firstSpherePos.x;
    float firstCirclePosY = _firstSpherePos.y;
    float firstCirclePosZ = _firstSpherePos.z;
    //  一つ目の球の半径
    float firstCircleRadius = _firstSphere.radius;

    //  二つ目の球の座標
    float secondCirclePosX = _secondSpherePos.x;
    float secondCirclePosY = _secondSpherePos.y;
    float secondCirclePosZ = _secondSpherePos.z;
    //  二つ目の球の半径
    float secondCircleRadius = _secondSphere.radius;

    //  中心座標同士の距離を求めて二乗する
    float squaredX = (firstCirclePosX - secondCirclePosX) * (firstCirclePosX - secondCirclePosX);
    float squaredY = (firstCirclePosY - secondCirclePosY) * (firstCirclePosY - secondCirclePosY);
    float squaredZ = (firstCirclePosZ - secondCirclePosZ) * (firstCirclePosZ - secondCirclePosZ);
    //  両円の半径を足して二乗する
    float squaredRadius = (firstCircleRadius + secondCircleRadius) * (firstCircleRadius + secondCircleRadius);

    //  お互いの頂点の距離がお互いの半径の合計よりも短いならばtrueを返す
    return squaredX + squaredY + squaredZ <= squaredRadius;
}

//  球状オブジェクトと箱型オブジェクトが衝突したか
//  NOTE: 現状はビームとプレイヤーおよびビームと回避判定のみのため、
//        処理負荷軽減のため、2D扱い(矩形と円)で判定を行う
bool CollisionManager::IsCollide(const SphereData _sphereData, const XMFLOAT3& _spherePos, const BoxData _boxData, const XMFLOAT3& _boxPos)
{
    //  円の座標
    //  NOTE: XZ平面上で衝突判定を行うため、Z座標を円のY座標としてあつかう
    float circlePosX = _spherePos.x;
    float circlePosY = _spherePos.z;

    //  円の半径
    float circleRadius = _sphereData.radius;

    //  矩形の辺の長さ
    //  NOTE: 座標がオブジェクトの上辺の中心を指すため、
    //        広さは座標から辺の長さの半分を加算および減算し、高さは辺の長さをそのまま足す
    //  NOTE: XZ平面上で衝突判定を行うため、箱型オブジェクトの奥行を矩形の高さに設定
    float rectWidthHalf = _boxData.width * HALF;
    float rectHeight = _boxData.depth * HALF;

    //  矩形の座標
    //  NOTE: 箱型オブジェクトは現状ビームだけであるため、座標は発射口に設定しています
    float rectCenterPosX = _boxPos.x;
    float rectCenterPosY = _boxPos.z;

    //  矩形のX座標の値の最大と最小
    float rectPosXMax = rectCenterPosX + rectWidthHalf;
    float rectPosXMin = rectCenterPosX - rectWidthHalf;

    //  矩形のZ座標の値の最大と最小
    float rectPosZMax = rectCenterPosY + rectHeight;
    float rectPosZMin = rectCenterPosY;

    //  矩形の傾き角度
    float rectTiltAngle = _boxData.angle;

    //  傾き角度分傾けた矩形の各頂点の座標を算出
    //  左上頂点
    XMFLOAT2 rectUpperLeftPos = CalculatePosRotationalMovement(rectPosXMin, rectPosZMax, rectCenterPosX, rectCenterPosY, rectTiltAngle);
    float rectUpperLeftPosX = rectUpperLeftPos.x;
    float rectUpperLeftPosY = rectUpperLeftPos.y;
    //  右上頂点
    XMFLOAT2 rectUpperRightPos = CalculatePosRotationalMovement(rectPosXMax, rectPosZMax, rectCenterPosX, rectCenterPosY, rectTiltAngle);
    float rectUpperRightPosX = rectUpperRightPos.x;
    float rectUpperRightPosY = rectUpperRightPos.y;
    //  右下頂点
    XMFLOAT2 rectLowerRightPos = CalculatePosRotationalMovement(rectPosXMax, rectPosZMin, rectCenterPosX, rectCenterPosY, rectTiltAngle);
    float rectLowerRightPosX = rectLowerRightPos.x;
    float rectLowerRightPosY = rectLowerRightPos.y;
    //  左下頂点
    XMFLOAT2 rectLowerLeftPos = CalculatePosRotationalMovement(rectPosXMin, rectPosZMin, rectCenterPosX, rectCenterPosY, rectTiltAngle);
    float rectLowerLeftPosX = rectLowerLeftPos.x;
    float rectLowerLeftPosY = rectLowerLeftPos.y;

    //  矩形を構成する辺に沿って時計回りにベクトルを定義
    //  左上頂点から右上頂点
    float rectUpVecX = rectUpperRightPosX - rectUpperLeftPosX;
    float rectUpVecY = rectUpperRightPosY - rectUpperLeftPosY;
    //  右上頂点から右下頂点
    float rectRightVecX = rectLowerRightPosX - rectUpperRightPosX;
    float rectRightVecY = rectLowerRightPosY - rectUpperRightPosY;
    //  右下頂点から左下頂点
    float rectDownVecX = rectLowerLeftPosX - rectLowerRightPosX;
    float rectDownVecY = rectLowerLeftPosY - rectLowerRightPosY;
    //  左下頂点から左上頂点
    float rectLeftVecX = rectUpperLeftPosX - rectLowerLeftPosX;
    float rectLeftVecY = rectUpperLeftPosY - rectLowerLeftPosY;

    //  矩形の各頂点から円の中心座標までのベクトルを定義
    //  左上頂点から
    float circleVecXFromRectUpperLeft = circlePosX - rectUpperLeftPosX;
    float circleVecYFromRectUpperLeft = circlePosY - rectUpperLeftPosY;
    //  右上頂点から
    float circleVecXFromRectUpperRight = circlePosX - rectUpperRightPosX;
    float circleVecYFromRectUpperRight = circlePosY - rectUpperRightPosY;
    //  右下頂点から
    float circleVecXFromRectLowerRight = circlePosX - rectLowerRightPosX;
    float circleVecYFromRectLowerRight = circlePosY - rectLowerRightPosY;
    //  左下頂点から
    float circleVecXFromRectLowerLeft = circlePosX - rectLowerLeftPosX;
    float circleVecYFromRectLowerLeft = circlePosY - rectLowerLeftPosY;

    //  各辺と円が重なっているかを円の中心から各辺の距離をもとに判定
    //  上辺を判定
    if (IsOverlapCircleAndSide(rectUpperLeftPosX, rectUpperLeftPosY,
        rectUpperRightPosX, rectUpperRightPosY,
        rectUpVecX, rectUpVecY,
        circleVecXFromRectUpperLeft, circleVecYFromRectUpperLeft,
        circleVecXFromRectUpperRight, circleVecYFromRectUpperRight,
        circlePosX, circlePosY, circleRadius))
    {
        return true;
    }
    //  右辺を判定
    if (IsOverlapCircleAndSide(rectUpperRightPosX, rectUpperRightPosY,
        rectLowerRightPosX, rectLowerRightPosY,
        rectRightVecX, rectRightVecY,
        circleVecXFromRectUpperRight, circleVecYFromRectUpperRight,
        circleVecXFromRectLowerRight, circleVecYFromRectLowerRight,
        circlePosX, circlePosY, circleRadius))
    {
        return true;
    }
    //  下辺を判定
    if (IsOverlapCircleAndSide(rectLowerRightPosX, rectLowerRightPosY,
        rectLowerLeftPosX, rectLowerLeftPosY,
        rectDownVecX, rectDownVecY,
        circleVecXFromRectLowerRight, circleVecYFromRectLowerRight,
        circleVecXFromRectLowerLeft, circleVecYFromRectLowerLeft,
        circlePosX, circlePosY, circleRadius))
    {
        return true;
    }
    //  左辺を判定
    if (IsOverlapCircleAndSide(rectLowerLeftPosX, rectLowerLeftPosY,
        rectUpperLeftPosX, rectUpperLeftPosY,
        rectLeftVecX, rectLeftVecY,
        circleVecXFromRectLowerLeft, circleVecYFromRectLowerLeft,
        circleVecXFromRectUpperLeft, circleVecYFromRectUpperLeft,
        circlePosX, circlePosY, circleRadius))
    {
        return true;
    }

    //  円が矩形の中に完全に含まれている場合もチェック
    if (IsCircleRightOfVec(rectUpVecX, rectUpVecY, circleVecXFromRectUpperLeft, circleVecYFromRectUpperLeft)
        && IsCircleRightOfVec(rectRightVecX, rectRightVecY, circleVecXFromRectUpperRight, circleVecYFromRectUpperRight)
        && IsCircleRightOfVec(rectDownVecX, rectDownVecY, circleVecXFromRectLowerRight, circleVecYFromRectLowerRight)
        && IsCircleRightOfVec(rectLeftVecX, rectLeftVecY, circleVecXFromRectLowerLeft, circleVecYFromRectLowerLeft))
    {
        return true;
    }

    //  全て当てはまらない場合は衝突していない
    return false;
}

//  回転移動させた頂点を算出
XMFLOAT2 CollisionManager::CalculatePosRotationalMovement(const float _posX, const float _posY, const float _rotationReferencePointX, const float _rotationReferencePointY, const float _tiltAngle)
{
    XMFLOAT2 retPos;

    retPos.x = (static_cast<float>(cos(_tiltAngle)) * (_posX - _rotationReferencePointX)) - (static_cast<float>(sin(_tiltAngle)) * (_posY - _rotationReferencePointY)) + _rotationReferencePointX;
    retPos.y = (static_cast<float>(sin(_tiltAngle)) * (_posX - _rotationReferencePointX)) + (static_cast<float>(cos(_tiltAngle)) * (_posY - _rotationReferencePointY)) + _rotationReferencePointY;

    return retPos;
}

//  辺と円は重なっているか
bool CollisionManager::IsOverlapCircleAndSide(
    const float _startPointPosX, const float _startPointPosY,
    const float _endPointPosX, const float _endPointPosY,
    const float _sideVecX, const float _sideVecY,
    const float _vecXStartPointToCircle, const  float _vecYStartPointToCircle,
    const float _vecXEndPointToCircle, const float _vecYEndPointToCircle,
    const float _circlePosX, const float _circlePosY, const float _circleRadius)
{
    //  円の半径の2乗
    float squareCircleRadius = static_cast<float>(pow(_circleRadius, TWO));

    //  円の中心から辺までの距離
    float distanceCirclePosToSide = static_cast<float>(fabs(_sideVecX * _vecYStartPointToCircle - _sideVecY * _vecXStartPointToCircle))
        / static_cast<float>(sqrt(pow(_sideVecX, TWO) + pow(_sideVecY, TWO)));
    //  辺と円は重なっているか
    if ((_sideVecX * _vecXStartPointToCircle) + (_sideVecY * _vecYStartPointToCircle) >= 0
        && (_sideVecX * _vecXEndPointToCircle) + (_sideVecY * _vecYEndPointToCircle) <= 0
        && distanceCirclePosToSide < _circleRadius)
    {
        return true;
    }
    else if (static_cast<float>(pow(_circlePosX - _startPointPosX, TWO)) + static_cast<float>(pow(_circlePosY - _startPointPosY, TWO)) <= squareCircleRadius
        || static_cast<float>(pow(_circlePosX - _endPointPosX, TWO)) + static_cast<float>(pow(_circlePosY - _endPointPosY, TWO)) <= squareCircleRadius)
    {
        return true;
    }

    return false;
}

//  ベクトルから見て右側に円はあるか
//  NOTE: 矩形を構成する辺に沿って時計回りに定義したベクトルから見て、
//        右側に円がある状況が全てのベクトルに当てはまる場合は、
//        矩形の内部に円が完全に入っているとみなす
bool CollisionManager::IsCircleRightOfVec(const float _sideVecX, const float _sideVecY, const float _vecXStartPointToCircle, const float _vecYStartPointToCircle)
{
    return static_cast<float>(_sideVecX * _vecYStartPointToCircle - _sideVecY * _vecXStartPointToCircle) <= 0;
}
