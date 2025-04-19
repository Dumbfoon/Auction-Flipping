#pragma once
#include <vector>
#include <unordered_map>
#include <string>

std::vector<unsigned char> base64_decode(const std::string& encoded);

std::vector<unsigned char> decompressGzip(const std::vector<unsigned char>& compressedData);

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j);

std::string DownloadUrlContent(const std::string& url);

std::string truncate(std::string number);

std::string getID(const std::string& item_bytes);

size_t count(const std::string& string, const std::string& substring);

void storeMap(const char* filepath, const std::unordered_map<std::string, unsigned long>& map);
