#include <windows.h>
#include <urlmon.h>
#include <iostream>

#pragma comment(lib, "urlmon.lib")

std::string DownloadUrlContent(const char* url)
{
    IStream* tempStream = nullptr;

    if (SUCCEEDED(URLOpenBlockingStreamA(nullptr, url, &tempStream, 0, nullptr))) {

        std::string content;
        char buffer[524288];
        ULONG bytesRead = 0;

        while (SUCCEEDED(tempStream->Read(buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0) {
            content.append(buffer, bytesRead);
        }

        tempStream->Release();
        return content;
    }

    return "";
}

int main()
{
    std::cout << "hi";
    std::string x = DownloadUrlContent("https://api.hypixel.net/v2/skyblock/auctions");
    std::cout << "hello";
    std::cout << x << std::endl;
}