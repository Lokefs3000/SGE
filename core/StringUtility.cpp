#include "pch.h"
#include "StringUtility.h"

std::vector<std::string> StringUtility::SplitString(std::string source, std::string token)
{
    //https://stackoverflow.com/questions/24327637/what-is-the-most-efficient-c-method-to-split-a-string-based-on-a-particular-de

    std::vector<std::string> results;
    
    size_t pos = 0;
    std::string curr;
    while ((pos = source.find(token)) != std::string::npos) {
        curr = source.substr(0, pos);
        source.erase(0, pos + token.length());
        results.push_back(curr);
    }
    results.push_back(curr); //extra

    return results;
}

std::string StringUtility::Trim(std::string source)
{
    //https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring

    source.erase(source.begin(), std::find_if(source.begin(), source.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    source.erase(std::find_if(source.rbegin(), source.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), source.end());

    return source;
}

std::string StringUtility::TrimChar(std::string source, unsigned char ch)
{
    //Refer to above comment

    source.erase(source.begin(), std::find_if(source.begin(), source.end(), [&ch](unsigned char _ch) {
        return _ch != ch;
        }));

    source.erase(std::find_if(source.rbegin(), source.rend(), [&ch](unsigned char _ch) {
        return _ch != ch;
        }).base(), source.end());

    return source;
}

std::string StringUtility::Replace(std::string source, std::string from, std::string to)
{
    if (from.empty())
        return source;
    size_t start_pos = 0;
    while ((start_pos = source.find(from, start_pos)) != std::string::npos) {
        source.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }

    return source;
}
