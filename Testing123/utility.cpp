#pragma once
#include "utility.h"

#include <nlohmann/json.hpp>
#include <zlib/zlib.h>

#include <urlmon.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <limits.h>
#include <unordered_map>

#pragma comment(lib, "urlmon.lib")

const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::vector<unsigned char> base64_decode(const std::string& encoded)
{
    std::vector<unsigned char> decoded;
    unsigned int val = 0;
    int valb = -8;

    for (unsigned char c : encoded)
    {
        if (isspace(c)) continue; // skip whitespace
        if (c == '=') break;
        size_t index = base64_chars.find(c);
        if (index == std::string::npos) throw std::runtime_error("Invalid base64 character");
        val = (val << 6) + index;
        valb += 6;
        if (valb >= 0)
        {
            decoded.push_back((val >> valb) & 0b11111111);
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

    if (inflateInit2(&strm, 15 + 16) != Z_OK) throw std::runtime_error("inflateInit2 failed");

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
    } 
    while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return decompressedData;
}

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j)
{
    return j.dump();
}

std::string DownloadUrlContent(const std::string& url)
{
    IStream* tempStream = nullptr;

    if (SUCCEEDED(URLOpenBlockingStreamA(nullptr, url.c_str(), &tempStream, 0, nullptr))) {

        std::string content;
        char buffer[8192];
        ULONG bytesRead = 0;

        while (SUCCEEDED(tempStream->Read(buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0) {
            content.append(buffer, bytesRead);
        }

        tempStream->Release();
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

std::string getID(const std::string& item_bytes)
{
    size_t indexID = NULL;
    size_t indexEND = NULL;
    std::vector<unsigned char> decompressedData;
    decompressedData.reserve(256);
    decompressedData = decompressGzip(base64_decode(item_bytes));
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

    return skyblockID;
}

size_t count(const std::string& string, const std::string& substring)
{
    int index = -1;
    size_t count = 0;
    while (true)
    {
        size_t idx = string.find(substring, index + 1);
        if (idx == std::string::npos) break;
        index = idx;
        count++;
    }
    return count;
}

void storeMap(const char* filepath, const std::unordered_map<std::string, unsigned long>& map)
{
    std::ofstream file(filepath);
    for (const std::pair<const std::string&, unsigned long>& pair : map)
    {
        file << "Lowest BIN for " << pair.first << " is " << pair.second << "\n";
    }
    file.close();
}