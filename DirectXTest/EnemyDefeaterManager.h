#pragma once
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>

using namespace DirectX;

enum class EnemyType;
enum class ChangeEnergyType;
//  塔の更新や描画を管理するクラス
class EnemyDefeaterManager
{
public:
    EnemyDefeaterManager();
    ~EnemyDefeaterManager();

    //  初期化
    void Init(const std::shared_ptr<class EventNotificator> _eventClass, const std::shared_ptr<class EnemyDefeaterMapper> _enemyDefeaterMapperClass,
        const std::function <void(const EnemyType&)> _defeatFunc, const std::function<bool(const EnemyType)> _isDisplayEnemy);
    //  値のリセット
    void Reset();

    //  更新
    void Update(const float _deltaTime);

    //  描画
    void DrawEnemyDefeater();

    //  塔の起動
    void ActivateEnemyDefeater();
    //  NOTE: エネルギーが溜まっているときに新たにエネミーが登場した際、
    //        起動していない塔があれば起動する
    void ActivateStandByEnemyDefeater();
    
    //  塔を停止させる
    //  NOTE: 退場して画面からいなくなったエネミーの種類に対応する塔を準備中に変更
    void StopEnemyDefeater();
private:
    //  出現しているエネミーの種類を得る関数へのポインタ
    std::function<bool(const EnemyType)> m_isDisplayEnemy;

    //  クラスへのポインタ
    std::vector<class EnemyDefeater*> m_enemyDefeater;  //  敵を倒す塔

    //  シングルトンクラスへの参照
    class CheckError& m_error;
};