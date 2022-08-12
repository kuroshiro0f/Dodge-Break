#include "WaveDirector.h"

#include "Timer.h"
#include "JsonDataType.h"
#include "EventNotificator.h"
#include "EventType.h"

#include "Singleton.h"
#include "LoadJson.h"

#if _DEBUG
#include "DataReLoader.h"
#endif

WaveDirector::WaveDirector()
    :m_changeWaveTimer(new Timer())
    , m_startAppearEnemy(nullptr)
    , m_isDisplayEnemy(nullptr)
    , m_exitEnemy(nullptr)
    , m_finishWaveFunc(nullptr)
    , m_waveNum(0)
    , m_aliveTime(0)
    , m_isInterval(false)
    , m_param({})
{
}

WaveDirector::~WaveDirector()
{
    delete m_changeWaveTimer;
}

//  初期化
void WaveDirector::Init(const std::function<void(int)> _startAppearEnemy, const std::function<bool()> _isDisplayEnemy,
    const std::function<void()> _exitEnemy, const std::function<void()> _finishWaveFunc, const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  エネミーの登場を開始する関数のポインタを取得
    m_startAppearEnemy = _startAppearEnemy;
    //  エネミーが登場しているか確認する関数のポインタを取得
    m_isDisplayEnemy = _isDisplayEnemy;
    //  エネミーを退場させる関数のポインタを取得
    m_exitEnemy = _exitEnemy;
    //  ウェーブ終了時の処理を行う関数のポインタを取得
    m_finishWaveFunc = _finishWaveFunc;

    //  外部ファイルからデータをロード
    LoadFileData();
#if _DEBUG
    //  ファイルをロードする関数を、
    //  データの再読み込みを行うクラスに渡す
    DataReLoader& dataReLoader = Singleton<DataReLoader>::GetInstance();
    dataReLoader.RegisterReLoadFunc(std::bind(&WaveDirector::LoadFileData, this));
#endif
}

//  リセット
void WaveDirector::Reset()
{
    //  最初のウェーブから
    m_waveNum = 0;
    //  生存時間をリセット
    m_aliveTime = 0;
    //  インターバルではない
    m_isInterval = false;
    //  時間計測クラスのリセット
    m_changeWaveTimer->Reset();
}

//  最初のウェーブを開始する処理
void WaveDirector::Start()
{
    m_startAppearEnemy(++m_waveNum);
    m_changeWaveTimer->Start();
}

//  外部ファイルからデータを読み取る
void WaveDirector::LoadFileData()
{
    //  外部ファイルからデータを取得
    LoadJson& fileData = Singleton<LoadJson>::GetInstance();

    //  外部ファイルのデータをもとにパラメータを設定
    int num = fileData.GetArraySize(JsonDataType::Wave, "WaveTime");
    for (int i = 0; i < num; i++)
    {
        m_param.waveTime.emplace_back(fileData.GetFloatData(JsonDataType::Wave, "WaveTime", i));
        m_param.waveInterval.emplace_back(fileData.GetFloatData(JsonDataType::Wave, "WaveInterval", i));
    }
    m_param.waveTotalNum = num;
}

//  更新
void WaveDirector::Update()
{
    //  ウェーブ中の処理
    if (!m_isInterval)
    {
        //  ウェーブ終了時間になったら、エネミーを退場させてインターバルへ変更
        if (m_changeWaveTimer->GetElapseTime() >= m_param.waveTime[m_waveNum - 1])
        {
            m_exitEnemy();
            if (!m_isDisplayEnemy())
            {
                m_isInterval = true;
                m_aliveTime += m_changeWaveTimer->GetElapseTime(); 
                m_changeWaveTimer->Start();
                //  最終ウェーブまで終わった場合は、終了時に実行する関数を呼ぶ
                if (m_waveNum >= m_param.waveTotalNum)
                {
                    m_finishWaveFunc();
                }
            }
        }
    }
    //  インターバル中の処理
    else
    {
        //  インターバルが終わったら次のウェーブを開始
        if (m_changeWaveTimer->GetElapseTime() >= m_param.waveInterval[m_waveNum - 1])
        {
            m_isInterval = false;
            m_startAppearEnemy(++m_waveNum);
            m_changeWaveTimer->Start();
        }
    }
}

//  生存時間を得る
float WaveDirector::GetAliveTime()
{
    if (!m_isInterval)
    {
        m_aliveTime += m_changeWaveTimer->GetElapseTime();
    }

    return m_aliveTime;
}

//  時間を得る関数
float WaveDirector::GetTime() const
{
    return m_changeWaveTimer->GetElapseTime();
}
//  インターバル中か得る関数
bool WaveDirector::IsInterval() const
{
    return m_isInterval;
}
