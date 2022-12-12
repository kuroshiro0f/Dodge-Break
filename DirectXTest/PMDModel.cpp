#include "PMDModel.h"

#include "EnumIterator.h"
#include "PMDModelData.h"
#include "PMDModelFilePathData.h"
#include "PMDModelType.h"
#include "PMDModelData.h"
#include "PMDDrawer.h"
#include "PMDLoader.h"
#include "PMDModelTransform.h"

PMDModel::PMDModel()
    :m_drawData({})
    , m_drawer(new PMDDrawer())
{
    //  処理なし
}

PMDModel::~PMDModel()
{
    //  メモリの解放
    m_drawData.clear();
}

//  初期化
void PMDModel::Init()
{
    //  列挙型PMDModelTypeのイテレータを作成
    typedef EnumIterator<PMDModelType, PMDModelType::Player, PMDModelType::BackGround> typeItr;

    //  モデルを全てロード
    for (auto itr : typeItr())
    {
        Load(itr);
        m_drawData[itr].drawNum = 0;
    }
}

//  1ループ中の描画命令の一斉発行
void PMDModel::IssueAllDrawCommand()
{
    //  列挙型PMDModelTypeのイテレータを作成
    typedef EnumIterator<PMDModelType, PMDModelType::Player, PMDModelType::BackGround> typeItr;
    //  各種モデルの描画命令を発行
    for (auto itr : typeItr())
    {
        //  使いまわす値を変数に格納
        DrawData* drawData = &m_drawData[itr];
        int* drawNum = &drawData->drawNum;

        //  現在のループで描画を行わないモデルの描画命令の発行処理をスキップ
        if (*drawNum == 0)
        {
            continue;
        }
        //  描画命令を発行
        m_drawer->IssueDrawCommand(*drawData->modelData, drawData->transformClass->GetTransformHeap(), *drawNum);

        //  次のループのために描画数をリセット
        *drawNum = 0;
    }
}

//  座標を変更して描画
void PMDModel::Draw(const XMFLOAT3& _pos, const PMDModelType _type)
{
    //  使いまわす値を変数に格納
    DrawData* drawData = &m_drawData[_type];

    //  指定されたモデルの座標変換用の情報を作成
    PMDModelTransform* tempTransformClass = drawData->transformClass;
    tempTransformClass->Update(_pos, drawData->drawNum);

    //  指定されたモデルの描画数を増やす
    drawData->drawNum++;
}
//  座標と角度を変更して描画
void PMDModel::Draw(const XMFLOAT3& _pos, const float _angle, const PMDModelType _type)
{
    //  使いまわす値を変数に格納
    DrawData* drawData = &m_drawData[_type];

    //  指定されたモデルの座標変換用の情報を作成
    PMDModelTransform* tempTransformClass = drawData->transformClass;
    tempTransformClass->Update(_pos,_angle, drawData->drawNum);

    //  指定されたモデルの描画数を増やす
    drawData->drawNum++;
}

//  拡大率と座標を変更して描画
void PMDModel::DrawChangePosAndScale(const XMFLOAT3& _pos, const float _scale, const PMDModelType _type)
{
    //  使いまわす値を変数に格納
    DrawData* drawData = &m_drawData[_type];

    //  指定されたモデルの座標変換用の情報を作成
    PMDModelTransform* tempTransformClass = drawData->transformClass;
    tempTransformClass->UpdatePosAndScale(_pos, _scale, drawData->drawNum);

    //  指定されたモデルの描画数を増やす
    drawData->drawNum++;
}

//  ロード
void PMDModel::Load(const PMDModelType _type)
{
    //  各種インスタンスを一時的な変数に取得
    PMDModelData* tempData = new PMDModelData();
    PMDModelTransform* tempTransformClass = new PMDModelTransform();
    PMDLoader* loader = new PMDLoader();

    //  モデルのファイルから情報をロード
    loader->LoadModel(PMDModelFilePathData::FILE_PATH[_type].c_str());
    //  読み込んだ情報を構造体に保存
    tempData->vbView = loader->GetVbView();     //  頂点情報
    tempData->ibView = loader->GetIbView();     //  Index情報(頂点のつなぎ方など)
    tempData->materials = loader->GetMaterials();   //  マテリアルの情報
    tempData->materialHeap = loader->GetMaterialHeap();     //  マテリアルの情報の扱い方を知るためのDescriptorHeap

    //  TransformPMDModelClassを初期化
    tempTransformClass->Init();

    //  各種インスタンスを配列に格納
    m_drawData[_type].modelData = tempData;
    m_drawData[_type].transformClass = tempTransformClass;
}
