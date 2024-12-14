#include "stdafx.h"
#include "Logger.h"

#include <chrono>
#include <iostream>
//#include <windows.h> // For OutputDebugStringA

#define GRN "\x1b[32m"
#define WHT "\033[0m"
#define RED "\x1b[91m"
#define YEL "\x1b[33m"

std::vector<LogEntry> Logger::messages;

std::string CurrentDateTimeToString()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm localTime;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&localTime, &now); // Thread-safe on Windows
#else
    localtime_r(&now, &localTime); // Thread-safe on POSIX systems
#endif
    
    std::string output(30, '\0');
    std::strftime(output.data(), output.size(), "%d-%b-%Y %H:%M:%S", &localTime);
    return output;
}

void Logger::Log(const std::string& message)
{
    const std::string formattedMessage = "Log: [" + CurrentDateTimeToString() + "]: " + message;
    const LogEntry logEntry{ LOG_INFO, formattedMessage };
    std::cout << GRN << logEntry.message << WHT << "\n";
    messages.push_back(logEntry);

    //// Send the message to the Output window in Visual Studio
    //std::string debugMessage = logEntry.message + "\n";
    //OutputDebugStringA(debugMessage.c_str());
    //OutputDebugStringA("Test message from Logger\n");
}

void Logger::Warn(const std::string& message)
{
    const LogEntry logEntry{ LOG_ERROR, "Warning: [" + CurrentDateTimeToString() + "]: " + message };
    std::cout << YEL << logEntry.message << WHT << "\n";
    messages.push_back(logEntry);
}

void Logger::Err(const std::string& message)
{
    const LogEntry logEntry{ LOG_ERROR, "Error: [" + CurrentDateTimeToString() + "]: " + message };
    std::cout << RED << logEntry.message << WHT << "\n";
    messages.push_back(logEntry);
}


