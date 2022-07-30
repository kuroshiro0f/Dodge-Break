#include "SpriteDrawer.h"

#include <CommonStates.h>
#include <WICTextureLoader.h>

#include "EnumIterator.h"
#include "SpriteData.h"
#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "CheckError.h"
#include "Device.h"

SpriteDrawer::SpriteDrawer()
    :m_device(Singleton<Device>::GetInstance())
    , m_error(Singleton<CheckError>::GetInstance())
    , m_resourceDescriptors(nullptr)
    , m_texture({})
    , m_spriteBatch(nullptr)
{
}

SpriteDrawer::~SpriteDrawer()
{
    //  メモリの解放
    m_texture.clear();
    m_texture.shrink_to_fit();
    m_resourceDescriptors.reset();
    m_spriteBatch.reset();
}

//  初期化
void SpriteDrawer::Init()
{
    //  DescriptorHeapの初期化
    m_resourceDescriptors = std::make_unique<DescriptorHeap>(m_device.dx12->GetDevice().Get(), SpriteType::EndDescriptors);
    //  ロード数の指定
    m_texture.resize(static_cast<int>(SpriteType::EndDescriptors));
    //  ロードする
    Load();
}

//  描画
void SpriteDrawer::Draw(const SpriteType _handle, const XMFLOAT2& _pos, const float _magnification, const XMVECTOR& _color)
{
    //  DescriptorHeapの設定
    ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap() };
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);

    //  サイズを調整
    XMUINT2 size = { static_cast<UINT>(GetTextureSize(m_texture[static_cast<int>(_handle)].Get()).x * _magnification),static_cast<UINT>(GetTextureSize(m_texture[static_cast<int>(_handle)].Get()).y * _magnification) };

    //  描画する
    m_spriteBatch->Begin(m_device.dx12->GetCommandList().Get());
    m_spriteBatch->Draw(m_resourceDescriptors->GetGpuHandle(static_cast<size_t>(_handle)), size, _pos, _color);
    m_spriteBatch->End();
}

//  描画(画像の描画範囲を指定する場合)
void SpriteDrawer::Draw(const SpriteType _handle, const XMFLOAT2& _pos
    , const float _topMagnification, const float _leftMagnification, const float _bottomMagnification, const float _rightMagnification
    , const float _magnification, const XMVECTOR& _color)
{
    //  DescriptorHeapの設定
    ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap() };
    m_device.dx12->GetCommandList()->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);

    //  サイズを調整
    XMUINT2 size = { static_cast<UINT>(GetTextureSize(m_texture[static_cast<int>(_handle)].Get()).x * _magnification),static_cast<UINT>(GetTextureSize(m_texture[static_cast<int>(_handle)].Get()).y * _magnification) };

    //  描画範囲指定
    //  NOTE: 画像の何割を表示するかを上下左右四方向から指定
    RECT rect;
    rect.top = static_cast<LONG>(size.y - _topMagnification * size.y);
    rect.left = static_cast<LONG>(size.x - _leftMagnification * size.x);
    rect.bottom = static_cast<LONG>(_bottomMagnification * size.y);
    rect.right = static_cast<LONG>(_rightMagnification * size.x);

    //  描画する
    m_spriteBatch->Begin(m_device.dx12->GetCommandList().Get());
    m_spriteBatch->Draw(m_resourceDescriptors->GetGpuHandle(static_cast<size_t>(_handle)), size, _pos, &rect, _color);
    m_spriteBatch->End();
}

//  ロード
int SpriteDrawer::Load()
{
    //  SpriteBatchオブジェクトの初期化
    ResourceUploadBatch resourceUpload(m_device.dx12->GetDevice().Get());
    resourceUpload.Begin();
    RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);
    SpriteBatchPipelineStateDescription pd(rtState, &CommonStates::NonPremultiplied);
    m_spriteBatch = std::make_unique<SpriteBatch>(m_device.dx12->GetDevice().Get(), resourceUpload, pd);

    //  列挙型SpriteTypeのイテレータを作成
    typedef EnumIterator<SpriteType, SpriteType::TitleBack, SpriteType::FadeOutBack> typeItr;

    for (auto itr : typeItr())
    {
        //  二重ロードの防止
        for (auto secondItr : typeItr())
        {
            if (itr != secondItr && SpriteData::FILE_PATH[itr] == SpriteData::FILE_PATH[secondItr])
            {
                std::string str = "画像 : " + std::to_string(static_cast<int>(itr)) + "のファイルは二重にロードされています。";
                m_error.CreateErrorMessage(str);
            }
        }
        //  テクスチャファイル読み込み
        if (FAILED(CreateWICTextureFromFile(m_device.dx12->GetDevice().Get(),
            resourceUpload,
            SpriteData::FILE_PATH[itr],
            m_texture[static_cast<int>(itr)].ReleaseAndGetAddressOf())))
        {
            assert(0);
            return -1;
        }
        //  シェーダーリソースビューの作成
        CreateShaderResourceView(m_device.dx12->GetDevice().Get(), m_texture[static_cast<int>(itr)].Get(), m_resourceDescriptors->GetCpuHandle(static_cast<size_t>(itr)));
    }

    //  SpriteBatchオブジェクトの後処理
    auto uploadResourcesFinished = resourceUpload.End(m_device.dx12->GetCmdQue());
    uploadResourcesFinished.wait();

    //  SpriteBatchにビューポートをセット
    m_spriteBatch->SetViewport(m_device.dx12->GetViewPort());
}