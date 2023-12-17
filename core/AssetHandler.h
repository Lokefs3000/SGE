#pragma once

#include <fstream>
#include <unordered_map>

#include "globals.h"

enum class Filtering;
enum class Repeating;
class Texture;

struct PakHeader {
	char ID[4] = { "PAK" };
	uint32_t PakVersion = 0;
	uint32_t NumEntries = 0;
};

struct PakFileTableEntry {
	char FilePath[255];
	bool Compressed = false;
	uint32_t UncompressedSize = 0;
	uint32_t CompressedSize = 0;
	uint32_t Offset = 0;
};

class AssetHandler {
private:
	std::string m_PackageLocation;

	std::ifstream m_PackageStream;

	bool m_IsPackaged;

	std::unordered_map<std::string, PakFileTableEntry> m_PackageAssets;

	static std::string GetPackingFileData(std::string& file);
	void ReadFromEntry(PakFileTableEntry asset, std::vector<char>& out);
public:
	AssetHandler(std::string path);
	~AssetHandler();

	EXPORT static void PackAssets(std::string path, bool compress);

	PakFileTableEntry& GetAsset(std::string name);
	std::string GetSource(std::string path);
	std::shared_ptr<Texture> GetTexture(std::string path, Filtering filtering, Repeating repeating);

	Texture* GetTextureRaw(std::string path, Filtering filtering, Repeating repeating);
};