#include "VoltageException.h"

#include <sstream>

VoltageException::VoltageException(int line, const char* file) :
	line(line),
	file(file)
{
}

const char* VoltageException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString() << std::endl;

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* VoltageException::GetType() const
{
	return "Voltage Exception";
}

int VoltageException::GetLine() const
{
	return line;
}

std::string VoltageException::GetOriginString() const
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}


