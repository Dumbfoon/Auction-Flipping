//viktor is the goat
#include "utility.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>
#include <limits.h>
#include <unordered_map>

#define DEBUG true
#define TEMP false
#define LOG(x) if(DEBUG) std::cout << x
#define TEMPORARY(x) if(TEMP) std::cout << x

int main()
{
    auto print_key_value = [](const auto& key, const auto& value)
    {
        if (value != ULONG_MAX && value != NULL) std::cout << "Item:[" << key << "] LBIN:[" << truncate(std::to_string(value)) << "]\n";
    };

    try {
        /*get item API data*/
        nlohmann::json itemData = nlohmann::json::parse(DownloadUrlContent("https://api.hypixel.net/v2/resources/skyblock/items"));

        /*create the item map*/
        std::unordered_map<std::string, unsigned long> itemMap;

        /*find out how many items there are*/
        size_t items = itemData["items"].size();

        /*setup item map*/
        for (size_t i = 0; i < items; i++)
        {
            std::string itemID = itemData["items"][i]["id"];
            itemMap[itemID] = ULONG_MAX;
        }

        /*run until program terminates*/
        while (true)
        {
            /*find number of AH pages*/
            bool end = false;

            /*gather data for every auction*/
            for (size_t n = 0; n < 100 && !end; n++)
            {
                nlohmann::json auctionInfo = nlohmann::json::parse(DownloadUrlContent("https://api.hypixel.net/v2/skyblock/auctions?page=" + std::to_string(n)));
                size_t auctions = auctionInfo["auctions"].size();
                if (auctionInfo["page"] == auctionInfo["totalPages"]) end = true;
                for (size_t i = 0; i < auctions; i++)
                {
                    size_t indexID = NULL;
                    size_t indexEND = NULL;
                    std::vector<unsigned char> decompressedData = decompressGzip(base64_decode(auctionInfo["auctions"][i]["item_bytes"]));
                    std::string skyblockID;
                    for (size_t j = 0; j < decompressedData.size() - 3; j++)
                    {
                        //if in the form id X
                        if ((int)decompressedData[j] == 105 && (int)decompressedData[j + 1] == 100 && (int)decompressedData[j + 2] == 0 && (int)decompressedData[j - 1] != 117)
                        {
                            indexID = j;
                            size_t x = j + 4;
                            while (true)
                            {
                                if ((int)decompressedData[x] == 0)
                                {
                                    if ((int)decompressedData[x + 1] == 0) indexEND = x + 1;
                                    else indexEND = x;
                                    break;
                                }
                                x++;
                            }
                        }
                    }
                    if (indexID == NULL) throw std::runtime_error("Failed to assign ID index properly.");
                    if (indexEND == NULL) throw std::runtime_error("Failed to assign END index properly.");

                    for (size_t j = indexID + 4; j < indexEND - 1; j++)
                    {
                        skyblockID += decompressedData[j];
                    }

                    if (auctionInfo["auctions"][i]["starting_bid"] < itemMap[skyblockID] && auctionInfo["auctions"][i]["bin"])
                    {
                        itemMap[skyblockID] = auctionInfo["auctions"][i]["starting_bid"];
                    }

                }

                std::cout << "Loading... [" << n + 1 << " of " << auctionInfo["totalPages"] << "]" << std::endl;
            }


            for (const std::pair<const std::string, unsigned long>& n : itemMap)
            {
                print_key_value(n.first, n.second);
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG(e.what() << std::endl);
    }
}