#pragma once
#include <vector>
#include <functional>
#include <memory>

#include "IWaveConditionGetter.h"

//  ウェーブの移行の指示を行うクラス
class WaveDirector :public IWaveConditionGetter
{
public:
    WaveDirector();
    ~WaveDirector();

    //  初期化
    void Init(const std::function<void(int)> _startAppearEnemy, const std::function<bool()> _isDisplayEnemy,
        const std::function<void()> _exitEnemy, const std::function<void()> _finishWaveFunc, const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  最初のウェーブを開始する処理
    void Start();

    //  更新
    void Update();

    //  生存時間を得る
    float GetAliveTime();

    //  時間を得る関数
    float GetTime()const override;
    //  インターバル中か得る関数
    bool IsInterval()const override;
private:
    //  外部ファイルからデータを読み取る
    void LoadFileData();

    //  現在のウェーブの番号
    int m_waveNum;

    //  ウェーブ間のインターバル中か
    bool m_isInterval;

    //  生存時間
    //  NOTE: 延長時間とインターバルを除いた値をカウント
    float m_aliveTime;

    //  エネミーの登場を開始する関数へのポインタ
    std::function <void(int)> m_startAppearEnemy;
    //  画面上からエネミーを退場させる関数へのポインタ
    std::function <void()> m_exitEnemy;
    //  エネミーが画面上にいるか確認する関数のポインタ
    std::function <bool()> m_isDisplayEnemy;
    //  ウェーブ狩猟時に実行する関数へのポインタ
    std::function <void()> m_finishWaveFunc;

    //  クラスのインスタンスを保持するポインタ
    class Timer* m_changeWaveTimer;     //  ウェーブの変更時間計測用

    //  外部ファイルから読みとるパラメータ
    struct Param
    {
        int waveTotalNum;   //  ウェーブの数
        std::vector<float> waveTime;    //  ウェーブ毎の時間
        std::vector<float> waveInterval; //  ウェーブ間のインターバルの時間
    };
    Param m_param;
};