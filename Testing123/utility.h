#pragma once

#include <string>
#include <vector>
#include <zlib/zlib.h>
#include <windows.h>
#include <urlmon.h>

std::vector<unsigned char> base64_decode(const std::string& encoded);

std::vector<unsigned char> decompressGzip(const std::vector<unsigned char>& compressedData);

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j);

std::string DownloadUrlContent(const std::string& url);

std::string truncate(std::string number);

std::string readFile(const std::string& filepath);