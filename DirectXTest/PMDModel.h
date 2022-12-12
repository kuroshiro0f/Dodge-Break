#pragma once
#include <DirectXMath.h>
#include <unordered_map>

using namespace DirectX;

enum class PMDModelType;
//  描画や座標変換を行うPMDモデル全てのデータを保持し、情報の変更を行うクラス
//  シングルトンで扱う
class PMDModel
{
public:
    PMDModel();
    ~PMDModel();

    //  初期化
    void Init();

    //  1ループ中の描画命令の一斉発行
    void IssueAllDrawCommand();

    //  座標を変更して描画
    //  NOTE: 実際はこの関数で描画していませんが、各クラスがモデルの描画をするにはこの関数を呼べばいいので、
    //        Draw～という命名にしております。
    //  TODO: 同じ関数名にせず、処理ごとに関数名を変更
    void Draw(const XMFLOAT3& _pos, const PMDModelType _type);
    //  座標と角度を変更して描画
    void Draw(const XMFLOAT3& _pos, const float _angle, const PMDModelType _type);

    //  拡大率と座標を変更して描画
    void DrawChangePosAndScale(const XMFLOAT3& _pos, const float _scale, const PMDModelType _type);
private:
    //  ロード
    void Load(const PMDModelType _type);

    //  PMDModelTypeをキーとしてmap配列に格納するデータ
    struct DrawData
    {
        int drawNum;        //  1ループにおける描画数
        struct PMDModelData* modelData; //  モデルのデータ構造体へのポインタ
        class PMDModelTransform* transformClass;      //  座標変換を行うクラスへのポインタ
    };
    std::unordered_map<PMDModelType,struct DrawData> m_drawData;

    //  PMDモデルの描画命令を発行するクラスへのポインタ
    class PMDDrawer* m_drawer;
};