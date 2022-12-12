#include "CollisionObject.h"

#include "CollisionType.h"
#include "ObjectTagName.h"

#include "Singleton.h"
#include "CollisionManager.h"

CollisionObject::CollisionObject()
    :m_pos({ 0,0,0 })
    , m_sphereData(nullptr)
    , m_boxData(nullptr)
    , m_collisionType(CollisionType::None)
    , m_objectTagName(ObjectTagName::None)
    , m_isDisplay(false)
{
    //  処理なし
}

//  初期化
void CollisionObject::Init(const CollisionType& _collisionType, const ObjectTagName _objectTagName)
{
    //  CollisionMgrクラスのシングルトンとしてインスタンスを得る
    CollisionManager& collisionManager = Singleton<CollisionManager>::GetInstance();
    //  オブジェクトのタイプを設定
    m_collisionType = _collisionType;
    m_objectTagName = _objectTagName;
    //  当たり判定を持つクラスをセット
    collisionManager.SetUpCollision(this);
}
