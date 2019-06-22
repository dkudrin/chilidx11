#pragma once
#include <exception>
#include <string>

class ChiliException : public std::exception
{
public:
	ChiliException(int line, const char * file);
	const char * what() const override; // переопределение станадартной функции  what из std::exception
	virtual const char * GetType() const; // виртуальная функция
	int GetLine() const;
	const std::string & GetFile() const;
	std::string GetOriginString() const;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};