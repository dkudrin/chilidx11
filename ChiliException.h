#pragma once
#include <exception>
#include <string>

class ChiliException : public std::exception
{
public:
	ChiliException(int line, const char * file);
	const char * what() const override; // ��������������� ������������ �������  what �� std::exception
	virtual const char * GetType() const; // ����������� �������
	int GetLine() const;
	const std::string & GetFile() const;
	std::string GetOriginString() const;
private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};