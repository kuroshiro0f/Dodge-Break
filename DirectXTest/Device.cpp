#include "Device.h"

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

//  必須のやつ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    //  ウィンドウが破棄されたら呼ばれる
    if (msg == WM_DESTROY) {
        //  OSに対して「もうこのアプリは終わる」と伝える
        PostQuitMessage(0);
        return 0;
    }
    //  規定の処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

//  初期化
bool Device::Init()
{
    auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
    CreateGameWindow(hwnd, m_windowClass);

    //  DirectX12ラッパー生成＆初期化
    dx12.reset(new Dx12Wrapper(hwnd));
    //  PMDRenederer初期化
    pmdRenderer.reset(new PMDRenderer(*dx12));
    pmdRenderer->Init();

    //  GraphicsMemoryオブジェクトの初期化
    gmemory = new GraphicsMemory(dx12->GetDevice().Get());

    //  SpriteBatchMemoryオブジェクトの初期化
    ResourceUploadBatch resUploadBatch(dx12->GetDevice().Get());
    resUploadBatch.Begin();
    RenderTargetState rtState(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);
    SpriteBatchPipelineStateDescription pd(rtState);
    spriteBatch = new SpriteBatch(dx12->GetDevice().Get(), resUploadBatch, pd);

    //  SpriteFontオブジェクトの初期化
    heapForSpriteFont = dx12->CreateDescriptorHeapForSpriteFont();
    spriteFont = new SpriteFont(
        dx12->GetDevice().Get(),
        resUploadBatch,
        L"Data/Font/AgencyFB.spritefont",
        heapForSpriteFont->GetCPUDescriptorHandleForHeapStart(),
        heapForSpriteFont->GetGPUDescriptorHandleForHeapStart());

    auto future = resUploadBatch.End(dx12->GetCmdQue());        //  転送

    //  待ち
    dx12->WaitForCommandQueue();
    future.wait();
    spriteBatch->SetViewport(dx12->GetViewPort());

    return true;
}

//  後処理
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

void Device::CreateGameWindow(HWND& _hwnd, WNDCLASSEX& _windowClass)
{
    HINSTANCE hInst = GetModuleHandle(nullptr);
    //  ウィンドウクラス生成＆登録
    _windowClass.cbSize = sizeof(WNDCLASSEX);
    _windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;    //  コールバック関数の指定
    _windowClass.lpszClassName = _T("DirectXTest");         //  アプリケーションクラス名
    _windowClass.hInstance = GetModuleHandle(0);            //  ハンドルの取得
    RegisterClassEx(&_windowClass);    //  アプリケーションクラス(こういうの作るからよろしくってOSに予告する)

    RECT wrc = { 0,0, WINDOW_WIDTH, WINDOW_HEIGHT };       //  ウィンドウサイズを決める
    AdjustWindowRect(&wrc, WS_POPUP | WS_BORDER, false);    //  ウィンドウのサイズはちょっと面倒なので関数を使って補正する
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
        _windowClass.hInstance,    //  呼び出しアプリケーションハンドルaa
        nullptr);    //  追加パラメータ
}
