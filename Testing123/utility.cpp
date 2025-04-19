#pragma once
#include "utility.h"

#include <nlohmann/json.hpp>
#include <zlib/zlib.h>

#include <urlmon.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <vector>
#include <limits.h>
#include <unordered_map>
#include <codecvt>
#include <fstream>

#pragma comment(lib, "urlmon.lib")

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

std::string DownloadUrlContent(const std::string& url)
{
    IStream* tempStream = nullptr;

    if (SUCCEEDED(URLOpenBlockingStreamA(nullptr, url.c_str(), &tempStream, 0, nullptr))) {

        std::string content;
        char buffer[524288];
        ULONG bytesRead = 0;

        while (SUCCEEDED(tempStream->Read(buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0) {
            content.append(buffer, bytesRead);
        }

        tempStream->Release();
        return content;
    }
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