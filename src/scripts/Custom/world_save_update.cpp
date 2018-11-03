#include "ScriptPCH.h"

class world_save_update : public WorldScript
{
public:
    world_save_update() : WorldScript("world_save_update") {}

    void OnStartup()
    {
        tick = 5 * MINUTE * IN_MILLISECONDS;
    }

    void OnUpdate(uint32 diff)
    {
        if (tick > diff)
        {
            tick -= diff;
            return;
        }
        else
            tick = IN_MILLISECONDS;

        if (saveSessionsQueue.size() == 0)
        {
            saveSessionsQueue = sWorld->GetAllSessions();
            return;
        }

        uint32 processPlayer = std::min(uint32(sWorld->getIntConfig(CONFIG_PLAYER_SAVE_PER_SECOND)), uint32(saveSessionsQueue.size()));
        for (uint8 i = 0; i < processPlayer; ++i)
        {
            if (WorldSession* session = sWorld->FindSession(saveSessionsQueue.begin()->first))
                if (Player* player = session->GetPlayer())
                    player->SaveToDB();
            saveSessionsQueue.erase(saveSessionsQueue.begin());
        }
    }

private:
    SessionMap saveSessionsQueue;
    uint32 tick;
};

void AddSC_world_save_update()
{
    new world_save_update();
}
