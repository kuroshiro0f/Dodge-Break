#pragma once
#include "CollisionObject.h"

#include <DirectXMath.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

using namespace DirectX;

enum class EnemyType;
enum class EnemyAttackType;
enum class EnemyState;
//  エネミークラス
class Enemy :public CollisionObject
{
public:
    Enemy();
    ~Enemy();

    //  初期化
    void Init(const std::shared_ptr<class EventNotificator> _eventClass, std::function<void()> _increaseDefeatEnemyNum);
    //  リセット
    void Reset();

    //  外部ファイルから読み取ったデータをロード
    void LoadFileData();

    //  更新
    void Update(const XMFLOAT3& _playerPos, const float _deltaTime);

    //  描画
    void Draw();

    //  エネミーをセットする
    void Appear(const struct AppearEnemyData& _data);

    //  エネミーを倒す
    void Defeat();
    //  エネミーが退場する
    //  NOTE: 撃破ではなくいなくなるだけ
    //  NOTE: 当たり判定を消し、中心から離れるように移動して、画面外に移動後に完全に消えるといった流れ
    void Exit();

    //  攻撃の停止
    void StopAttack();

    //  エネミーの種類のゲッター
    EnemyType GetEnemyType()const;
    //  エネミーの状態のゲッター
    EnemyState GetEnemyState()const;
private:
    //  衝突した際の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  エネミーの座標の移動
    void TrackMove(const XMFLOAT3& _targetPos);

    //  エネミーの座標を指定した移動
    void NoTrackMove(const XMFLOAT3& _playerPos, const std::vector<int>& _movePoint, const std::vector<int>& _controlPoint, std::vector<float>& _moveRate,const int _moveNum);

    //  退場時の移動
    void ExitMove();

    //  角度の計算
    void CalculateAngle(const XMFLOAT3& _targetPos);

    //  ぶつかった際の移動
    void OnOverlapCollision(const XMFLOAT3& _pos);

    //  直前の座標
    XMFLOAT3 m_beforePos;
    //  角度
    float m_angle;
    //  デルタタイム
    float m_deltaTime;

    //  移動割合
    float m_moveRate;

    //  攻撃した回数
    int m_attackedNum;

    //  外部ファイルによって設定されたパラメータを参照する際に、配列の添え字に設定される番号
    //  NOTE: 現在の番号の次の番号も配列の添え字に設定する場合があり、
    //        さらにこの番号は最大値の次が0となってループするため、
    //        その処理のためにm_nextPointIndexを宣言
    int m_paramIndex;           //  現在の番号
    int m_nextParamIndex;       //  次の番号

    //  最初の攻撃を行ったか
    bool m_isShootFirstAttack;
    //  リスタート時の攻撃開始までの時間
    float m_restartShootTimeAdd;

    //  エネミーの状態
    EnemyState m_state;

    //  クラスへのポインタ
    class Timer* m_shootTimer;  //  発射間隔のタイマー
    Timer* m_exitTimer;     //  退場までのタイマー
    Timer* m_burstTimer;    //  バースト撃ちのタイマー
    class EnemyAttackDirector* m_attackDirector;   //  攻撃を管理するクラス

    //  バースト撃ちで狙う座標
    XMFLOAT3 m_burstTargetPos;

    //  外部ファイルによって設定されたパラメータ
    struct AppearEnemyData* m_param;

    //  エネミーの撃破数を増加させる関数のポインタ
    std::function<void()> m_increaseDefeatEnemyNumFunc;

    //  シングルトンクラスへの参照
    class PMDModel& m_model;            //  PMDモデルの管理
    class EffekseerManager& m_effect;   //  Effekseerから読み込んだエフェクトの管理
    class Sound& m_sound;               //  音の管理

    //  固定移動用の座標指定(8x8)
    //  NOTE: 外部ファイルにて設定した値を読み込んで使用
    //  NOTE: 移動に必要なデータ
    std::unordered_map<int, XMFLOAT3> m_movePosData;
    //  中央の座標
    XMFLOAT3 m_centerPos;

    std::shared_ptr<class EventNotificator> m_eventClass;
};