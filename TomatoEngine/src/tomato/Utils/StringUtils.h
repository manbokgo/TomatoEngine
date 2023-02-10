#pragma once
#include <Windows.h>

namespace tomato
{
	class StringUtils
	{
	public:
		[[nodiscard]] static std::string GetExtension(std::string_view filepath)
		{
			auto lastDot = filepath.find_last_of(".");
			return static_cast<std::string>(filepath.substr(lastDot + 1, filepath.size() - lastDot));
		}

		[[nodiscard]] static std::string GetName(std::string_view filepath)
		{
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			return static_cast<std::string>(filepath.substr(lastSlash, count));
		}

		[[nodiscard]] static std::string GetNameWithExtension(std::string_view filepath)
		{
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			return static_cast<std::string>(filepath.substr(lastSlash, filepath.size()));
		}

		static void ReplaceString(std::string& subject, std::string_view search, std::string_view replace)
		{
			size_t pos = 0;
			while ((pos = subject.find(search, pos)) != std::string::npos)
			{
				subject.replace(pos, search.length(), replace);
				pos += replace.length();
			}
		}
		
		static std::string WstringToString(const std::wstring& input)
		{
			char      multiByteStr[MAX_PATH];
			const int requiredLength = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, nullptr, 0, nullptr, nullptr);
			WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, multiByteStr, requiredLength, nullptr, nullptr);

			return multiByteStr;
		}

		static std::wstring StringToWstring(const std::string& input)
		{
			wchar_t   wideStr[MAX_PATH];
			const int requiredLength = MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, nullptr, 0);
			MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, wideStr, requiredLength);

			return wideStr;
		}

		static void WcharToChar(const wchar_t* input, char* output, size_t length)
		{
			const int requiredLength = WideCharToMultiByte(CP_ACP, 0, input, -1, nullptr, 0, nullptr, nullptr);
			if (length < requiredLength)
			{
				assert(false);
			}
			WideCharToMultiByte(CP_ACP, 0, input, -1, output, requiredLength, nullptr, nullptr);
		}

		static void CharToWchar(const char* input, wchar_t* output, size_t length)
		{
			const int requiredLength = MultiByteToWideChar(CP_ACP, 0, input, -1, nullptr, 0);
			if (length < requiredLength)
			{
				assert(false);
			}
			MultiByteToWideChar(CP_ACP, 0, input, -1, output, requiredLength);
		}
	};
}
