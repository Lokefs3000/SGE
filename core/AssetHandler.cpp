#include "pch.h"
#include "AssetHandler.h"

#include <filesystem>
#include <sstream>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "DebugMessenger.h"

#include "CompressionUtility.h"
#include "DataParser.h"
#include "MemoryUtilities.h"

#include "Texture.h"

std::string AssetHandler::GetPackingFileData(std::string& file)
{
	auto f = file.find_last_of(".");
	if (f != std::string::npos) {
		std::string	ext = file.substr(f + 1);

		if (ext == "sge") {
			//compress sge files
		}
	}

	std::ifstream stream(file, std::ios::binary);
	std::stringstream data;
	data << stream.rdbuf();
	stream.close();
	return data.str();
}

void AssetHandler::ReadFromEntry(PakFileTableEntry asset, std::vector<char>& out)
{
	std::vector<char> fileBuffer;
	fileBuffer.resize(asset.CompressedSize);
	m_PackageStream.seekg(asset.Offset);
	m_PackageStream.read(fileBuffer.data(), asset.CompressedSize);

	if (asset.Compressed) {
		bool ret = CompressionUtility::Decompress(fileBuffer, out);
		if (!ret) {
			DebugError("Failed to decompress asset: ", asset.FilePath, "! Resulting data is most likely unusable.");
		}
	}
	else {
		out = fileBuffer;
	}
}

AssetHandler::AssetHandler(std::string path)
{
	if (!std::filesystem::exists(path)) {
		path = "data.pak";
	}

	m_IsPackaged = false;

#ifndef __EMSCRIPTEN__
	if (std::filesystem::exists(path)) {
		m_IsPackaged = !std::filesystem::is_directory(path);

		if (!m_IsPackaged) {
			for (std::filesystem::path entry : std::filesystem::recursive_directory_iterator(path))
			{
				if (std::filesystem::is_directory(entry))
					continue; //skip directories!

				PakFileTableEntry asset{};
				memcpy(asset.FilePath, entry.string().c_str(), std::min(entry.string().size(), (size_t)255));
				asset.Compressed = false;
				asset.UncompressedSize = 1;
				asset.CompressedSize = 1;

				m_PackageAssets.insert(std::make_pair(entry.string().substr(path.size()), asset));
			}
		}
		else
		{
#else
		m_IsPackaged = true;
#endif
			m_PackageStream = std::ifstream(path, std::ios::binary | std::ios::ate);

			std::vector<char> headerData;
			headerData.resize(sizeof(PakHeader));
			m_PackageStream.seekg(0);
			m_PackageStream.read(headerData.data(), sizeof(PakHeader));

			PakHeader header{};
			memcpy(&header, headerData.data(), sizeof(PakHeader));

			PakHeader fresh{}; //Fresh will ALWAYS have the right ID
			if (header.ID[0] != fresh.ID[0] && header.ID[1] != fresh.ID[1] && header.ID[2] != fresh.ID[2] && header.ID[3] != fresh.ID[3]) {
				FatalError("Invalid package!", "The supplied package file does not have the right ID! Supplied: ", std::string(header.ID), ", required: ", std::string(header.ID));
			}
			if (header.PakVersion != PAK_VERSION) {
				FatalError("Invalid package!", "The supplied package file does not have the right version! Supplied: ", header.PakVersion, ", required: ", PAK_VERSION);
			}

			headerData.resize(sizeof(PakFileTableEntry) * header.NumEntries);
			m_PackageStream.seekg(sizeof(PakHeader));
			m_PackageStream.read(headerData.data(), sizeof(PakFileTableEntry) * header.NumEntries);

			size_t offset = 0;
			for (size_t i = 0; i < header.NumEntries; i++)
			{
				PakFileTableEntry entry;
				memcpy(&entry, headerData.data() + offset, sizeof(PakFileTableEntry));
				offset += sizeof(PakFileTableEntry);

				m_PackageAssets.insert(std::make_pair(entry.FilePath, entry));
			}
#ifndef __EMSCRIPTEN__
		}
	}
	else {
		FatalError("No resources!", "No available resource package file or folder was found!");
	}
#endif
}

AssetHandler::~AssetHandler()
{
	if (m_PackageStream.is_open()) {
		m_PackageStream.close();
	}
}

void AssetHandler::PackAssets(std::string path, bool compress)
{
	if (std::filesystem::exists(path) && std::filesystem::is_directory(path) && std::filesystem::exists(path + "\\config.sge")) {
		DebugInfo("Starting asset packing. Using version: ", PAK_VERSION);
		
		PakHeader header{};
		header.PakVersion = PAK_VERSION;
		
		std::vector<char> dataBuffer;
		std::vector<PakFileTableEntry> fileEntries;

		for (std::filesystem::path entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (std::filesystem::is_directory(entry))
				continue; //skip directories!

			std::ifstream fileStream(entry.string(), std::ios::ate | std::ios::binary);
			if (fileStream.fail()) {
				DebugError("Failed to open file: ", entry.string(), "!");
				continue;
			}

			PakFileTableEntry pakEntry{};
			pakEntry.UncompressedSize = fileStream.tellg();
			pakEntry.Offset = dataBuffer.size();
			pakEntry.Compressed = (pakEntry.UncompressedSize > 4000000u && compress);

			std::string file = entry.string().substr(path.size() + 1).c_str();
			memcpy(pakEntry.FilePath, file.c_str(), std::min(file.size(), (size_t)255));

			fileStream.seekg(0);

			std::vector<char> fileData;
			fileData.resize(pakEntry.UncompressedSize);
			fileStream.read(fileData.data(), pakEntry.UncompressedSize);

			if (pakEntry.Compressed) {
				std::vector<char> compressedFileData;
				bool ret = CompressionUtility::Compress(fileData, compressedFileData);
				if (!ret) {
					DebugWarning("Failed to compress file! Falling back to uncompressed.");

					pakEntry.Compressed = false;
					pakEntry.CompressedSize = pakEntry.UncompressedSize;
					dataBuffer.insert(dataBuffer.end(), fileData.begin(), fileData.end());
				}

				pakEntry.CompressedSize = compressedFileData.size();
				dataBuffer.insert(dataBuffer.end(), compressedFileData.data(), compressedFileData.data() + compressedFileData.size());
			}
			else {
				pakEntry.Compressed = false;
				pakEntry.CompressedSize = pakEntry.UncompressedSize;
				dataBuffer.insert(dataBuffer.end(), fileData.begin(), fileData.end());
			}

			fileEntries.push_back(pakEntry);
			fileStream.close();
			header.NumEntries++;

			DebugInfo("Packaged file: ", pakEntry.FilePath, ", Compressed: ", pakEntry.Compressed ? "true" : "false", ", UncompressedSize: ", pakEntry.UncompressedSize, ", CompressedSize: ", pakEntry.CompressedSize, ", Offset: ", pakEntry.Offset);
		}

		DebugInfo("Creating package header data.");

		std::vector<char> headBuffer;
		headBuffer.resize(sizeof(PakHeader) + (sizeof(PakFileTableEntry) * fileEntries.size()));

		/*Header*/
		memcpy(headBuffer.data(), &header, sizeof(PakHeader));

		size_t offset = sizeof(PakHeader);
		for (size_t i = 0; i < fileEntries.size(); i++)
		{
			fileEntries[i].Offset += headBuffer.size();
			memcpy(headBuffer.data() + offset, &fileEntries[i], sizeof(PakFileTableEntry));
			offset += sizeof(PakFileTableEntry);
		}

		DebugInfo("Writing package data to file \"data.pack\".");

		std::ofstream pakStream("data.pak", std::ios::binary);
		pakStream.write(headBuffer.data(), headBuffer.size());
		pakStream.write(dataBuffer.data(), dataBuffer.size());

		pakStream.close();

		DebugInfo("Packed: ", header.NumEntries, " files.");
	}
}

PakFileTableEntry& AssetHandler::GetAsset(std::string name)
{
	if (m_PackageAssets.count(name))
		return m_PackageAssets.at(name);

	DebugWarning("Could not find asset: " + name);

	PakFileTableEntry asset;
	return asset;
}

std::string AssetHandler::GetSource(std::string path)
{
	PakFileTableEntry asset = GetAsset(path);
	if (asset.UncompressedSize == 0)
		return "";

	if (m_IsPackaged) {
		std::vector<char> dataBuffer;
		ReadFromEntry(asset, dataBuffer);
		
		return dataBuffer.data();
	}
	else {
		std::ifstream data(asset.FilePath);
		std::stringstream stream;
		stream << data.rdbuf();
		data.close();

		return stream.str();
	}

	return "";
}

std::shared_ptr<Texture> AssetHandler::GetTexture(std::string path, Filtering filtering, Repeating repeating)
{
	return std::shared_ptr<Texture>(GetTextureRaw(path, filtering, repeating));
}

Texture* AssetHandler::GetTextureRaw(std::string path, Filtering filtering, Repeating repeating)
{
	PakFileTableEntry asset = GetAsset(path);
	if (asset.UncompressedSize == 0)
		return NULL;

	if (m_IsPackaged) {
		std::vector<char> dataBuffer;
		ReadFromEntry(asset, dataBuffer);

		int w, h, ch;
		stbi_uc* src = stbi_load_from_memory((stbi_uc*)dataBuffer.data(), dataBuffer.size(), &w, &h, &ch, 0);

		Format format;
		switch (ch)
		{
		case 1:
			format = Format::R;
			break;
		case 2:
			format = Format::RG;
			break;
		case 3:
		default:
			format = Format::RGB;
			break;
		case 4:
			format = Format::RGBA;
			break;
		}

		Texture* tex = new Texture(src, w, h, filtering, repeating, format);

		stbi_image_free(src);
		return tex;
	}
	else {
		int w, h, ch;
		stbi_uc* src = stbi_load(asset.FilePath, &w, &h, &ch, 0);

		Format format;
		switch (ch)
		{
		case 1:
			format = Format::R;
			break;
		case 2:
			format = Format::RG;
			break;
		case 3:
		default:
			format = Format::RGB;
			break;
		case 4:
			format = Format::RGBA;
			break;
		}

		Texture* tex = new Texture(src, w, h, filtering, repeating, format);

		stbi_image_free(src);
		return tex;
	}

	return NULL;
}
