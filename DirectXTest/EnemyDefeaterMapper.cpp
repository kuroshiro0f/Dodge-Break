#include "EnemyDefeaterMapper.h"

EnemyDefeaterMapper::EnemyDefeaterMapper()
{
    //  処理なし
}

EnemyDefeaterMapper::~EnemyDefeaterMapper()
{
    m_enemyDefeaterPosList.clear();
}

//  塔の座標を登録
void EnemyDefeaterMapper::RegisterEnemyDefeaterPos(const EnemyType _type, const XMFLOAT3& _pos)
{
    m_enemyDefeaterPosList[_type] = _pos;
}

//  塔の座標を得る
XMFLOAT3 EnemyDefeaterMapper::GetEnemyDefeaterPos(const EnemyType _type)
{
    return m_enemyDefeaterPosList[_type];
}
