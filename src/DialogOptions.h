#ifndef HDIALOG_DIALOG_OPTIONS_H
#define HDIALOG_DIALOG_OPTIONS_H

#include <iosfwd>
#include <string>
#include <vector>

struct DialogOptions {
	std::string title = "hdialog";
	std::string message;
	std::string primaryButton = "OK";
	std::string secondaryButton;
	std::string iconPath;
	std::string inputLabel;
	std::string inputValue;
	std::string checkboxLabel;
	std::string selectedItem;
	std::vector<std::string> selectionItems;
	int timeoutSeconds = 0;
	int progress = -1;
	int width = 520;
	int height = 240;
	bool showHelp = false;
	bool passwordInput = false;
	bool checkboxChecked = false;
};

bool ParseDialogOptions(int argumentCount, const char* const arguments[],
	DialogOptions& options, std::string& error);
void PrintUsage(std::ostream& stream, const char* programName);

#endif
