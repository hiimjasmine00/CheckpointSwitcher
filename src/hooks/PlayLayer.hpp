#include <Geode/modify/PlayLayer.hpp>

class $modify(CSPlayLayer, PlayLayer) {
    struct Fields {
        CheckpointObject* m_currentCheckpoint = nullptr;
    };

    static void onModify(ModifyBase<ModifyDerive<CSPlayLayer, PlayLayer>>& self);

    void loadFromCheckpoint(CheckpointObject* object);
    void storeCheckpoint(CheckpointObject* checkpoint);
    void removeCheckpoint(bool first);
    void queueLoadCheckpoint(uint32_t checkpoint);
    void togglePracticeMode(bool practiceMode);
};
