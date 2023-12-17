#pragma once

#include <vector>
#include <string>

class CompressionUtility {
private:

public:
	static bool Compress(std::vector<char>& data, std::vector<char>& result);
	static bool Decompress(std::vector<char>& data, std::vector<char>& result);
};