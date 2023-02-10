#include "pch.h"
#include "ExternalConsoleSink.h"

namespace tomato
{
	std::function<void(std::string_view, const char*, const char*, int32_t, Log::Level)> ExternalConsoleSink::OnFlush;
}
