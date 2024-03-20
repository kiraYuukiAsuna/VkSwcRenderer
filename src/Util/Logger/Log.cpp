#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "EditorConsoleSink.h"
#include <filesystem>

namespace Seele {
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_AppLogger;
    std::shared_ptr<spdlog::logger> Log::s_EditorConsoleLogger;

    bool Log::m_HasConsole = true;

    void Log::Init(const std::string &logsDirectory,
                   const std::string &coreLogsFileName,
                   const std::string &appLogsFileName,
                   const std::string &editorConsoleLogsFileName, bool hasConsole) {
        m_HasConsole = hasConsole;

        // Create "logs" directory if doesn't exist
        std::string coreLogsFile = logsDirectory + "/" + coreLogsFileName;
        std::string appLogsFile = logsDirectory + "/" + appLogsFileName;
        std::string editorConsoleLogsFile = logsDirectory + "/" + editorConsoleLogsFileName;

        if (!std::filesystem::exists(logsDirectory)) {
            std::filesystem::create_directories(logsDirectory);
        }

        int maxSize = 1024 * 1024 * 10; // 10MB
        int maxFiles = 10; // 10Files

        std::vector<spdlog::sink_ptr> coreSinks =
                {
                        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(coreLogsFile, maxSize, maxFiles),
                };
        if (m_HasConsole) {
            coreSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }

        std::vector<spdlog::sink_ptr> appSinks =
                {
                        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(appLogsFile, maxSize, maxFiles),
                };
        if (m_HasConsole) {
            appSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }

        std::vector<spdlog::sink_ptr> editorConsoleSinks =
                {
                        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(editorConsoleLogsFile, maxSize,
                                                                               maxFiles),
                };
        if (m_HasConsole) {
            editorConsoleSinks.push_back(std::make_shared<EditorConsoleSink>(1));
        }

        coreSinks[0]->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %n: %v");
        appSinks[0]->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %n: %v");

        if (m_HasConsole) {
            coreSinks[1]->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%l] %n: %v%$");
            appSinks[1]->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%l] %n: %v%$");
            for (const auto &sink: editorConsoleSinks)
                sink->set_pattern("%^%v%$");
        }

        s_CoreLogger = std::make_shared<spdlog::logger>("VkSwcRenderer-Core", coreSinks.begin(), coreSinks.end());
        s_CoreLogger->set_level(spdlog::level::trace);

        s_AppLogger = std::make_shared<spdlog::logger>("VkSwcRenderer-App", appSinks.begin(), appSinks.end());
        s_AppLogger->set_level(spdlog::level::trace);

        s_EditorConsoleLogger = std::make_shared<spdlog::logger>("VkSwcRenderer-EditorConsole", editorConsoleSinks.begin(),
                                                                 editorConsoleSinks.end());
        s_EditorConsoleLogger->set_level(spdlog::level::trace);
    }

    void Log::Shutdown() {
        s_EditorConsoleLogger.reset();
        s_AppLogger.reset();
        s_CoreLogger.reset();
        spdlog::drop_all();
    }
}