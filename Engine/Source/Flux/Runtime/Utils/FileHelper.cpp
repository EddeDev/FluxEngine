#include "FluxPCH.h"
#include "FileHelper.h"

namespace Flux {

	bool FileHelper::LoadFileToBinaryU8(std::vector<uint8>& outBinary, const std::filesystem::path& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
			return false;

		in.seekg(0, std::ios::end);
		std::streampos size = in.tellg();
		in.seekg(0, std::ios::beg);
		outBinary.resize(size);
		in.read((char*)outBinary.data(), size);

		in.close();
		return true;
	}

	bool FileHelper::SaveBinaryToFileU8(const std::vector<uint8>& binary, const std::filesystem::path& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);
		if (out.is_open())
		{
			out.write((char*)binary.data(), binary.size());
			out.flush();
			out.close();
			return true;
		}
		return false;
	}

	bool FileHelper::LoadFileToBinaryU16(std::vector<uint16>& outBinary, const std::filesystem::path& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
			return false;

		in.seekg(0, std::ios::end);
		std::streampos size = in.tellg();
		in.seekg(0, std::ios::beg);
		outBinary.resize(size / sizeof(uint16));
		in.read((char*)outBinary.data(), size);

		in.close();
		return true;
	}

	bool FileHelper::SaveBinaryToFileU16(const std::vector<uint16>& binary, const std::filesystem::path& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);
		if (out.is_open())
		{
			out.write((char*)binary.data(), binary.size() * sizeof(uint16));
			out.flush();
			out.close();
			return true;
		}
		return false;
	}

	bool FileHelper::LoadFileToBinaryU32(std::vector<uint32>& outBinary, const std::filesystem::path& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
			return false;

		in.seekg(0, std::ios::end);
		std::streampos size = in.tellg();
		in.seekg(0, std::ios::beg);
		outBinary.resize(size / sizeof(uint32));
		in.read((char*)outBinary.data(), size);

		in.close();
		return true;
	}

	bool FileHelper::SaveBinaryToFileU32(const std::vector<uint32>& binary, const std::filesystem::path& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);
		if (out.is_open())
		{
			out.write((char*)binary.data(), binary.size() * sizeof(uint32));
			out.flush();
			out.close();
			return true;
		}
		return false;
	}

	bool FileHelper::LoadFileToString(std::string& outString, const std::filesystem::path& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (!in)
			return false;

		in.seekg(0, std::ios::end);
		std::streampos size = in.tellg();
		in.seekg(0, std::ios::beg);
		outString.resize(size);
		in.read(outString.data(), size);

		in.close();
		return true;
	}

	bool FileHelper::SaveStringToFile(const std::string& string, const std::filesystem::path& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);
		if (out.is_open())
		{
			out.write(string.data(), string.size());
			out.flush();
			out.close();
			return true;
		}
		return false;
	}

}