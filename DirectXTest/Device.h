#pragma once
#include <tchar.h>
#include <DirectXTex.h>
#include <wrl.h>

//  DXTK12
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>

#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

class Dx12Wrapper;
class PMDRenderer;
//  デバイスの設定を行うクラス
//  シングルトンで扱う
class Device
{
public:
    Device();
    ~Device();

    //  初期化
    bool Init();

    //  デバイス
    std::shared_ptr<Dx12Wrapper> dx12;
    //  レンダラー
    std::shared_ptr<PMDRenderer> pmdRenderer;

    //  ウィンドウハンドル
    HWND hwnd;

    //  フォント用ヒープ
    ComPtr<ID3D12DescriptorHeap> heapForSpriteFont;

    //  グラフィックスメモリオブジェクト
    GraphicsMemory* gmemory = nullptr;
    //  フォント表示用オブジェクト
    SpriteFont* spriteFont = nullptr;
    //  スプライト表示用オブジェクト
    SpriteBatch* spriteBatch = nullptr;

    //  後処理
    void Finalize();

    //  ウィンドウのサイズのゲッター
    SIZE GetWindowSize()const;

private:
    //  ウィンドウクラス周り
    WNDCLASSEX m_windowClass = {};

    //  ゲーム用ウィンドウの生成
    void CreateGameWindow(HWND& _hwnd, WNDCLASSEX& _windowClass);
};