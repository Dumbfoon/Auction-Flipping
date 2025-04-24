#pragma once
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <iostream>
#include "utility.h"

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

    std::string m_Name;
    Rarity m_Rarity;
    int m_Level;
    unsigned long m_Price;
    double m_CoinsPerEXP;

    static const std::unordered_map<std::string, Rarity> tierToRarity;

    double getCEXP(const Pet& other) const;
    bool operator==(const Pet& other) const;
};

std::ostream& operator<<(std::ostream& os, const Pet& pet);

bool isPet(const nlohmann::json& auction);

Pet getAsPet(const nlohmann::json& auction);

void storePets(const char* filepath, const std::vector<Pet>& pets);

std::string rarityToString(Pet::Rarity rarity);

unsigned int rarityToExp(Pet::Rarity rarity);

template<size_t N>
std::pair<unsigned long, size_t> smallestInArray(const Pet(&arr)[N])
{
    unsigned long smallest = ULONG_MAX;
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

template <size_t amount>
void calculateProfit(const std::vector<Pet>& pets)
{
    /*we need to make an array of pets and how many pets is based on the value of amount and then after making the array we initialise all the pets and then after that we need to have like a pairing of the lowest LVL 1 pet and LVL 100 pet of the same name and rarity, then we calculate the difference in price and divide it by the EXP needed*/
    Pet topPets[amount];
    for (size_t i = 0; i < amount; ++i) {
        topPets[i] = Pet("NULL", Pet::Rarity::COMMON, 1, 0);
    }

    for (size_t i = 0; i < pets.size(); i++)
    {
        Pet pet = pets[i];
        for (size_t j = 0; j < pets.size(); j++)
        {
            if (i == j) continue;
            Pet pet2 = pets[j];
            if (pet.m_Name == pet2.m_Name && pet.m_Rarity == pet2.m_Rarity)
            {
                std::pair<unsigned long, size_t> smallestPair = smallestInArray(topPets);
                double cEXP = pet.getCEXP(pet2);
                long priceDifference = pet.m_Price - pet2.m_Price;
                if (cEXP > smallestPair.first)
                {
                    pet.m_CoinsPerEXP = cEXP;
                    topPets[smallestPair.second] = pet;
                }
            }
        }
    }

    for (size_t i = 0; i < amount; i++)
    {
        const Pet& pet = topPets[i];
        std::cout << "Best coins per exp is: " << rarityToString(pet.m_Rarity) << " " << pet.m_Name << " with " << pet.m_CoinsPerEXP << " coins per EXP.(" << pet.m_Price << ")\n";
    }
}