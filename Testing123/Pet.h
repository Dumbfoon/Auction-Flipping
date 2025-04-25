#pragma once
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <iostream>
#include "utility.h"

/*********************/
/*pet class*/
class Pet
{
public:
    enum class Rarity {
        BLANK = 0,
        COMMON,
        UNCOMMON,
        RARE,
        EPIC,
        LEGENDARY,
        MYTHIC,
        DIVINE,
        SPECIAL,
        VERY_SPECIAL,
        ULTIMATE
    };

    Pet();
    Pet(const std::string& name, Rarity rarity, int level, unsigned long price);

    std::string m_Name = "NULL";
    Rarity m_Rarity = Rarity::COMMON;
    int m_Level = 1;
    unsigned long m_Price = 0;
    double m_CoinsPerEXP = 0;

    static const std::unordered_map<std::string, Rarity> tierToRarity;

    double getCEXP(const Pet& other) const;
    bool operator==(const Pet& other) const;
};

/*****************************************/
/*pet utility functions*/
std::ostream& operator<<(std::ostream& os, const Pet& pet);

bool isPet(const nlohmann::json& auction);

Pet getAsPet(const nlohmann::json& auction);

void storePets(const char* filepath, const std::vector<Pet>& pets);

std::string rarityToString(Pet::Rarity rarity);

unsigned int rarityToExp(Pet::Rarity rarity);

/***************************************/
/*templated pet utility functions (not in .cpp file)*/
template<size_t N>
std::pair<double, size_t> smallestInArray(const Pet(&arr)[N])
{
    double smallest = ULONG_MAX;
    size_t idx = 0;
    for (size_t i = 0; i < N; i++)
    {
        if (arr[i].m_CoinsPerEXP < smallest)
        {
            smallest = arr[i].m_CoinsPerEXP;
            idx = i;
        }
    }
    return std::make_pair(smallest, idx);
}

void calculateProfit(const std::vector<Pet>& pets, unsigned int amount);
