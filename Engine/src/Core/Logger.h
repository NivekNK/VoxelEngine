#pragma once

#define SPDLOG_FMT_EXTERNAL
#define SPDLOG_LEVEL_NAMES \
    { "Trace", "Debug", "Info", "Warning", "Error", "Fatal", "Off" }
#define SPDLOG_SHORT_LEVEL_NAMES \
    { "T", "D", "I", "W", "E", "F", "O" }

#include <spdlog/spdlog.h>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#ifdef NK_DIST
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif

namespace nk {
    class Logger {
    public:
        static void Init(const std::string& pattern = GetDefaultPattern());

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() {
            return s_CoreLogger;
        }

        static std::shared_ptr<spdlog::logger>& GetClientLogger() {
            return s_ClientLogger;
        }

        static std::string GetDefaultPattern() { return "%^[%T] %n %l: %v%$"; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}  // namespace nk

#if LOG_WARN_ENABLED == 1
    #define CoreWarn(...) ::nk::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#else
    #define CoreWarn(...)
#endif

#if LOG_INFO_ENABLED == 1
    #define CoreInfo(...) ::nk::Logger::GetCoreLogger()->info(__VA_ARGS__)
#else
    #define CoreInfo(...)
#endif

#if LOG_DEBUG_ENABLED == 1
    #define CoreDebug(...) ::nk::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#else
    #define CoreDebug(...)
#endif

#if LOG_TRACE_ENABLED == 1
    #define CoreTrace(...) ::nk::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#else
    #define CoreTrace(...)
#endif

#define CoreError(...) ::nk::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define CoreFatal(...) ::nk::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros
#define Warn(...) ::nk::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define Info(...) ::nk::Logger::GetClientLogger()->info(__VA_ARGS__)
#define Debug(...) ::nk::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define Trace(...) ::nk::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define Error(...) ::nk::Logger::GetClientLogger()->error(__VA_ARGS__)
#define Fatal(...) ::nk::Logger::GetClientLogger()->critical(__VA_ARGS__)
