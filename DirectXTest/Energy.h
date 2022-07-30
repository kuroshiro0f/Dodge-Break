#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

//  エネルギー自体のクラス
class Energy
{
public:
    Energy();
    ~Energy();

    //  初期化
    void Init(std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  外部ファイルから読み取ったデータをロード
    void LoadFileData();

    //  回避によるエネルギーの増加
    void IncreaseForDodge();
    //  被弾によるエネルギーの増加
    void IncreaseForDamage();
    //  エネミーの撃破によるエネルギーの減少
    void DecreaseForDeleteEnemy();

    //  エネルギー量のゲッター
    int GetEnergy()const { return m_energy; }
    //  エネルギーは満タンか
    bool IsEnergyMax()const { return m_energy == m_maxEnergyNum; }
private:
    //  エネルギー量
    float m_energy;
    //  最大量
    float m_maxEnergyNum;
    //  エネルギーの初期値
    float m_firstEnergy;

    //  エネルギーの変化量
    struct ChangeAmount
    {
        float dodge;        //  回避
        float damage;       //  被弾
        float deleteEnemy;  //  エネミーの撃破
    };
    ChangeAmount m_changeAmout;

    //  イベント通知クラス
    std::shared_ptr<EventNotificator> m_eventClass;
};