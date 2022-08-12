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
//  Deviceを保持し、ウィンドウを作成するクラス
//  シングルトンで扱う
class Device
{
public:
    Device();
    ~Device();

    //  初期化
    bool Init();

    //  Device
    std::shared_ptr<Dx12Wrapper> dx12;
    //  Renderer
    std::shared_ptr<PMDRenderer> pmdRenderer;

    //  ウィンドウハンドル
    HWND hwnd;

    //  フォント用DescriptorHeap
    ComPtr<ID3D12DescriptorHeap> heapForSpriteFont;

    //  GraphicsMemoryクラスのインスタンスを保持するポインタ
    //  NOTE: GPUへのデータのアップロードにおいてメモリの割り当てを管理するクラス
    GraphicsMemory* gmemory = nullptr;
    //  フォント表示用オブジェクト
    SpriteFont* spriteFont = nullptr;
    //  Sprite表示用オブジェクト
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