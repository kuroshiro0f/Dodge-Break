#pragma once
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include <deque>
#include <functional>
#include <memory>

using namespace DirectX;

enum class EnemyType;
enum class CollisionType;
//  出現させるエネミーを決定して、出現処理に関する指示を各クラスに行うクラス
//  NOTE: 外部データを読み取って配列に格納し、
//        出現時間になったら、そのオブジェクトを出現させる仕組み
class AppearEnemyDirector
{
public:
    AppearEnemyDirector();
    ~AppearEnemyDirector();

    //  初期化
    void Init(const std::function <void(const std::vector<struct AppearEnemyData*>&)> _appearEnemy,
        const std::function<void()> _activateEnemyDefeater, const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  出現させるための処理を開始する
    void Start(int _groupNum);

    //  更新
    void Update();

    //  出現時間の延長
    void ExtendAppearTime();
    
#if _DEBUG
    //  指定された時間から再開
    void RestartCheckPoint(const int _num);

    //  再読み込み
    void ReLoad();

    //  リスタートのチェックポイントの番号表示用
    int GetDebugCheckPointNum() { return m_debugCheckPointIndex; }
#endif
private:
    //  ロード
    void Load();

    //  出現させるエネミーを決める
    //  NOTE: 外部ファイルにて出現時間順の昇順でソートしてあるため、
    //        順番に出現させるだけでいい
    void DecideAppearEnemy();

    //  出現させるエネミーのデータを設定する
    struct AppearEnemyData SetUpEnemyData(int _enemyNum);

    //  出現させる時に呼び出す関数ポインタ
    std::function <void(const std::vector<AppearEnemyData*>&)> m_appearEnemy;
    //  塔を起動させるときに呼び出す関数ポインタ
    std::function<void()> m_activateEnemyDefeater;

    //  これまでに出現させた回数
    //  NOTE: この変数をデータを格納している配列の添え字に設定して、
    //        エネミーの出現順番を管理
    int m_appearCount;

    //  出現させるエネミーのウェーブの番号
    int m_appearWaveNum;

    //  NOTE: ウェーブ中での延長回数に応じて延長時間を算出し、
    //        エネミーの出現時間を延長する
    //  出現時間の延長回数
    int m_extendCount;
    //  出現時間の延長時間
    float m_extendTime;
#if _DEBUG
    //  リスタートの時間
    float m_restartTime;

    //  リスタート用のチェックポイント格納配列
    std::vector<float> m_debugCheckPoint;
    int m_debugCheckPointIndex;     //  添え字
#endif

    //  出現させるエネミーのデータを格納する変数
    std::vector<AppearEnemyData*> m_enemyData;
    std::vector<AppearEnemyData*> m_appearEnemyData;

    //  クラスへのポインタ
    class Timer* m_appearTimer;     //  出現時間管理用

    //  シングルトンクラスへの参照
    class LoadJson& m_json;               //  jsonファイルのロードや情報の保管
};