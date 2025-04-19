//i like men

#include <nlohmann/json.hpp>
#include <zlib/zlib.h>

#include <urlmon.h>
#include <windows.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <limits.h>
#include <unordered_map>
#include <codecvt>
#include <fstream>

#pragma comment(lib, "urlmon.lib")

#define DEBUG true
#define TEMP false
#define LOG(x) if(DEBUG) std::cout << x
#define TEMPORARY(x) if(TEMP) std::cout << x

const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::vector<unsigned char> base64_decode(const std::string& encoded)
{
    std::vector<unsigned char> decoded;
    int val = 0, valb = -8;

    for (unsigned char c : encoded)
    {
        if (isspace(c)) continue; // skip whitespace
        if (c == '=') break;
        int index = base64_chars.find(c);
        if (index == std::string::npos) throw std::runtime_error("Invalid base64 character");
        val = (val << 6) + index;
        valb += 6;
        if (valb >= 0)
        {
            decoded.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }

    return decoded;
}

std::vector<unsigned char> decompressGzip(const std::vector<unsigned char>& compressedData)
{
    const int CHUNK = 4096;
    std::vector<unsigned char> decompressedData;
    z_stream strm = {};

    strm.avail_in = compressedData.size();
    strm.next_in = const_cast<unsigned char*>(compressedData.data());

    if (inflateInit2(&strm, 15 + 16) != Z_OK) {
        throw std::runtime_error("inflateInit2 failed");
    }

    unsigned char out[CHUNK];

    int ret;
    do
    {
        strm.avail_out = CHUNK;
        strm.next_out = out;

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&strm);
            throw std::runtime_error("Code " + std::to_string(ret));
        }

        decompressedData.insert(decompressedData.end(), out, out + (CHUNK - strm.avail_out));
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return decompressedData;
}

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j)
{
    return j.dump();
}

struct StreamDeleter {
    void operator()(IStream* p) const { if (p) p->Release(); }
};

std::string DownloadUrlContent(const std::wstring& url) {
    std::unique_ptr<IStream, StreamDeleter> stream;
    IStream* tempStream = nullptr;

    if (SUCCEEDED(URLOpenBlockingStream(nullptr, url.c_str(), &tempStream, 0, nullptr))) {
        stream.reset(tempStream);

        std::string content;
        char buffer[4096];
        ULONG bytesRead = 0;

        while (SUCCEEDED(stream->Read(buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0) {
            content.append(buffer, bytesRead);
        }

        return content;
    }

    return "";
}

std::string truncate(std::string number)
{
    size_t numLength = number.length();
    size_t offset = numLength % 3;
    for (size_t i = numLength - 1; i > 0; i--)
    {
        if (i % 3 == offset)
        {
            number.insert(i, 1, ',');
        }
    }

    return number;
}

std::string readFile(const std::string& filepath)
{
    std::string ret;
    std::string text;
    std::ifstream MyFile(filepath);
    while (std::getline(MyFile, text))
    {
        ret += text;
    }
    MyFile.close();
    return ret;
}

const std::string auctionStr = "https://api.hypixel.net/v2/skyblock/auctions";
const std::wstring auctionQuery = std::wstring(auctionStr.begin(), auctionStr.end());

const std::string itemStr = "https://api.hypixel.net/v2/resources/skyblock/items";
const std::wstring itemQuery = std::wstring(itemStr.begin(), itemStr.end());

int main()
{
    auto print_key_value = [](const auto& key, const auto& value)
        {
            if (value != ULONG_MAX && value != NULL) std::cout << "Item:[" << key << "] LBIN:[" << truncate(std::to_string(value)) << "]\n";
        };

    try {
        nlohmann::json auctionBig = nlohmann::json::parse(DownloadUrlContent(auctionQuery));
        nlohmann::json itemInfo = nlohmann::json::parse(DownloadUrlContent(itemQuery));

        std::unordered_map<std::string, unsigned long> itemMap;

        size_t items = itemInfo["items"].size();
        LOG("There are currently " + std::to_string(items) + " items.\n");

        size_t pages = auctionBig["totalPages"];

        for (size_t i = 0; i < items; i++)
        {
            std::string itemID = itemInfo["items"][i]["id"];
            itemMap[itemID] = ULONG_MAX;
        }

        for (size_t n = 0; n < pages; n++)
        {
            const std::string auctStr = auctionStr + "?page=" + std::to_string(n);
            const std::wstring auctQuery = std::wstring(auctStr.begin(), auctStr.end());
            nlohmann::json auctionInfo = nlohmann::json::parse(DownloadUrlContent(auctQuery));
            size_t auctions = auctionInfo["auctions"].size();
            LOG("There are currently " + std::to_string(auctions) + " auctions on Page " + std::to_string(n) + ".\n");
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
                if (indexID == NULL)
                {
                    //LOG(auctionInfo["auctions"][i]["item_name"] << "\n");
                    throw std::runtime_error("Failed to assign ID index properly.");
                }
                if (indexEND == NULL)
                {
                    throw std::runtime_error("Failed to assign UUID index properly.");
                }

                for (size_t j = indexID + 4; j < indexEND - 1; j++)
                {
                    skyblockID += decompressedData[j];
                }

                if (auctionInfo["auctions"][i]["starting_bid"] < itemMap[skyblockID] && auctionInfo["auctions"][i]["bin"])
                {
                    itemMap[skyblockID] = auctionInfo["auctions"][i]["starting_bid"];
                }

            }

            std::cout << "Loading... [" << n + 1 << " of " << pages << "]" << std::endl;
        }


        for (const std::pair<const std::string, unsigned long>& n : itemMap)
            print_key_value(n.first, n.second);
    }
    catch (const std::exception& e)
    {
        LOG(e.what() << std::endl);
    }
}