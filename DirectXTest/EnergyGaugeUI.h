#pragma once
#include <memory>

//  エネルギーゲージUIを描画するクラス
class EnergyGaugeUI
{
public:
    EnergyGaugeUI();
    ~EnergyGaugeUI();

    //  初期化
    void Init(class IPlayerParameterGetter* _playerParameterGetter, const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  更新
    void Update();

    //  描画
    void Draw();

    //  エネルギーゲージをリセットする
    void ResetEnergyGauge();
private:
    //  エネルギーゲージのアニメーション
    void PlayAnimationEnergyGaugeIncrease();

    //  表示上のエネルギー量
    float m_displayEnergyNum;
    //  実際に溜まっているエネルギー量
    float m_actualEnergyNum;
    //  エネルギーが発光中か
    bool m_isFlashEnergy;
    //  エネルギーゲージは増加したか
    bool m_isIncreaseEnergy;

    //  エネルギーを得るためのクラスへのポインタ
    IPlayerParameterGetter* m_playerParameterGetter;

    //  画像描画クラスのシングルトンインスタンスを持つための変数
    class SpriteDrawer& m_sprite;
};