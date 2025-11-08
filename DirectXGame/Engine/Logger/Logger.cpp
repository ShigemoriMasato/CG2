#include "Logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <filesystem>

namespace fs = std::filesystem;

namespace {

    void archiveOldLogs(const std::string& logDir, size_t maxFiles) {
        namespace fs = std::filesystem;
        std::vector<fs::directory_entry> logFiles;

        for (const auto& entry : fs::directory_iterator(logDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".log") {
                logFiles.push_back(entry);
            }
        }

        std::sort(logFiles.begin(), logFiles.end(),
            [](const auto& a, const auto& b) {
                return fs::last_write_time(a) < fs::last_write_time(b);
            });

        if (logFiles.size() > maxFiles) {
            fs::create_directories(logDir + "/archive");
            for (size_t i = 0; i < logFiles.size() - maxFiles; ++i) {
                fs::rename(logFiles[i], logDir + "/archive/" + logFiles[i].path().filename().string());
            }
        }

    }
}

void Logger::Initialize() {
#ifdef SH_DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::info);
#endif

    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
}

std::shared_ptr<spdlog::logger> Logger::getLogger(const std::string& name, uint32_t flug) {
    auto existing = spdlog::get(name);
    if (existing) return existing;

    std::vector<spdlog::sink_ptr> sinks;

    if (flug & MakeFile) {
        std::filesystem::create_directories("Logs/" + name);

        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &t);

        std::ostringstream fileName;
        fileName << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".log";

        std::string logDir = "Logs/" + name + "/" + fileName.str();

        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDir, true));

        archiveOldLogs("Logs/" + name, 5);
    }

    if (flug & UseConsole) {
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    spdlog::register_logger(logger);

    return logger;
}
