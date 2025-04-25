#include "Pet.h"
#include "utility.h"
#include <utility>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cmath>

/******************************/
/*pet utility functions*/

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

void calculateProfit(const std::vector<Pet>& pets, unsigned int amount)
{
    using Key = std::pair<std::string, Pet::Rarity>; // (name, rarity)

    // Idk some witchcraft
    struct keyHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string>()(k.first) ^ std::hash<int>()(static_cast<int>(k.second));
        }
    };

    // Maps key (pet name and pet rarity) to pair (lvl 1 price, lvl 100 price)
    std::unordered_map<Key, std::pair<long long, long long>, keyHash> petMap;

    // Groups the LVL1/LVL100 pets together
    for (const auto& pet : pets) 
    {
        Key key = { pet.m_Name, pet.m_Rarity };
        if (pet.m_Level == 1) 
        {
            petMap[key].first = pet.m_Price;
        }
        else if (pet.m_Level == 100) 
        {
            petMap[key].second = pet.m_Price;
        }
    }

    // How the results are stored
    struct Result 
    {
        std::string name;
        Pet::Rarity rarity;
        double coinsPerExp;
    };

    // Make vector of results
    std::vector<Result> results;

    // Iterate for every key-costPair
    for (const auto& [key, costPair] : petMap) 
    {
        // Name rarity pair from key
        const auto& [name, rarity] = key;

        //Get costs from cost pair
        long long lvl1Cost = costPair.first;
        long long lvl100Cost = costPair.second;

        if (lvl1Cost > 0 && lvl100Cost > 0 && lvl100Cost > lvl1Cost) 
        {
            double coinsPerExp = static_cast<double>(lvl100Cost - lvl1Cost) / rarityToExp(rarity);
            results.push_back({ name, rarity, coinsPerExp });
        }
    }

    // Sort descending by value per exp
    std::sort(results.begin(), results.end(), 
        [](const Result& a, const Result& b) 
        {
            return a.coinsPerExp > b.coinsPerExp;
        }
    );

    // Print (amount) results
    for (size_t i = 0; i < amount; i++) 
    {
        Result r = results[i];
        std::cout << rarityToString(r.rarity) << " " << r.name << " ( " << r.coinsPerExp << " coins/exp)\n";
    }
}

bool isPet(const nlohmann::json& auction) 
{ 
    return to_string(auction["item_name"]).find("[Lvl") != std::string::npos; 
}

std::ostream& operator<<(std::ostream& os, const Pet& pet)
{
    os << "[LVL " << pet.m_Level << "] " << rarityToString(pet.m_Rarity) << " " << pet.m_Name << " - " << truncate(std::to_string(pet.m_Price)) << " coins.\n";
    return os;
}

/****************************/
/*pet class functions*/
Pet::Pet() : m_Name("NULL"), m_Rarity(Pet::Rarity::COMMON), m_Level(1), m_Price(0), m_CoinsPerEXP(0) {}

Pet::Pet(const std::string& name, Pet::Rarity rarity, int level, unsigned long price) : m_Name(name), m_Rarity(rarity), m_Level(level), m_Price(price), m_CoinsPerEXP(0) {}

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

/**************************/
/*pet class members*/
const std::unordered_map<std::string, Pet::Rarity> Pet::tierToRarity = {
    {"COMMON", Rarity::COMMON},
    {"UNCOMMON", Rarity::UNCOMMON},
    {"RARE", Rarity::RARE},
    {"EPIC", Rarity::EPIC},
    {"LEGENDARY", Rarity::LEGENDARY},
    {"MYTHIC", Rarity::MYTHIC}
};