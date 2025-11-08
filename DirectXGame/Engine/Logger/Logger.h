#pragma once
#include <spdlog/spdlog.h>
#include <string>

namespace Logger {

	enum LoggerFlag : uint32_t {
		None = 0,
		MakeFile = 1 << 0,
		UseConsole = 1 << 1,
	};

	void Initialize();

	std::shared_ptr<spdlog::logger> getLogger(const std::string& name, uint32_t flug = MakeFile | UseConsole);

}
