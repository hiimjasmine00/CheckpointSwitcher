#include <Geode/modify/UILayer.hpp>

class $modify(CSUILayer, UILayer) {
    struct Fields {
        cocos2d::CCMenu* m_switchMenu = nullptr;
        cocos2d::CCLabelBMFont* m_switchLabel = nullptr;
        cocos2d::CCSprite* m_prevSprite = nullptr;
        CCMenuItemSpriteExtra* m_prevButton = nullptr;
        cocos2d::CCSprite* m_nextSprite = nullptr;
        CCMenuItemSpriteExtra* m_nextButton = nullptr;
        uint32_t m_currentCheckpoint = 0;
        bool m_switchStarted = false;
    };

    static void onModify(ModifyBase<ModifyDerive<CSUILayer, UILayer>>& self);

    bool init(GJBaseGameLayer* layer);
    void onPrev(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);
    void readjustCheckpoint();
    void resetCheckpoint(uint32_t checkpoint = 0);
    void updateMenu();
};
