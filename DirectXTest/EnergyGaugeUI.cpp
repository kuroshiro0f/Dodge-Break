#include "EnergyGaugeUI.h"

#include "SpriteType.h"
#include "EventNotificator.h"
#include "EventType.h"
#include "EnergyGaugeUIData.h"

#include "IPlayerParameterGetter.h"

#include "Singleton.h"
#include "SpriteDrawer.h"

EnergyGaugeUI::EnergyGaugeUI()
    : m_playerParameterGetter(nullptr)
    , m_actualEnergyNum(0)
    , m_displayEnergyNum(0)
    , m_isFlashEnergy(false)
    , m_sprite(Singleton<SpriteDrawer>::GetInstance())
{
    //  処理なし
}

EnergyGaugeUI::~EnergyGaugeUI()
{
    //  処理なし
}

//  初期化
void EnergyGaugeUI::Init(class IPlayerParameterGetter* _playerParameterGetter, const std::shared_ptr<class EventNotificator> _eventClass)
{
    //  プレイヤーのパラメータを取得する関数のポインタを取得
    m_playerParameterGetter = _playerParameterGetter;

    //  エネミーが倒されたときにエネルギーゲージをリセットするように登録
    _eventClass->Register(EventType::DeleteEnemy, std::bind(&EnergyGaugeUI::ResetEnergyGauge, this));
}

//  リセット
void EnergyGaugeUI::Reset()
{
    //  エネルギーゲージアニメーション用変数のリセット
    m_actualEnergyNum = 0;
    m_displayEnergyNum = 0;
}

//  更新
void EnergyGaugeUI::Update()
{
    //  エネルギー量の更新
    //  NOTE: エネルギーのゲージの増加量を微調整するためにfloatに変換
    m_actualEnergyNum = static_cast<float>(m_playerParameterGetter->GetEnergy());

    //  エネルギー量が増加したなら増加アニメーションを行う
    if (m_actualEnergyNum > m_displayEnergyNum)
    {
        PlayAnimationEnergyGaugeIncrease();
    }
}

//  描画
void EnergyGaugeUI::Draw()
{
    //  エネルギー量が増加したなら増加アニメーションを行う
    if (m_isIncreaseEnergy)
    {
        //  エネルギーゲージを明滅させる
        if (m_isFlashEnergy)
        {
            //  エネルギーの表示
            m_sprite.Draw(SpriteType::GaugeFrame, EnergyGaugeUIData::ENERGY_FRAME_POS, EnergyGaugeUIData::ENERGY_MAGNIFICATION);
            m_sprite.Draw(SpriteType::Gauge, EnergyGaugeUIData::ENERGY_POS, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, m_displayEnergyNum * EnergyGaugeUIData::ENERGY_AMOUNT_TO_PERCENTAGE, EnergyGaugeUIData::ENERGY_MAGNIFICATION, EnergyGaugeUIData::FLASH_COLOR);

            m_isFlashEnergy = false;
        }
        else
        {
            //  エネルギーの表示
            m_sprite.Draw(SpriteType::GaugeFrame, EnergyGaugeUIData::ENERGY_FRAME_POS, EnergyGaugeUIData::ENERGY_MAGNIFICATION);
            m_sprite.Draw(SpriteType::Gauge, EnergyGaugeUIData::ENERGY_POS, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, m_displayEnergyNum * EnergyGaugeUIData::ENERGY_AMOUNT_TO_PERCENTAGE, EnergyGaugeUIData::ENERGY_MAGNIFICATION);

            m_isFlashEnergy = true;
        }
        m_isIncreaseEnergy = false;
    }
    else
    {
        //  エネルギーの表示
        m_sprite.Draw(SpriteType::GaugeFrame, EnergyGaugeUIData::ENERGY_FRAME_POS, EnergyGaugeUIData::ENERGY_MAGNIFICATION);
        m_sprite.Draw(SpriteType::Gauge, EnergyGaugeUIData::ENERGY_POS, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, EnergyGaugeUIData::ENERGY_GAUGE_UI_RATE, m_displayEnergyNum * EnergyGaugeUIData::ENERGY_AMOUNT_TO_PERCENTAGE, EnergyGaugeUIData::ENERGY_MAGNIFICATION);
    }
}

//  エネルギーゲージをリセットする
void EnergyGaugeUI::ResetEnergyGauge()
{
    m_displayEnergyNum = 0;
}

//  エネルギーゲージのアニメーション
void EnergyGaugeUI::PlayAnimationEnergyGaugeIncrease()
{
    m_displayEnergyNum += EnergyGaugeUIData::ENERGY_ADD_SPEED;
    m_isIncreaseEnergy = true;
    if (m_displayEnergyNum >= m_actualEnergyNum)
    {
        m_displayEnergyNum = m_actualEnergyNum;
    }
}