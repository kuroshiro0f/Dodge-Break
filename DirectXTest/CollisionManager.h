#pragma once
#include <list>

//  当たり判定の管理をするクラス
class CollisionManager
{
public:
    CollisionManager();
    ~CollisionManager();

    //  更新
    void Update();

    //  当たり判定を持つクラスを設定する
    void SetUpCollision(class CollisionObject* _class);
private:
    //  二つの球の当たり判定
    void CollideTwoSphere(class CollisionObject* _firstClass, class CollisionObject* _secondClass);

    //  当たり判定を持つクラスを格納する配列
    class CollisionObject* m_player;
    std::list<CollisionObject*> m_enemy;
    std::list<CollisionObject*> m_enemyAttack;
    std::list<CollisionObject*> m_enemyDefeater;
    class CollisionObject* m_dodgeRange;
    std::list<CollisionObject*> m_enemyDefeaterActivateEffect;
};