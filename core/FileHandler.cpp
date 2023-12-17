#include "pch.h"
#include "FileHandler.h"

#include <fstream>
#include <sstream>

std::string FileHandler::ReadText(std::string path)
{
    std::ifstream file(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
}
