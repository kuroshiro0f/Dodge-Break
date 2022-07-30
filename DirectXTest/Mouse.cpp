#include "Mouse.h"

#include "CoordinateTransformation.h"
#include "Dx12Wrapper.h"

#include "Singleton.h"
#include "Device.h"

//  マウスカーソルのID（標準矢印）
static const LPCSTR CURSOR_ID = IDC_ARROW;

//  最初のマウスの座標
static const XMINT2 FIRST_MOUSE_POS = { 960,540 };

Mouse::Mouse()
    :m_point({ 0,0 })
    , m_device(Singleton<Device>::GetInstance())
{
}

Mouse::~Mouse()
{
    //  処理なし
}

//  初期化
void Mouse::Init()
{
    SetCursorPos(FIRST_MOUSE_POS.x, FIRST_MOUSE_POS.y);
    SetCursor(LoadCursor(NULL, CURSOR_ID));
}

//  更新
void Mouse::Update()
{
    GetCursorPos(&m_point);
    ScreenToClient(m_device.hwnd, &m_point);
}

//  マウスのスクリーン座標を得る
XMFLOAT2 Mouse::GetMouseScreenPos()
{
    GetCursorPos(&m_point);
    ScreenToClient(m_device.hwnd, &m_point);
    return { static_cast<float>(m_point.x),static_cast<float>(m_point.y) };
}

//  マウスのワールド座標を得る
XMFLOAT3 Mouse::GetMouseWorldPos()
{
    XMFLOAT3 pos = CoordinateTransformation::CalcScreenPosToXZ({ static_cast<float>(m_point.x),static_cast<float>(m_point.y) },
        static_cast<float>(m_device.GetWindowSize().cx), static_cast<float>(m_device.GetWindowSize().cy),
        m_device.dx12->GetViewMatrix(), m_device.dx12->GetProjMatrix());
    return pos;
}