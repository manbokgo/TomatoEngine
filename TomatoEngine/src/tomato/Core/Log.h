#pragma once

#include <filesystem>

#include "tomato/Core/UUID.h"

namespace tomato
{
    class Log
    {
    public:
        enum Level : uint32_t
        {
            Trace = 1,
            Debug = 2,
            Info = 4,
            Warn = 8,
            Error = 16,
            Critical = 32,
        };

    public:
        static void Init();

        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };
}

template <>
struct fmt::formatter<tomato::UUID>
{
    constexpr auto parse(const format_parse_context& ctx) const -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const tomato::UUID& input, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", static_cast<uint64_t>(input));
    }
};

template <>
struct fmt::formatter<std::filesystem::path>
{
    constexpr auto parse(format_parse_context& ctx) const -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const std::filesystem::path& input, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", input.string());
    }
};

#undef FMT

#define TOMATO_CORE_TRACE(...)		::tomato::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TOMATO_CORE_INFO(...)		::tomato::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TOMATO_CORE_DEBUG(...)		::tomato::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define TOMATO_CORE_WARN(...)		::tomato::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TOMATO_CORE_ERROR(...)		::tomato::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TOMATO_CORE_CRITICAL(...)	::tomato::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define TOMATO_APP_TRACE(...)		::tomato::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TOMATO_APP_INFO(...)		::tomato::Log::GetClientLogger()->info(__VA_ARGS__)
#define TOMATO_APP_DEBUG(...)		::tomato::Log::GetClientLogger()->debug(__VA_ARGS__)
#define TOMATO_APP_WARN(...)		::tomato::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TOMATO_APP_ERROR(...)		::tomato::Log::GetClientLogger()->error(__VA_ARGS__)
#define TOMATO_APP_CRITICAL(...)	::tomato::Log::GetClientLogger()->critical(__VA_ARGS__)

#define TOMATO_APP_TRACE_EXTERNAL(file, line, function, ...)			::tomato::Log::GetClientLogger()->log(spdlog::source_loc{ file, line, function }, spdlog::level::trace, __VA_ARGS__)
#define TOMATO_APP_INFO_EXTERNAL(file, line, function, ...)			::tomato::Log::GetClientLogger()->log(spdlog::source_loc{ file, line, function }, spdlog::level::info, __VA_ARGS__)
#define TOMATO_APP_DEBUG_EXTERNAL(file, line, function, ...)			::tomato::Log::GetClientLogger()->log(spdlog::source_loc{ file, line, function }, spdlog::level::debug, __VA_ARGS__)
#define TOMATO_APP_WARN_EXTERNAL(file, line, function, ...)			::tomato::Log::GetClientLogger()->log(spdlog::source_loc{ file, line, function }, spdlog::level::warn, __VA_ARGS__)
#define TOMATO_APP_ERROR_EXTERNAL(file, line, function, ...)			::tomato::Log::GetClientLogger()->log(spdlog::source_loc{ file, line, function }, spdlog::level::err, __VA_ARGS__)
#define TOMATO_APP_CRITICAL_EXTERNAL(file, line, function, ...)		::tomato::Log::GetClientLogger()->log(spdlog::source_loc{ file, line, function }, spdlog::level::critical, __VA_ARGS__)
