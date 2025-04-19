#pragma once

#include <string>
#include <vector>
#include <zlib/zlib.h>
#include <memory>
#include <windows.h>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

std::vector<unsigned char> base64_decode(const std::string& encoded);

std::vector<unsigned char> decompressGzip(const std::vector<unsigned char>& compressedData);

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j);

struct StreamDeleter {
    void operator()(IStream* p) const;
};

std::string DownloadUrlContent(const std::wstring& url);

std::string truncate(std::string number);

std::string readFile(const std::string& filepath);