#pragma once

#include <exception>
#include <string>

class VoltageException : std::exception
{
private:
	const int line;
	const std::string file;

public:
	VoltageException(int line, const char* file);
	virtual const char* what() const override;
	virtual const char* GetType() const;
	int GetLine() const;
	std::string GetOriginString() const;

protected:
	mutable std::string whatBuffer;
};

