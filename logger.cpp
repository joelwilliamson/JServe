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
	logFile.open ( filename.c_str() );
	}

Log::~Log()
	{
	logFile.close();
	}

void Log::operator() ( const std::string& message, const Log_Level priority )
	{
	if ( priority <= verbosity )
		{
		logFile << message;
		logFile.flush();
		}
		return *this;
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

Log& Log::operator<< ( const int message )
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
