/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

IpCountryMgr::~IpCountryMgr()
{
    for (IpSet::iterator itr = _Ips.begin(); itr != _Ips.end(); ++itr)
        delete *itr;
    _Ips.clear();
}

void IpCountryMgr::Load()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    // IpCountryCountry
    uint32 ip_from;
    uint32 ip_to;
    const char* country_code;
    const char* country_name;

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IPCOUNTRY);
    PreparedQueryResult result = LoginDatabase.Query(stmt);
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            ip_from = fields[0].GetUInt32();
            ip_to = fields[1].GetUInt32();
            country_code = fields[2].GetCString();
            country_name = fields[3].GetCString();

            _Ips.push_back(new IpCountry(ip_from, ip_to, country_code, country_name));
            count++;
        }
        while (result->NextRow());
    }
    else
    {
        sLog->outString(">> Loaded 0 ipcountries. DB table `ipcountries` is empty!");
        return;
    }

    sLog->outString(">> Loaded %u ipcountries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void IpCountryMgr::Reload()
{
    // Memory leak?
    _Ips.clear();

    Load();
}

IpCountry* IpCountryMgr::GetIpCountry(uint32 ip)
{
    for (IpSet::iterator itr = _Ips.begin(); itr != _Ips.end(); ++itr)
        if (ip <= (*itr)->GetIpTo())
            return (*itr);

    return NULL;
}