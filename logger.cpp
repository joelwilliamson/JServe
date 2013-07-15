#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <chrono>

#include "logger.hpp"

void open_log()
	{
	// Nothing to do here since I am logging to stdout.
	}

void logger ( const std::string & format, ... )
	{
	va_list ap;
	va_start ( ap,format );
	vfprintf ( stderr,format.c_str(),ap );
	fflush ( stderr );
	va_end ( ap );
	}

void close_log()
	{
	// Nothing to do.
	}

Log::Log ( const std::string& filename, Log_Level verbosity )
	: verbosity ( verbosity )
	{
	using std::chrono::system_clock;
	logFile.open ( filename.c_str(), std::ios::app | std::ios::out);
	time_t tt = system_clock::to_time_t(system_clock::now());
	logFile << "Opened at: " << ctime(&tt) <<std::endl;
	}

Log::~Log()
	{
	using std::chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());
	logFile << "Closed at: " << ctime(&tt);
	logFile.flush();
	logFile.close();
	}

void Log::operator() ( const std::string& message, const Log_Level priority ) {
	if ( priority <= verbosity ) {
		logFile << message << std::endl;
		logFile.flush();
		}
	}

Log& Log::operator<< ( const std::string& message )
	{
	if ( current_priority <= verbosity )
		{
		logFile << message;
		logFile.flush();
		}
	return *this;
	}

Log& Log::operator<< ( const char message[] )
	{
	if ( current_priority <= verbosity )
		{
		logFile << message;
		logFile.flush();
		}
	return *this;
	}

Log& Log::operator<< ( const long long int message )
	{
	if ( current_priority <= verbosity )
		{
		logFile << message;
		logFile.flush();
		}
	return *this;
	}

Log& Log::operator<< ( const Log_Level new_priority )
	{
	current_priority = new_priority;
	return *this;
	}
	
Log_Level Log::get_priority()
	{
	return current_priority;
	}
