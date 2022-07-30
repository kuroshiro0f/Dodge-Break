#pragma once
#include "IDefeatEnemyNumGetter.h"

#include <DirectXMath.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

using namespace DirectX;

enum class EnemyType;
enum class EnemyAttackType;
//  エネミーの更新や描画を管理するクラス
class EnemyManager :public IDefeatEnemyNumGetter
{
public:
    EnemyManager();
    ~EnemyManager();

    //  初期化
    void Init(const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  更新
    void Update(const XMFLOAT3& _playerPos, const float _deltaTime);

    //  描画
    void DrawEnemy();

    //  エネミーを出現させる
    void AppearEnemy(const std::vector<struct AppearEnemyData*>& _data);

    //  エネミーを倒す
    void DefeatEnemy(const EnemyType _type);

    //  エネミーを退場させる
    void ExitAllEnemy();

    //  エネミーは画面上に存在するか
    bool IsDisplayEnemy();
    //  指定した種類のエネミーは画面上に存在するか
    bool IsDisplayEnemyType(const EnemyType _type);

    //  エネミーの撃破数の増加
    void IncreaseDefeatEnemyNum();

    //  エネミーの撃破数のゲッター
    int GetDefeatEnemyNum()const override { return m_defeatEnemyNum; }
private:
    //  データ
    struct MapData* m_mapData;
    //  クラスへのポインタ
    std::vector<class Enemy*> m_enemy;  //  敵

    //  倒されたエネミーの数
    int m_defeatEnemyNum;

    //  出現を止めるエネミーの種類
    EnemyType m_stopAppearEnemyType;
    //  出現を止める時間
    class Timer* m_stopAppearTimer;

    //  シングルトンクラスへの参照
    class CheckError& m_error;
};