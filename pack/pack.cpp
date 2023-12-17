#include <iostream>

#include <AssetHandler.h>
#include <string>

int main(int argc, char* argv[])
{
    if (argc > 1) {
        bool compress = false;
        if (std::string(argv[2]) == "true")
            compress = true;
        
        AssetHandler::PackAssets(argv[1], compress);
    }

    return 0;
}