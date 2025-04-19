#pragma once

#include <string>
#include <vector>

std::vector<unsigned char> base64_decode(const std::string& encoded);

std::vector<unsigned char> decompressGzip(const std::vector<unsigned char>& compressedData);

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j);

std::string DownloadUrlContent(const std::string& url);

std::string truncate(std::string number);

std::string getID(const std::string& item_bytes);
