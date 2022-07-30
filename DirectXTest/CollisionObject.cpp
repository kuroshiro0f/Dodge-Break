#include "CollisionObject.h"

#include "CollisionType.h"

#include "Singleton.h"
#include "CollisionManager.h"

CollisionObject::CollisionObject()
    :m_pos({ 0,0,0 })
    , m_radius(0)
    , m_type(CollisionType::None)
    , m_isDisplay(false)
{
}

//  初期化
void CollisionObject::Init(const CollisionType& _type)
{
    //  CollisionMgrクラスのシングルトンとしてインスタンスを得る
    CollisionManager& collisionManager = Singleton<CollisionManager>::GetInstance();
    //  オブジェクトのタイプを設定
    m_type = _type;
    //  当たり判定を持つクラスをセット
    collisionManager.SetUpCollision(this);
}
