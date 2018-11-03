/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef IPCOUNTRYMGR_H_
#define IPCOUNTRYMGR_H_

#include "IpCountry.h"

class IpCountryMgr
{
    friend class ACE_Singleton<IpCountryMgr, ACE_Null_Mutex>;

    public:
        IpCountryMgr() {}
        ~IpCountryMgr();

        void Load();
        void Reload();

        IpCountry* GetIpCountry(uint32 ip);

    private:
        typedef std::vector<IpCountry*> IpSet;

        IpSet _Ips;
};

#define sIpCountryMgr ACE_Singleton<IpCountryMgr, ACE_Null_Mutex>::instance()

#endif /*IPCOUNTRYMGR_H_*/