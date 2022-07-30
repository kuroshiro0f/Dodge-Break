#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <list>

#pragma comment(lib,"EffekseerRendererDX12.lib")
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"LLGI.lib")

using namespace DirectX;

enum class EffectType;
//  Effekseerのロードや再生を行うクラス
//  シングルトンで扱う
class EffekseerManager
{
public:
    EffekseerManager();
    ~EffekseerManager();

    //  初期化
    void Init();

    //  エフェクトの再生
    //  NOTE: エフェクトをループ再生する場合、この関数によって返される値を
    //        int型変数に保持し、エフェクトハンドルとして、
    //        更新や停止に利用する
    int PlayEffect(const EffectType _type, const XMFLOAT3& _pos, bool _isLoop, const XMFLOAT3& _angle = { 0.0f,0.0f,0.0f });

    //  エフェクトの停止
    void StopEffect(int _handle);

    //  描画
    void Draw();

    //  エフェクトの更新
    //  NOTE: ループ再生する場合は、初めからエフェクトを再生しなおす度にハンドルの値が変わるため、
    //        int型変数を返すようにして、エフェクトハンドルの値も更新できるようにする
    //  NOTE: この関数を呼ぶのは、ループ再生しているエフェクトがある場合のみ
    int UpdateLoopPlayEffect(const XMFLOAT3& _pos, const int _handle,const float _deltaTime, const XMFLOAT3& _angle = { 0.0f,0.0f,0.0f });

    //  カメラとEffekseerの表示場所の同期
    void SyncronizeEffekseerCamera();
private:
    //  エフェクト発生に必要な設定
    void SetUpEffect();

    //  DirectX12専用のRendererの初期化
    void DX12RendererInit();

    //  エフェクトファイルの読み込み
    void LoadEffect(const EffectType _type);

    //  エフェクトマネージャー
    Effekseer::RefPtr<Effekseer::Manager> m_efkMgr;
    //  エフェクトレンダラー
    Effekseer::RefPtr<EffekseerRenderer::Renderer> m_efkRenderer;
    //  メモリプール
    Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> m_efkMemoryPool;
    //  コマンドリスト
    Effekseer::RefPtr<EffekseerRenderer::CommandList> m_efkCmdList;
    //  エフェクト本体
    std::unordered_map<EffectType, Effekseer::RefPtr<Effekseer::Effect>> m_effect;

    //  ループ再生中のエフェクトのデータ
    struct LoopPlayData
    {
        Effekseer::Handle handle;
        EffectType type;
    };
    std::list<LoopPlayData> m_loopPlayData;

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
    class CheckError& m_error;
};