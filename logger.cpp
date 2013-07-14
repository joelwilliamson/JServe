#include <cstdio>
#include <cstdarg>

#include "logger.hpp"

void open_log()
{
	// Nothing to do here since I am logging to stdout.
}

void logger ( const std::string & format, ... )
{
	va_list ap;
	va_start(ap,format);
	vfprintf(stderr,format.c_str(),ap);
	fflush(stderr);
	va_end(ap);
}

void close_log()
{
	// Nothing to do.
}

Log::Log(const std::string& filename, Log_Level verbosity)
	: current_level(verbosity)
{
	logFile.open(filename.c_str());
}

Log::~Log()
{
	logFile.close();
}

void Log::operator()(const std::string& message, const Log_Level priority)
{
	if (priority <= current_level) {
		logFile << message << std::endl;
		logFile.flush();
	}
}

