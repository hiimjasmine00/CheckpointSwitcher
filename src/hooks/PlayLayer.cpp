#include "PlayLayer.hpp"
#include "UILayer.hpp"
#include <Geode/binding/CheckpointObject.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <jasmine/hook.hpp>

using namespace geode::prelude;

void CSPlayLayer::onModify(ModifyBase<ModifyDerive<CSPlayLayer, PlayLayer>>& self) {
    jasmine::hook::modify(self.m_hooks, "enabled");
    (void)self.setHookPriority("PlayLayer::loadFromCheckpoint", Priority::Early);
}

void CSPlayLayer::loadFromCheckpoint(CheckpointObject* object) {
    if (object == m_currentCheckpoint) return PlayLayer::loadFromCheckpoint(object);

    auto currentCheckpoint = m_fields->m_currentCheckpoint;
    if (currentCheckpoint) return PlayLayer::loadFromCheckpoint(currentCheckpoint);

    startMusic();
    if (m_startPosObject && m_currentCheckpoint) {
        PlayLayer::loadFromCheckpoint(m_currentCheckpoint);
    }
    else {
        // For the Blending Glow mod by NinKaz
        m_player1->updatePlayerArt();
        if (m_gameState.m_isDualMode) m_player2->updatePlayerArt();
    }
}

void CSPlayLayer::storeCheckpoint(CheckpointObject* checkpoint) {
    #ifdef GEODE_IS_DESKTOP
    auto f = m_fields.self();
    auto currentCheckpoint = f->m_currentCheckpoint;
    #endif

    PlayLayer::storeCheckpoint(checkpoint);

    #ifdef GEODE_IS_DESKTOP
    if (currentCheckpoint && !m_checkpointArray->containsObject(currentCheckpoint)) {
        f->m_currentCheckpoint = static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(0));
    }
    #endif

    static_cast<CSUILayer*>(m_uiLayer)->resetCheckpoint(m_checkpointArray->count());
}

void CSPlayLayer::removeCheckpoint(bool first) {
    auto f = m_fields.self();
    auto currentCheckpoint = f->m_currentCheckpoint;

    if (!first) {
        Ref<CheckpointObject> checkpoint = currentCheckpoint;
        if (checkpoint) {
            m_checkpointArray->removeObject(checkpoint);
            m_checkpointArray->addObject(checkpoint);
        }
    }

    PlayLayer::removeCheckpoint(first);

    if (first && currentCheckpoint && !m_checkpointArray->containsObject(currentCheckpoint)) {
        f->m_currentCheckpoint = m_checkpointArray->count() > 0 ? static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(0)) : nullptr;
    }

    auto uiLayer = static_cast<CSUILayer*>(m_uiLayer);
    if (first) uiLayer->updateMenu();
    else uiLayer->readjustCheckpoint();
}

void CSPlayLayer::queueLoadCheckpoint(uint32_t checkpoint) {
    m_fields->m_currentCheckpoint = checkpoint > 0 ? static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(checkpoint - 1)) : nullptr;
}

void CSPlayLayer::togglePracticeMode(bool practiceMode) {
    PlayLayer::togglePracticeMode(practiceMode);
    static_cast<CSUILayer*>(m_uiLayer)->resetCheckpoint();
}
