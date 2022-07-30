#pragma once
#include <DirectXMath.h>
#include <unordered_map>

using namespace DirectX;

enum class EnemyType;
//  塔の座標を知らせるクラス
class EnemyDefeaterMapper
{
public:
    EnemyDefeaterMapper();
    ~EnemyDefeaterMapper();

    //  塔の座標を登録
    void RegisterEnemyDefeaterPos(const EnemyType _type, const XMFLOAT3& _pos);

    //  塔の座標を得る
    XMFLOAT3 GetEnemyDefeaterPos(const EnemyType _type);
private:
    //  塔の座標を保持する配列
    std::unordered_map<EnemyType, XMFLOAT3> m_enemyDefeaterPosList;
};