#include "PMDModel.h"

#include <vector>

#include "EnumIterator.h"
#include "PMDDrawData.h"
#include "PMDModelType.h"
#include "PMDModelData.h"
#include "PMDDrawer.h"
#include "PMDLoader.h"
#include "PMDShifter.h"

PMDModel::PMDModel()
    :m_drawClassIndexNumber(0)
    , m_drawData({})
    , m_loader({})
    , m_shifter({})
    , m_drawer({})
{
}

PMDModel::~PMDModel()
{
    //  メモリの解放
    m_drawData.clear();
    m_loader.clear();
    m_shifter.clear();
    m_shifter.shrink_to_fit();
    m_drawer.clear();
    m_drawer.shrink_to_fit();
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
    }
}

//  更新
void PMDModel::Update()
{
    //  次のループ用に初期化
    //  NOTE: 座標変換と描画を行うクラスを配列化しており、
    //        ループごとに配列の先頭にあるクラスから描画処理を行うために、
    //        配列の添え字に指定するこの変数を毎ループ0で初期化しています。
    m_drawClassIndexNumber = 0;
}

//  描画
void PMDModel::Draw(const XMFLOAT3& _pos, const PMDModelType _type)
{
    //  座標変換用の情報を作成
    PMDShifter* temp = m_shifter[m_drawClassIndexNumber];
    temp->Update(_pos);
    //  描画用の情報を作成
    m_drawer[m_drawClassIndexNumber]->Draw(*m_drawData[_type], temp->GetTransformHeap());
    //  次の描画用に番号を加算
    m_drawClassIndexNumber++;
}
void PMDModel::Draw(const XMFLOAT3& _pos, const float _angle, const PMDModelType _type)
{
    //  座標変換用の情報を作成
    PMDShifter* temp = m_shifter[m_drawClassIndexNumber];
    temp->Update(_pos,_angle);
    //  描画用の情報を作成
    m_drawer[m_drawClassIndexNumber]->Draw(*m_drawData[_type], temp->GetTransformHeap());
    //  次の描画用に番号を加算
    m_drawClassIndexNumber++;
}

//  拡大率と座標を変更して描画
void PMDModel::DrawChangePosAndScale(const XMFLOAT3& _pos, const float _scale, const PMDModelType _type)
{
      //  座標変換用の情報を作成
    PMDShifter* temp = m_shifter[m_drawClassIndexNumber];
    temp->UpdatePosAndScale(_pos, _scale);
    //  描画用の情報を作成
    m_drawer[m_drawClassIndexNumber]->Draw(*m_drawData[_type], temp->GetTransformHeap());
    //  次の描画用に番号を加算
    m_drawClassIndexNumber++;
}

//  モデルを描画するための設定
void PMDModel::SetUp(const int _modelNum)
{
    //  指定数クラスのインスタンスを生成
    for (int i = 0; i < _modelNum; i++)
    {
        m_shifter.emplace_back(new PMDShifter());
        m_shifter.back()->Init();
        m_drawer.emplace_back(new PMDDrawer());
    }
}

//  ロード
void PMDModel::Load(const PMDModelType _type)
{
    PMDDrawData* tempData = new PMDDrawData();
    PMDLoader* tempLoader = new PMDLoader();
    //  モデルのファイルから情報をロード
    tempLoader->LoadModel(PMDModelData::FILE_PATH[_type].c_str());
    //  読み込んだ情報を構造体にコピーして保存
    tempData->vbView = tempLoader->GetVbView();
    tempData->ibView = tempLoader->GetIbView();
    tempData->materials = tempLoader->GetMaterials();
    tempData->materialHeap = tempLoader->GetMaterialHeap();
    m_drawData[_type] = tempData;
    m_loader[_type] = tempLoader;
}
