#pragma once

#include <vector>
#include <string>

class StringUtility {
private:

public:
	static std::vector<std::string> SplitString(std::string source, std::string token);
	static std::string Trim(std::string source);
	static std::string TrimChar(std::string source, unsigned char ch);
};