#pragma once

#include <spdlog/spdlog.h>
#include <map>

namespace Seele {
    class Log {
    public:
        enum class Type : uint8_t {
            Core = 0, App
        };

        enum class Level : uint8_t {
            Trace = 0, Info, Warn, Error, Fatal
        };

        struct TagDetails {
            bool Enabled = true;
            Level LevelFilter = Level::Trace;
        };

        static void Init(const std::string&logsDirectory = "Logs",
                         const std::string&coreLogsFileName = "Core.log",
                         const std::string&appLogsFileName = "App.log",
                         const std::string&editorConsoleLogsFileName = "EditorConsole.log", bool hasConsole = true,
                         std::size_t rotateMaxSize = 1024 * 1024 * 10 /* 10MB */, std::size_t rotateMaxFiles = 10);

        static void Shutdown();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; }
        static std::shared_ptr<spdlog::logger>& GetEditorConsoleLogger() { return s_EditorConsoleLogger; }

        static bool HasTag(const std::string&tag) { return s_EnabledTags.find(tag) != s_EnabledTags.end(); }
        static std::map<std::string, TagDetails>& EnabledTags() { return s_EnabledTags; }

        template<typename... Args>
        static void PrintMessage(Log::Type type, Log::Level level, std::string_view tag,
                                 const std::format_string<Args...>&format, Args&&... args);

        // Enum utils
        static const char* LevelToString(Level level) {
            switch (level) {
                case Level::Trace: return "Trace";
                case Level::Info: return "Info";
                case Level::Warn: return "Warn";
                case Level::Error: return "Error";
                case Level::Fatal: return "Fatal";
            }
            return "";
        }

        static Level LevelFromString(std::string_view string) {
            if (string == "Trace") return Level::Trace;
            if (string == "Info") return Level::Info;
            if (string == "Warn") return Level::Warn;
            if (string == "Error") return Level::Error;
            if (string == "Fatal") return Level::Fatal;

            return Level::Trace;
        }

        static bool IsInitialized() { return m_Initialized; }

        static void Flush() {
            if (m_Initialized) {
                s_CoreLogger->flush();
                s_AppLogger->flush();
                s_EditorConsoleLogger->flush();
            }
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_AppLogger;
        static std::shared_ptr<spdlog::logger> s_EditorConsoleLogger;

        inline static std::map<std::string, TagDetails> s_EnabledTags;

        static bool m_HasConsole;

        static bool m_Initialized;
    };
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging
template<typename... Args>
void SeeleCoreTraceTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Trace, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreInfoTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Info, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreWarnTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Warn, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreErrorTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Error, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreFatalTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Fatal, tag, format,
                               std::forward<Args>(args)...);
}

// App logging
template<typename... Args>
void SeeleTraceTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Trace, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleInfoTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Info, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleWarnTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Warn, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleErrorTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Error, tag, format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleFatalTag(const char* tag, const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Fatal, tag, format,
                               std::forward<Args>(args)...);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
template<typename... Args>
void SeeleCoreTrace(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Trace, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreInfo(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Info, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreWarn(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Warn, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreError(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Error, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleCoreFatal(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::Core, ::Seele::Log::Level::Fatal, "", format,
                               std::forward<Args>(args)...);
}

// App Logging
template<typename... Args>
void SeeleTrace(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Trace, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleInfo(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Info, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleWarn(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Warn, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleError(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Error, "", format,
                               std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleFatal(const std::format_string<Args...>&format, Args&&... args) {
    ::Seele::Log::PrintMessage(::Seele::Log::Type::App, ::Seele::Log::Level::Fatal, "", format,
                               std::forward<Args>(args)...);
}

// Editor Console Logging Macros
template<typename... Args>
void SeeleEditorConsoleTrace(Args&&... args) {
    Seele::Log::GetEditorConsoleLogger()->trace(std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleEditorConsoleInfo(Args&&... args) {
    Seele::Log::GetEditorConsoleLogger()->info(std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleEditorConsoleWarn(Args&&... args) {
    Seele::Log::GetEditorConsoleLogger()->warn(std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleEditorConsoleError(Args&&... args) {
    Seele::Log::GetEditorConsoleLogger()->error(std::forward<Args>(args)...);
}

template<typename... Args>
void SeeleEditorConsoleFatal(Args&&... args) {
    Seele::Log::GetEditorConsoleLogger()->critical(std::forward<Args>(args)...);
}

class SeeleLog {
private:
    SeeleLog(const std::string&logsDirectory = "Logs",
             const std::string&coreLogsFileName = "Core.log",
             const std::string&appLogsFileName = "App.log",
             const std::string&editorConsoleLogsFileName = "EditorConsole.log", bool hasConsole = true,
             std::size_t rotateMaxSize = 1024 * 1024 * 10 /* 10MB */, std::size_t rotateMaxFiles = 10) {
        Seele::Log::Init(logsDirectory, coreLogsFileName, appLogsFileName, editorConsoleLogsFileName, hasConsole,
                         rotateMaxSize, rotateMaxFiles);
    }

public:
    static SeeleLog& Create(const std::string&logsDirectory = "Logs",
                            const std::string&coreLogsFileName = "Core.log",
                            const std::string&appLogsFileName = "App.log",
                            const std::string&editorConsoleLogsFileName = "EditorConsole.log", bool hasConsole = true,
                            std::size_t rotateMaxSize = 1024 * 1024 * 10 /* 10MB */, std::size_t rotateMaxFiles = 10) {
        static SeeleLog instance(logsDirectory, coreLogsFileName, appLogsFileName, editorConsoleLogsFileName,
                                 hasConsole,
                                 rotateMaxSize, rotateMaxFiles);
        return instance;
    }

    ~SeeleLog() {
        Seele::Log::Shutdown();
    }
};

namespace Seele {
    template<typename... Args>
    void Log::PrintMessage(Log::Type type, Log::Level level, std::string_view tag,
                           const std::format_string<Args...>&format,
                           Args&&... args) {
        auto detail = s_EnabledTags[std::string(tag)];
        if (detail.Enabled && detail.LevelFilter <= level) {
            auto logger = (type == Type::Core) ? GetCoreLogger() : GetAppLogger();
            auto userLogString = std::format(format, std::forward<Args>(args)...);
            auto finaLogString = tag.empty()
                                     ? std::format("{}{}", tag, userLogString)
                                     : std::format("[{}] {}", tag, userLogString);
            switch (level) {
                case Level::Trace:
                    logger->trace(finaLogString);
                    break;
                case Level::Info:
                    logger->info(finaLogString);
                    break;
                case Level::Warn:
                    logger->warn(finaLogString);
                    break;
                case Level::Error:
                    logger->error(finaLogString);
                    break;
                case Level::Fatal:
                    logger->critical(finaLogString);
                    break;
            }
        }
    }
}
