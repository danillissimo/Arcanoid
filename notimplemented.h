#pragma once

#include <exception>

class _NotImplementedException : std::exception {
private:
	const char *message, *fileName;
	int lineNumber;
public:
	_NotImplementedException(const char* message, const char* fileName, int lineNumber) :
		message(message),
		fileName(fileName),
		lineNumber(lineNumber)
	{}

	const char* what() const noexcept OVERRIDE { return message; }
};

#define NotImplementedException(message) _NotImplementedException(message, __FILE__, __LINE__)