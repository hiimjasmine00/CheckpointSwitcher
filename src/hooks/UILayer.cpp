#include "UILayer.hpp"
#include "PlayLayer.hpp"
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

void CSUILayer::onModify(ModifyBase<ModifyDerive<CSUILayer, UILayer>>& self) {
    jasmine::hook::modify(self.m_hooks, "enabled");
}

bool CSUILayer::init(GJBaseGameLayer* layer) {
    if (!UILayer::init(layer)) return false;

    auto playLayer = typeinfo_cast<PlayLayer*>(layer);
    if (!playLayer) return true;

    auto winSize = CCDirector::get()->getWinSize();
    auto f = m_fields.self();

    f->m_switchMenu = CCMenu::create();
    f->m_switchMenu->setPosition({ winSize.width / 2.0f, jasmine::setting::getValue<float>("menu-y-offset") });
    f->m_switchMenu->setVisible(layer->m_isPracticeMode && !jasmine::setting::getValue<bool>("hide-menu"));
    f->m_switchMenu->setID("switch-menu"_spr);
    addChild(f->m_switchMenu);

    f->m_prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    f->m_prevSprite->setScale(0.6f);
    f->m_prevButton = CCMenuItemSpriteExtra::create(f->m_prevSprite, this, menu_selector(CSUILayer::onPrev));
    f->m_prevButton->setID("prev-button"_spr);
    f->m_switchMenu->addChild(f->m_prevButton);

    f->m_switchLabel = CCLabelBMFont::create("0/0", "bigFont.fnt");
    f->m_switchLabel->setScale(0.6f);
    f->m_switchLabel->setID("switch-label"_spr);
    f->m_switchMenu->addChild(f->m_switchLabel);

    f->m_nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    f->m_nextSprite->setScale(0.6f);
    f->m_nextSprite->setFlipX(true);
    f->m_nextButton = CCMenuItemSpriteExtra::create(f->m_nextSprite, this, menu_selector(CSUILayer::onNext));
    f->m_nextButton->setID("next-button"_spr);
    f->m_switchMenu->addChild(f->m_nextButton);

    f->m_switchMenu->setOpacity(jasmine::setting::getValue<int>("menu-opacity"));
    f->m_switchMenu->setLayout(AxisLayout::create()->setAutoScale(false)->setGap(10.0f));

    addEventListener(KeybindSettingPressedEventV3(GEODE_MOD_ID, "previous-checkpoint"), [this](
        const Keybind& keybind, bool down, bool repeat, double timestamp
    ) {
        if (down && !repeat) onPrev(nullptr);
    });

    addEventListener(KeybindSettingPressedEventV3(GEODE_MOD_ID, "next-checkpoint"), [this](
        const Keybind& keybind, bool down, bool repeat, double timestamp
    ) {
        if (down && !repeat) onNext(nullptr);
    });

    addEventListener(SettingChangedEventV3(GEODE_MOD_ID, "hide-menu"), [this](std::shared_ptr<SettingV3> setting) {
        m_fields->m_switchMenu->setVisible(
            !std::static_pointer_cast<BoolSettingV3>(std::move(setting))->getValue() && m_gameLayer->m_isPracticeMode);
    });

    addEventListener(SettingChangedEventV3(GEODE_MOD_ID, "menu-y-offset"), [this](std::shared_ptr<SettingV3> setting) {
        m_fields->m_switchMenu->setPositionY(std::static_pointer_cast<FloatSettingV3>(std::move(setting))->getValue());
    });

    addEventListener(SettingChangedEventV3(GEODE_MOD_ID, "menu-opacity"), [this](std::shared_ptr<SettingV3> setting) {
        m_fields->m_switchMenu->setOpacity(std::static_pointer_cast<IntSettingV3>(std::move(setting))->getValue());
    });

    return true;
}

void CSUILayer::onPrev(CCObject* sender) {
    auto playLayer = static_cast<CSPlayLayer*>(m_gameLayer);
    if (!playLayer->m_isPracticeMode) return;

    auto checkpoints = playLayer->m_checkpointArray->count();
    if (checkpoints == 0) return;

    auto f = m_fields.self();
    if (f->m_currentCheckpoint == 0) f->m_currentCheckpoint = checkpoints;
    else f->m_currentCheckpoint--;

    f->m_switchStarted = true;
    playLayer->queueLoadCheckpoint(f->m_currentCheckpoint);
    playLayer->resetLevel();
    updateMenu();
}

void CSUILayer::onNext(CCObject* sender) {
    auto playLayer = static_cast<CSPlayLayer*>(m_gameLayer);
    if (!playLayer->m_isPracticeMode) return;

    auto checkpoints = playLayer->m_checkpointArray->count();
    if (checkpoints == 0) return;

    auto f = m_fields.self();
    if (f->m_currentCheckpoint >= checkpoints) f->m_currentCheckpoint = 0;
    else f->m_currentCheckpoint++;

    f->m_switchStarted = true;
    playLayer->queueLoadCheckpoint(f->m_currentCheckpoint);
    playLayer->resetLevel();
    updateMenu();
}

void CSUILayer::readjustCheckpoint() {
    auto f = m_fields.self();
    if (f->m_currentCheckpoint > 0) f->m_currentCheckpoint--;
    static_cast<CSPlayLayer*>(m_gameLayer)->queueLoadCheckpoint(f->m_currentCheckpoint);
    updateMenu();
}

void CSUILayer::resetCheckpoint(uint32_t checkpoint) {
    auto f = m_fields.self();
    if (checkpoint == 0) {
        f->m_currentCheckpoint = 0;
        f->m_switchStarted = false;
    }
    else if (!f->m_switchStarted) {
        f->m_currentCheckpoint = checkpoint;
        static_cast<CSPlayLayer*>(m_gameLayer)->queueLoadCheckpoint(checkpoint);
    }
    updateMenu();
}

void CSUILayer::updateMenu() {
    auto playLayer = static_cast<PlayLayer*>(m_gameLayer);
    auto f = m_fields.self();

    if (!playLayer->m_isPracticeMode || jasmine::setting::getValue<bool>("hide-menu")) {
        return f->m_switchMenu->setVisible(false);
    }
    else {
        f->m_switchMenu->setVisible(true);
    }

    auto checkpoint = f->m_currentCheckpoint;
    auto checkpoints = playLayer->m_checkpointArray->count();

    f->m_switchLabel->setString(fmt::format("{}/{}", checkpoint, checkpoints).c_str());
    f->m_switchLabel->limitLabelWidth(40.0f, 0.6f, 0.1f);
    f->m_switchMenu->updateLayout();
}
