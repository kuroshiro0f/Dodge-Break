#pragma once
#include <DirectXMath.h>
#include <unordered_map>
#include <vector>

using namespace DirectX;

enum class PMDModelType;
//  PMDモデルのロードや描画を行うクラス
class PMDModel
{
public:
    PMDModel();
    ~PMDModel();

    //  初期化
    void Init();

    //  更新
    void Update();

    //  描画
    //  TODO: 同じ関数名にせず、処理ごとに関数名を変更
    void Draw(const XMFLOAT3& _pos, const PMDModelType _type);
    void Draw(const XMFLOAT3& _pos, const float _angle, const PMDModelType _type);

    //  拡大率と座標を変更して描画
    void DrawChangePosAndScale(const XMFLOAT3& _pos, const float _scale, const PMDModelType _type);

    //  モデルを描画するための設定
    void SetUp(const int _modelNum = 1);
private:
    //  ロード
    void Load(const PMDModelType _type);

    //  描画用クラスの配列の添え字を指定する番号
    int m_drawClassIndexNumber;

    //  構造体へのポインタ
    std::unordered_map<PMDModelType,struct PMDDrawData*> m_drawData;    //  PMDの描画に必要なデータ

    //  シングルトンクラスへの参照
    std::unordered_map<PMDModelType,class PMDLoader*> m_loader; //  PMDモデルのロード
    std::vector<class PMDShifter*> m_shifter;       //  PMDモデルの座標変換
    std::vector<class PMDDrawer*> m_drawer;         //  PMDモデルの描画
};