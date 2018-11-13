/*
 * Copyright (C) 2010-2012 Battle.cl <http://www.battle.cl/>
 *
 * Proyecto privado desarrollado en paralelo a TrinityCore,
 * que tiene como finalidad el desarrollo de WoWChile, el estudio
 * y la practica de la informatica en World of Warcraft.
 *
 */

#ifndef IPCOUNTRY_H_
#define IPCOUNTRY_H_

class IpCountry
{
    public:
        IpCountry(uint32 ip_from, uint32 ip_to, std::string country_code, std::string country_name) : _ip_from(ip_from), _ip_to(ip_to), _country_code(country_code), _country_name(country_name) {}

        uint32 GetIpFrom() { return this->_ip_from; }
        uint32 GetIpTo() { return this->_ip_to; }
        const char* GetCountryCode() { return this->_country_code.c_str(); }
        const char* GetCountryName() { return this->_country_name.c_str(); }

    private:
        uint32 _ip_from;
        uint32 _ip_to;
        std::string _country_code;
        std::string _country_name;
};

#endif /*IPCOUNTRY_H_*/