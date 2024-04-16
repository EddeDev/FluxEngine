#pragma once

namespace Flux {

	namespace StringUtils {

		std::string FormatBytes(size_t bytes);

		std::string ReplaceAll(std::string string, const std::string& from, const std::string& to);

		bool ContainsIgnoreCase(const std::string& string, const std::string& searchString);
		bool EqualsIgnoreCase(const std::string& string1, const std::string& string2);

		std::vector<std::string> SplitString(std::string s, const std::string& delimiter);
		std::string ToLowerString(std::string string);
		std::string ToUpperString(std::string string);
		std::string VectorToString(const std::vector<std::string>& strings);

		template<typename T>
		std::string VectorToString(const std::vector<T>& values)
		{
			std::vector<std::string> strings;
			for (auto& value : values)
				strings.push_back(std::to_string(value));
			return VectorToString(strings);
		}

	}

}