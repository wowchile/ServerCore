/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "Creature.h"
#include "GameObject.h"
#include "Player.h"

#define EVENT_CREATURE_BASE_GUID 10000000
#define EVENT_GOBJECT_BASE_GUID 10000000

class Event
{
    public:
        Event(std::string name, uint32 mapId, float x, float y, float z, uint32 phaseMask = 1, bool visible = false, uint32 time = 0, uint32 id = 0)
            : _id(id), _name(name), _mapId(mapId), _x(x), _y(y), _z(z), _phaseMask(phaseMask), _visible(visible), _time(time) { }
        ~Event() { }

        void Create();
        void RemoveFromDB();

        uint32 GetEventId() { return _id; }

        void SetName(const char* name);
        std::string GetName() { return _name; }

        uint32 GetMapId() { return _mapId; }
        float GetPositionX() { return _x; }
        float GetPositionY() { return _y; }
        float GetPositionZ() { return _z; }

        void SetPhaseMask(uint32 phaseMask);
        uint32 GetPhaseMask() { return _phaseMask; }

        void SetVisible(bool visible);
        bool IsVisible() { return _visible; }

        void SetTime(uint32 time);
        uint32 GetTime() { return _time; }

        void InsertCreature(uint64 creatureGUID, uint32 playerAccount = 0);
        void RemoveCreature(Creature* creature);
        bool IsEventCreature(Creature* creature) { return creature && IsEventCreature(creature->GetGUID()); }
        bool IsEventCreature(uint64 guid) { return guid >= EVENT_CREATURE_BASE_GUID && _creatures.find(guid) != _creatures.end(); }
        uint32 GetAccountOfCreature(uint64 guid);

        void InsertGameObject(uint64 gobjectGUID, uint32 playerAccount = 0);
        void RemoveGameObject(GameObject* gobject);
        bool IsEventGameObject(GameObject* gobject) { return gobject && IsEventGameObject(gobject->GetGUID()); }
        bool IsEventGameObject(uint64 guid) { return guid >= EVENT_GOBJECT_BASE_GUID && _gobjects.find(guid) != _gobjects.end(); }
        uint32 GetAccountOfGameObject(uint64 guid);

        void AddConstructor(Player* player);
        void DelConstructor(Player* player);
        bool IsConstructor(Player* player) { return player && IsConstructor(player->GetGUID()); }
        bool IsConstructor(uint64 guid) { return _constructors.find(guid) != _constructors.end(); }

        std::list<uint32> GetEventConstructorsHistory();

        void SaveToDB();

        void LoadCreatures();
        void LoadGameObjects();

    private:
        void RemoveCreatures();
        void RemoveGameObjects();

        typedef std::set<uint64> GuidSet;

        uint32 _id;
        std::string _name;
        uint32 _mapId;
        float _x, _y, _z;
        uint32 _phaseMask;
        bool _visible;
        uint32 _time;

        // Criaturas y Objetos spawneados bajo este evento
        GuidSet _creatures;
        GuidSet _gobjects;
        // Constructores activos
        GuidSet _constructors;
};

#endif /*EVENT_H_*/
