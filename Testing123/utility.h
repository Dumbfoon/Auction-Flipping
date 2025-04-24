#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <utility>

std::vector<unsigned char> base64_decode(const std::string& encoded);

std::vector<unsigned char> decompressGzip(const std::vector<unsigned char>& compressedData);

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j);

std::string DownloadUrlContent(const std::string& url);

std::string truncate(std::string number);

size_t count(const std::string& string, const std::string& substring);

void storeMap(const char* filepath, std::unordered_map<std::string, std::string>& map, std::unordered_map<std::string, unsigned long>& map2);

nlohmann::json getURL(const std::string& url);

/*skyblock functions below*/

std::string getID(const std::string& item_bytes);

template <typename T>
bool vectorContains(const std::vector<T>& vec, T value)
{
    return std::find(vec.cbegin(), vec.cend(), value) != vec.cend();
}
