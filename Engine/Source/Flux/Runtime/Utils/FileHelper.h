#pragma once

namespace Flux {

	namespace FileHelper {

		bool LoadFileToBinaryU8(std::vector<uint8>& outBinary, const std::filesystem::path& path);
		bool SaveBinaryToFileU8(const std::vector<uint8>& binary, const std::filesystem::path& path);

		bool LoadFileToBinaryU16(std::vector<uint16>& outBinary, const std::filesystem::path& path);
		bool SaveBinaryToFileU16(const std::vector<uint16>& binary, const std::filesystem::path& path);

		bool LoadFileToBinaryU32(std::vector<uint32>& outBinary, const std::filesystem::path& path);
		bool SaveBinaryToFileU32(const std::vector<uint32>& binary, const std::filesystem::path& path);

		bool LoadFileToString(std::string& outString, const std::filesystem::path& path);
		bool SaveStringToFile(const std::string& string, const std::filesystem::path& path);

	}

}