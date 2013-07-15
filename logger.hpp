#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdarg>
#include <string>

#include <fstream>
#include <ostream>

/* The Log_Level of a message indicates how important it is that a message
 * appear in the log. If the current logging level is set below the level of a
 * message, it won't be added to the log.
 */
enum Log_Level {
	EMERG, // An emergency condition
	ALERT, // A problem that requires immediate attention
	CRITICAL,
	ERROR,
	WARNING,
	NOTICE, // A normal, yet noteworthy condition
	INFO, // An information message
	DEBUG,
};

void open_log();

/* Writes an error message to the log */
void logger(const std::string & format,...);

void close_log();

class Log
{
	std::ofstream logFile;
	Log_Level verbosity;
	Log_Level current_priority;

public:
	Log(const std::string &filename, Log_Level verbosity = NOTICE);
	~Log();
	/* Write the message to the log, provided the current priority is lower than
	 * priority
	 */
	void operator()(const std::string &message, const Log_Level priority = DEBUG);
	Log& operator<<(const std::string &message);
	Log& operator<<(const char message[]);
	Log& operator<<(const long long int message);
	Log& operator<<(const Log_Level new_priority);
	
	Log_Level get_priority();
};

#endif