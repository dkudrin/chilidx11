#include "ChiliException.h"
#include <sstream>
#include <iostream>

ChiliException::ChiliException(int line, const char * file)
:line(line), file(file)
{}

const char * ChiliException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * ChiliException::GetType() const
{
	return "Chili Exception";
}

int ChiliException::GetLine() const
{
	return line;
}

const std::string& ChiliException::GetFile() const
{
	return file;
}

std::string ChiliException::GetOriginString() const
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}