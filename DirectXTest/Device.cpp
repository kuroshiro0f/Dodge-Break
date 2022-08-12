#include "Device.h"

#include <Keyboard.h>

#include "Dx12Wrapper.h"
#include "PMDRenderer.h"

//  ウィンドウ定数
const unsigned int WINDOW_WIDTH = 1920;
const unsigned int WINDOW_HEIGHT = 1080;

Device::Device()
{
    //  処理なし
}

Device::~Device()
{
    //  処理なし
}

//  ウィンドウに送信されたメッセージの処理
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    //  ウィンドウが破棄されたら呼ばれる
    if (msg == WM_DESTROY) {
        //  OSに対して「もうこのアプリは終わる」と伝える
        PostQuitMessage(0);
        return 0;
    }
    //  キーボードを使用するためのメッセージループ処理
    Keyboard::ProcessMessage(msg, wparam, lparam);
    //  規定の処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

//  初期化
bool Device::Init()
{
    //  現在のスレッドにマルチスレッドアパートメントを設定
    auto result = CoInitializeEx(0, COINIT_MULTITHREADED);

    //  ゲーム用のウィンドウの作成
    CreateGameWindow(hwnd, m_windowClass);

    //  DirectX12Wrapper初期化
    dx12.reset(new Dx12Wrapper(hwnd));
    //  PMDRenederer初期化
    pmdRenderer.reset(new PMDRenderer(*dx12));
    pmdRenderer->Init();

    //  フォントの設定
    //  GraphicsMemoryクラスの初期化
    gmemory = new GraphicsMemory(dx12->GetDevice().Get());
    //  GPUメモリにリソースをアップロードするためのヘルパーを初期化
    ResourceUploadBatch resUploadBatch(dx12->GetDevice().Get());
    //  アップロードするフォントのリソースの設定開始
    resUploadBatch.Begin();
    //  フォント表示用にPipelineを設定
    RenderTargetState rtState(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);
    SpriteBatchPipelineStateDescription pd(rtState);
    //  画像描画クラスの初期化
    //  NOTE: フォントも画像であるため、その処理に必要
    spriteBatch = new SpriteBatch(dx12->GetDevice().Get(), resUploadBatch, pd);
    //  ゲーム内で使用する文字のフォントを設定
    heapForSpriteFont = dx12->CreateDescriptorHeapForSpriteFont();
    spriteFont = new SpriteFont(
        dx12->GetDevice().Get(),
        resUploadBatch,
        L"Data/Font/AgencyFB.spritefont",   //  AgencyFBフォントを使用
        heapForSpriteFont->GetCPUDescriptorHandleForHeapStart(),
        heapForSpriteFont->GetGPUDescriptorHandleForHeapStart());
    //  GPUメモリにフォント情報を非同期でアップロードする命令を発行
    auto future = resUploadBatch.End(dx12->GetCmdQue());
    //  CommandQueueの実行を待機
    dx12->WaitForCommandQueue();
    //  リソースのアップロードを待機
    future.wait();
    //  フォントの描画範囲をViewportに合わせる
    spriteBatch->SetViewport(dx12->GetViewPort());

    return true;
}

//  終了処理
void Device::Finalize()
{
    //  もうクラスは使わないから登録解除
    UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}

//  ウィンドウのサイズのゲッター
SIZE Device::GetWindowSize() const
{
    SIZE ret;
    ret.cx = WINDOW_WIDTH;
    ret.cy = WINDOW_HEIGHT;
    return ret;
}

//  ゲーム用ウィンドウの生成
void Device::CreateGameWindow(HWND& _hwnd, WNDCLASSEX& _windowClass)
{
    HINSTANCE hInst = GetModuleHandle(nullptr);
    //  ウィンドウクラス生成＆登録
    _windowClass.cbSize = sizeof(WNDCLASSEX);
    _windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;    //  コールバック関数の指定
    _windowClass.lpszClassName = _T("DirectXTest");         //  アプリケーションクラス名
    _windowClass.hInstance = GetModuleHandle(0);            //  ハンドルの取得
    RegisterClassEx(&_windowClass);    //  ウィンドウクラスを登録

    RECT wrc = { 0,0, WINDOW_WIDTH, WINDOW_HEIGHT };       //  ウィンドウサイズを決める
    AdjustWindowRect(&wrc, WS_POPUP | WS_BORDER, false);    //  ウィンドウのサイズは関数を使って補正
    //  ウィンドウオブジェクトの生成
    _hwnd = CreateWindow(_windowClass.lpszClassName,    //  クラス名指定
        _T("Game"),    //  タイトルバーの文字
        WS_POPUP | WS_BORDER,    //  全画面
        CW_USEDEFAULT,    //  表示X座標はOSにお任せします
        CW_USEDEFAULT,    //  表示Y座標はOSにお任せします
        wrc.right - wrc.left,    //  ウィンドウ幅
        wrc.bottom - wrc.top,    //  ウィンドウ高
        nullptr,    //  親ウィンドウハンドル
        nullptr,    //  メニューハンドル
        _windowClass.hInstance,    //  呼び出しアプリケーションハンドル
        nullptr);    //  追加パラメータ
}
