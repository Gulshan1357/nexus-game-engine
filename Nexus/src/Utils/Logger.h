#pragma once

#include <string>
#include <vector>

enum LogType
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

struct LogEntry
{
	LogType type;
	std::string message;
};

// Since this logger uses cout, it'll only print to console. It'll work for Rider but not for VS2022 because it is a Windows App not a Console App.
class Logger
{
public:
	static std::vector<LogEntry> messages;
	static void Log(const std::string& message);
	static void Warn(const std::string& message);
	static void Err(const std::string& message);
};