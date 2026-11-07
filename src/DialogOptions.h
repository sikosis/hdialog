#ifndef HDIALOG_DIALOG_OPTIONS_H
#define HDIALOG_DIALOG_OPTIONS_H

#include <iosfwd>
#include <string>

struct DialogOptions {
	std::string title = "hdialog";
	std::string message;
	std::string primaryButton = "OK";
	std::string secondaryButton;
	int timeoutSeconds = 0;
	int width = 520;
	int height = 240;
	bool showHelp = false;
};

bool ParseDialogOptions(int argumentCount, const char* const arguments[],
	DialogOptions& options, std::string& error);
void PrintUsage(std::ostream& stream, const char* programName);

#endif
