#include "FluxPCH.h"
#include "StringUtils.h"

namespace Flux {

	std::string StringUtils::FormatBytes(size_t bytes)
	{
		constexpr size_t KB = 1024;     
		constexpr size_t MB = KB * 1024;
		constexpr size_t GB = MB * 1024;

		if (bytes >= GB)
			return fmt::format("{0} GB", bytes / GB);
		if (bytes >= MB)
			return fmt::format("{0} MB", bytes / MB);
		if (bytes >= KB)
			return fmt::format("{0} KB", bytes / KB);

		return fmt::format("{0} byte{1}", bytes, bytes != 1 ? "s" : "");
	}

	std::string StringUtils::ReplaceAll(std::string string, const std::string& from, const std::string& to)
	{
		size_t begin = 0;
		while ((begin = string.find(from, begin)) != std::string::npos)
		{
			string.replace(begin, from.length(), to);
			begin += to.length();
		}
		return string;
	}

	bool StringUtils::ContainsIgnoreCase(const std::string& string, const std::string& searchString)
	{
		return ToLowerString(string).find(ToLowerString(searchString)) != std::string::npos;
	}

	bool StringUtils::EqualsIgnoreCase(const std::string& string1, const std::string& string2)
	{
		return ToLowerString(string1) == ToLowerString(string2);
	}

	std::vector<std::string> StringUtils::SplitString(std::string s, const std::string& delimiter)
	{
		size_t begin = 0, end, delimiterLength = delimiter.length();
		std::string token;
		std::vector<std::string> result;

		while ((end = s.find(delimiter, begin)) != std::string::npos)
		{
			token = s.substr(begin, end - begin);
			begin = end + delimiterLength;
			result.push_back(token);
		}

		result.push_back(s.substr(begin));
		return result;
	}

	std::string StringUtils::ToLowerString(std::string string)
	{
		std::transform(string.begin(), string.end(), string.begin(), [](uint8 c)
		{
			return std::tolower(c);
		});
		return string;
	}

	std::string StringUtils::ToUpperString(std::string string)
	{
		std::transform(string.begin(), string.end(), string.begin(), [](uint8 c)
		{
			return std::toupper(c);
		});
		return string;
	}

	std::string StringUtils::VectorToString(const std::vector<std::string>& strings)
	{
		std::stringstream ss;
		for (size_t i = 0; i < strings.size(); i++)
		{
			if (i > 0)
				ss << ", ";
			ss << strings[i];
		}
		return ss.str();
	}

}