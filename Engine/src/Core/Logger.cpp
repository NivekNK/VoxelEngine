#include "nkpch.h"

#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace nk {
    std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

    void Logger::Init(const std::string& pattern) {
        spdlog::set_pattern(std::move(pattern));

        s_CoreLogger = spdlog::stdout_color_mt("NK");
        s_CoreLogger->set_level(spdlog::level::trace);
        const auto coreLoggerSink =
            dynamic_cast<spdlog::sinks::stdout_color_sink_mt*>(
                s_CoreLogger->sinks().back().get()
            );

        s_ClientLogger = spdlog::stderr_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace);
        const auto clientLoggerSink =
            dynamic_cast<spdlog::sinks::stderr_color_sink_mt*>(
                s_ClientLogger->sinks().back().get()
            );
    }
}  // namespace nk
