#include "Pet.h"
#include "utility.h"
#include <utility>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cmath>

std::string rarityToString(Pet::Rarity rarity) 
{
    switch (rarity) {
    case Pet::Rarity::COMMON: return "COMMON";
    case Pet::Rarity::UNCOMMON: return "UNCOMMON";
    case Pet::Rarity::RARE: return "RARE";
    case Pet::Rarity::EPIC: return "EPIC";
    case Pet::Rarity::LEGENDARY: return "LEGENDARY";
    case Pet::Rarity::MYTHIC: return "MYTHIC";
    default: return "UNKNOWN";
    }
}

unsigned int rarityToExp(Pet::Rarity rarity)
{
    switch (rarity) {
    case Pet::Rarity::COMMON: return 5624785;
    case Pet::Rarity::UNCOMMON: return 8644220;
    case Pet::Rarity::RARE: return 12626665;
    case Pet::Rarity::EPIC: return 17222200;
    case Pet::Rarity::LEGENDARY: case Pet::Rarity::MYTHIC: return 25353230;
    default: return 1;
    }
}

Pet::Pet() { m_CoinsPerEXP = 0; }

Pet::Pet(const std::string& name, Pet::Rarity rarity, int level, unsigned long price) : m_Name(name), m_Rarity(rarity), m_Level(level), m_Price(price) {}


bool Pet::operator==(const Pet& other) const
{
    if (this->m_Name == other.m_Name && this->m_Rarity == other.m_Rarity && this->m_Level == other.m_Level) return true;
    return false;
}

double Pet::getCEXP(const Pet& other) const
{ 
    unsigned int expNeeded = rarityToExp(m_Rarity);
    unsigned long lvl1price = (m_Level < other.m_Level) ? m_Price : other.m_Price;
    unsigned long lvl100price = (m_Level > other.m_Level) ? m_Price : other.m_Price;
    if (lvl1price > lvl100price) return 0;
    return (double)(lvl100price - lvl1price) / (double)expNeeded;
}

std::ostream& operator<<(std::ostream& os, const Pet& pet)
{
    os << "[LVL " << pet.m_Level << "] " << rarityToString(pet.m_Rarity) << " " << pet.m_Name << " - " << truncate(std::to_string(pet.m_Price)) << " coins.\n";
    return os;
}

const std::unordered_map<std::string, Pet::Rarity> Pet::tierToRarity = {
    {"COMMON", Rarity::COMMON},
    {"UNCOMMON", Rarity::UNCOMMON},
    {"RARE", Rarity::RARE},
    {"EPIC", Rarity::EPIC},
    {"LEGENDARY", Rarity::LEGENDARY},
    {"MYTHIC", Rarity::MYTHIC}
};


Pet getAsPet(const nlohmann::json& auction)
{
    std::string nameString;
    for (char character : to_string(auction["item_name"]))
    {
        if (isascii(character) && character != 34) nameString += character;
    }
    size_t lastChar = 0;
    for (size_t i = 0; i < nameString.length(); i++)
    {
        if (isalpha(nameString[i])) lastChar = i;
    }
    nameString.erase(lastChar + 1, 1);
    if (count(nameString, "[") == 2)
    {
        size_t idx1 = nameString.find("[");
        idx1 = nameString.find("[", idx1 + 1);
        size_t idx2 = nameString.find("]");
        idx2 = nameString.find("]", idx2 + 1);
        nameString.erase(idx1, idx2 - idx1 + 1);
    }
    auto it = Pet::tierToRarity.find(auction["tier"]);
    Pet::Rarity rarity = (it != Pet::tierToRarity.end()) ? it->second : Pet::Rarity::COMMON;
    size_t idx1 = nameString.find("[Lvl");
    size_t idx2 = nameString.find("]");
    int level = stoi(nameString.substr(idx1 + 5, idx2 - idx1 - 5));
    size_t idx3 = nameString.length();
    std::string name = nameString.substr(idx2 + 2, idx3 - idx2 - 2);
    unsigned long price = auction["starting_bid"];
    return Pet(name, rarity, level, price);
}


bool isPet(const nlohmann::json& auction) { return to_string(auction["item_name"]).find("[Lvl") != std::string::npos; }