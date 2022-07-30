#include "EffekseerManager.h"

#include <string>

#include "EnumIterator.h"
#include "EffectData.h"
#include "EffectType.h"
#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "CheckError.h"
#include "Device.h"

//  画面上に一度に表示する最大エフェクト数
static const int DISPLAY_EFFECT_NUM_MAX = 500;

//  画面上に一度に表示する最大パーティクル数
static const int DESPLAY_PARTICLE_NUM_MAX = 10000;

EffekseerManager::EffekseerManager()
    :m_efkRenderer(nullptr)
    , m_efkMgr(nullptr)
    , m_efkMemoryPool(nullptr)
    , m_efkCmdList(nullptr)
    , m_effect({})
    , m_loopPlayData({})
    , m_device(Singleton<Device>::GetInstance())
    , m_error(Singleton<CheckError>::GetInstance())
{
    //  処理なし
}

EffekseerManager::~EffekseerManager()
{
    //  処理なし
}

//  初期化
void EffekseerManager::Init()
{
    //  Effekseerレンダラーの初期化
    DXGI_FORMAT bbFormats[] = { DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM };
    m_efkRenderer = EffekseerRendererDX12::Create(
        m_device.dx12->GetDevice().Get(),    //  DirectX12のデバイス
        m_device.dx12->GetCmdQue(),          //  DirectX12のコマンドキュー
        2,                                   //  バックバッファーの数
        bbFormats,                           //  レンダーターゲットフォーマット
        1,                                   //  レンダーターゲット数
        DXGI_FORMAT_UNKNOWN,                 //  デプスはなし
        false,                               //  反対デプスはなし
        DESPLAY_PARTICLE_NUM_MAX);                              //  最大パーティクルの数

    //  Effekseerマネージャーの初期化
    m_efkMgr = Effekseer::Manager::Create(DISPLAY_EFFECT_NUM_MAX);

    //  エフェクトの設定
    SetUpEffect();
    //  DirectX12用の初期化
    DX12RendererInit();

    //  列挙型EffectTypeのイテレータを作成
    typedef EnumIterator<EffectType, EffectType::AppearancePlayer, EffectType::ActivateUniqueShootEnemyTower> typeItr;

    //  エフェクトの種類の合計数だけロードをする。
    for (auto itr : typeItr())
    {
        //  二重ロードの防止
        for (auto secondItr : typeItr())
        {
            if (itr != secondItr && EffectData::PATH_MAP[itr] == EffectData::PATH_MAP[secondItr])
            {
                std::string str = "エフェクト : " + std::to_string(static_cast<int>(itr)) + "のファイルは二重にロードされています。";
                m_error.CreateErrorMessage(str);
            }
        }
        LoadEffect(itr);
    }
}

//  エフェクトの設定
void EffekseerManager::SetUpEffect()
{
    //  座標系を左手系にする(クライアント側の系に合わせる)
    m_efkMgr->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

    //  描画用インスタンスから描画機能を設定
    m_efkMgr->SetSpriteRenderer(m_efkRenderer->CreateSpriteRenderer());
    m_efkMgr->SetRibbonRenderer(m_efkRenderer->CreateRibbonRenderer());
    m_efkMgr->SetRingRenderer(m_efkRenderer->CreateRingRenderer());
    m_efkMgr->SetTrackRenderer(m_efkRenderer->CreateTrackRenderer());
    m_efkMgr->SetModelRenderer(m_efkRenderer->CreateModelRenderer());
    

    //  描画用インスタンスからテクスチャの読み込み機能を設定
    //  独自拡張も可能
    m_efkMgr->SetTextureLoader(m_efkRenderer->CreateTextureLoader());
    m_efkMgr->SetModelLoader(m_efkRenderer->CreateModelLoader());
    m_efkMgr->SetMaterialLoader(m_efkRenderer->CreateMaterialLoader());
}

//  DirectX12のレンダラー用の初期化
void EffekseerManager::DX12RendererInit()
{
    //  メモリプールの設定
    m_efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(m_efkRenderer->GetGraphicsDevice());
    //  コマンドリストにEffekseerレンダラーの情報も追加
    m_efkCmdList = EffekseerRenderer::CreateCommandList(m_efkRenderer->GetGraphicsDevice(), m_efkMemoryPool);
    m_efkRenderer->SetCommandList(m_efkCmdList);
}

//  ロード
void EffekseerManager::LoadEffect(const EffectType _type)
{
    //  ファイルからエフェクト情報を読み取る
    m_effect[_type] = Effekseer::Effect::Create(
        m_efkMgr,
        (const EFK_CHAR*)EffectData::PATH_MAP[_type],
        EffectData::MAGNIFICATION_MAP[_type],
        (const EFK_CHAR*)EffectData::FILE_MAP[_type]);
    //  エラーチェック
    if (m_effect[_type] == nullptr)
    {
        std::string str = "エフェクト : " + std::to_string(static_cast<int>(_type)) + " の読み取り中にエラーが発生しました。";
        m_error.CreateErrorMessage(str);
    }
}

//  再生
//  NOTE: エフェクトをループ再生する場合、この関数によって返される値を
//        int型変数に保持し、エフェクトハンドルとして、
//        更新や停止に利用する
int EffekseerManager::PlayEffect(const EffectType _type, const XMFLOAT3& _pos, bool _isLoop, const XMFLOAT3& _angle)
{
    //  指定された座標に指定されたタイプのエフェクトを再生
    //  ループ再生するなら配列にデータを格納して保持する
    if (_isLoop)
    {
        LoopPlayData data;
        data.type = _type;
        data.handle = m_efkMgr->Play(m_effect[_type], _pos.x, _pos.y, _pos.z);
        m_loopPlayData.emplace_back(data);

        //  角度の変更
        m_efkMgr->SetRotation(data.handle, _angle.x, _angle.y, _angle.z);

        return data.handle;
    }
    else
    {
        int temp = m_efkMgr->Play(m_effect[_type], _pos.x, _pos.y, _pos.z);

        //  角度の変更
        m_efkMgr->SetRotation(temp, _angle.x, _angle.y, _angle.z);

        return temp;
    }
}

//  停止
void EffekseerManager::StopEffect(int _handle)
{
    //  指定されたエフェクトを停止
    m_efkMgr->StopEffect(_handle);
    //  ループ再生中のエフェクトデータから削除
    for (auto itr = m_loopPlayData.begin(); itr != m_loopPlayData.end(); itr++)
    {
        if (itr->handle == _handle)
        {
            m_loopPlayData.erase(itr);
            return;
        }
    }
}

//  更新
void EffekseerManager::Draw()
{
    //  エフェクト描画
    m_efkMgr->Update();                //  マネージャーの更新(時間更新)
    m_efkMemoryPool->NewFrame();       //  描画すべきレンダーターゲットを選択

    //  描画処理の開始
    EffekseerRendererDX12::BeginCommandList(m_efkCmdList, m_device.dx12->GetCommandList().Get());

    //  描画前処理
    m_efkRenderer->BeginRendering();
    //  エフェクト描画
    m_efkMgr->Draw();
    //  描画後処理
    m_efkRenderer->EndRendering();

    //  描画処理終了
    EffekseerRendererDX12::EndCommandList(m_efkCmdList);
}

//  エフェクトの更新
//  NOTE: ループ再生する場合は、初めからエフェクトを再生しなおす度にハンドルの値が変わるため、
//        int型変数を返すようにして、エフェクトハンドルの値も更新できるようにする
int EffekseerManager::UpdateLoopPlayEffect(const XMFLOAT3& _pos, const int _handle, const float _deltaTime, const XMFLOAT3& _angle)
{
    //  ハンドルによって指定されたエフェクトがループ再生中か検索
    for (auto itr = m_loopPlayData.begin(); itr != m_loopPlayData.end(); itr++)
    {
        //  ループ再生中且つエフェクトの再生が終わっていたら、最初から再生しなおす。
        if (itr->handle == _handle && !m_efkMgr->Exists(_handle))
        {
            //  再生
            itr->handle = m_efkMgr->Play(m_effect[itr->type], _pos.x, _pos.y, _pos.z);
            //  角度の変更
            m_efkMgr->SetRotation(itr->handle, _angle.x, _angle.y, _angle.z);

            return itr->handle;
        }
    }
    //  位置の変更
    m_efkMgr->SetLocation(_handle, _pos.x, _pos.y, _pos.z);
    //  角度の変更
    m_efkMgr->SetRotation(_handle, _angle.x, _angle.y, _angle.z);
    //  速度をデルタタイムに合わせる
    m_efkMgr->SetSpeed(_handle, _deltaTime);
    return _handle;
}

//  カメラにエフェクトの位置を同期
void EffekseerManager::SyncronizeEffekseerCamera()
{
    //  Effekseer用ビュー行列
    Effekseer::Matrix44 fkViewMat;
    //  Effekseer用プロジェクション行列
    Effekseer::Matrix44 fkProjMat;

    //  デバイスのビュー行列とプロジェクション行列をコピー
    auto view = m_device.dx12->GetViewMatrix();
    auto proj = m_device.dx12->GetProjMatrix();

    //  Effekseer用とデバイス上のそれぞれの行列を同期
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            fkViewMat.Values[i][j] = view.r[i].m128_f32[j];
            fkProjMat.Values[i][j] = proj.r[i].m128_f32[j];
        }
    }

    //  それぞれの行列に従ってカメラを設定
    m_efkRenderer->SetCameraMatrix(fkViewMat);
    m_efkRenderer->SetProjectionMatrix(fkProjMat);
}
