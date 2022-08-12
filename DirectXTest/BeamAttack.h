#pragma once
#include "CollisionObject.h"

#include <DirectXMath.h>
#include <vector>
#include <functional>

using namespace DirectX;

//  エネミーのビーム攻撃のクラス
class BeamAttack :public CollisionObject
{
public:
    BeamAttack();
    ~BeamAttack();

    //  初期化
    void Init();
    //  リセット
    void Reset();

    //  更新
    void Update(const float _deltaTime);
    //  発射
    void Shoot(int _attackPetternID, const XMFLOAT3& _shootPos, std::function<void(class BeamAttack*)> _returnPoolFunc);

    //  攻撃を画面上から削除し、インスタンスをオブジェクトプールに戻す
    void Delete();
private:
    //  衝突時の処理
    void OnCollisionEnter(const CollisionObject& _class)override;

    //  外部ファイルの読み込み
    void LoadFileData();

    //  ビームを伸ばす
    void Extend();

    //  各動作を処理速度に合わせるためのデルタタイム
    float m_deltaTime;

    //  ビーム攻撃の状態を定義
    enum class State
    {
        Charge,     //  エネルギーを溜める演出の再生中
        Attack,     //  攻撃中
        NoShoot,    //  未発射

        None        //  初期化用
    };
    State m_state;

    //  レーザーエフェクトの発射位置
    //  NOTE: エフェクトの再生位置であるm_shootPosと、
    //        衝突判定の基準となるm_posで使い分け
    XMFLOAT3 m_shootPos;

    //  攻撃角度
    float m_angle;

    //  攻撃の長さ
    float m_length;

    //  エフェクトの再生を操作するためのハンドル
    int m_effectHandle;

    //  エフェクト更新用の角度
    XMFLOAT3 m_effectAngle;

    //  オブジェクトプールに自身のインスタンスを戻すための関数のポインタ
    std::function<void(class BeamAttack*)> m_returnPoolFunc;

    //  クラスのインスタンスを保持するポインタ変数
    class Timer* m_changeStateTimer;    //  状態変化用タイマー
    class Timer* m_aliveTimer;      //  生存時間用タイマー

    //  シングルトンインスタンスを保持する変数
    class EffekseerManager& m_effect;   //  エフェクト再生用

    //  外部ファイルから読み取るデータをまとめた構造体
    struct Param
    {
        float aliveTime;    //  生存時間
        float playSpeed;    //  エフェクトの再生速度
        float chargeTime;   //  エネルギーを溜める演出の再生時間
        float shootAngle;   //  発射方向
        float speed;        //  攻撃の移動速度(値が0なら即着弾するように設定)
        float lengthMax;    //  最大の長さ
        float thickness;    //  太さ
    };
    //  発射するビームにて使用するパラメータ
    Param m_param;

    //  ロードしたデータを全て保持しておく配列
    std::vector<Param> m_paramList;
};