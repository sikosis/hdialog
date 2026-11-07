#include "DialogOptions.h"
#include "JsonOutput.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void
Expect(bool condition, const char* description)
{
	if (condition)
		return;
	std::cerr << "FAIL: " << description << '\n';
	std::exit(1);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
TestDefaults()
{
	const char* arguments[] = { "hdialog" };
	DialogOptions options;
	std::string error;
	Expect(ParseDialogOptions(1, arguments, options, error), "default parse");
	Expect(options.title == "hdialog", "default title");
	Expect(options.primaryButton == "OK", "default primary button");
	Expect(options.timeoutSeconds == 0, "default timeout disabled");
	Expect(options.iconPath == "info" && options.iconSize == 96,
		"default built-in icon");
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
TestAllValues()
{
	const char* arguments[] = { "hdialog", "--title=Update", "--message",
		"Ready", "--button1text", "Install", "--button2text=Later",
		"--timeout", "30", "--width", "640", "--height=320",
		"--textfield", "Name", "--value=Arthur", "--progress", "25" };
	DialogOptions options;
	std::string error;
	Expect(ParseDialogOptions(17, arguments, options, error), "custom parse");
	Expect(options.title == "Update", "custom title");
	Expect(options.message == "Ready", "custom message");
	Expect(options.primaryButton == "Install", "custom primary button");
	Expect(options.secondaryButton == "Later", "custom secondary button");
	Expect(options.timeoutSeconds == 30, "custom timeout");
	Expect(options.width == 640 && options.height == 320, "custom dimensions");
	Expect(options.inputLabel == "Name" && options.inputValue == "Arthur",
		"custom input");
	Expect(options.progress == 25, "custom progress");
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
TestInvalidValues()
{
	const char* unknown[] = { "hdialog", "--wat", "value" };
	DialogOptions options;
	std::string error;
	Expect(!ParseDialogOptions(3, unknown, options, error),
		"unknown option rejected");

	const char* smallWidth[] = { "hdialog", "--width", "100" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(3, smallWidth, options, error),
		"small width rejected");

	const char* emptyButton[] = { "hdialog", "--button1text=" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(2, emptyButton, options, error),
		"empty primary button rejected");

	const char* largeProgress[] = { "hdialog", "--progress", "101" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(3, largeProgress, options, error),
		"large progress rejected");

	const char* smallIcon[] = { "hdialog", "--iconsize", "16" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(3, smallIcon, options, error),
		"small icon rejected");
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
TestFormControls()
{
	const char* arguments[] = { "hdialog", "--textfield", "Password:",
		"--password", "--checkbox", "Remember", "--checked", "--select",
		"Alpha", "--select=Beta", "--selected", "Beta" };
	DialogOptions options;
	std::string error;
	Expect(ParseDialogOptions(12, arguments, options, error),
		"form controls parse");
	Expect(options.passwordInput, "password enabled");
	Expect(options.checkboxLabel == "Remember" && options.checkboxChecked,
		"checked checkbox");
	Expect(options.selectionItems.size() == 2, "selection item count");
	Expect(options.selectedItem == "Beta", "initial selection");

	const char* badPassword[] = { "hdialog", "--password" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(2, badPassword, options, error),
		"password without text field rejected");

	const char* badSelection[] = { "hdialog", "--select", "Alpha",
		"--selected", "Beta" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(5, badSelection, options, error),
		"unknown initial selection rejected");
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
TestNamedFieldsAndJson()
{
	const char* arguments[] = { "hdialog", "--field", "username=User name:",
		"--passwordfield=password=Password:", "--fieldvalue",
		"username=Arthur", "--json" };
	DialogOptions options;
	std::string error;
	Expect(ParseDialogOptions(7, arguments, options, error),
		"named fields parse");
	Expect(options.fields.size() == 2, "named field count");
	Expect(options.fields[0].name == "username"
		&& options.fields[0].value == "Arthur", "named field value");
	Expect(options.fields[1].password, "named password field");
	Expect(options.jsonOutput, "JSON output enabled");

	std::vector<std::pair<std::string, std::string>> fields = {
		{ "user\"name", "Arthur\nDent" }, { "path", "C:\\Temp" }
	};
	const std::string json = FormatJsonOutput(fields, true, "Tea", true,
		false, true, "Home", true, "/boot/home/file.txt");
	Expect(json == "{\"fields\":{\"user\\\"name\":\"Arthur\\nDent\","
		"\"path\":\"C:\\\\Temp\"},\"input\":\"Tea\",\"checked\":false,"
		"\"selection\":\"Home\",\"path\":\"/boot/home/file.txt\"}",
		"JSON escaping and structure");
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
TestPickers()
{
	const char* fileArguments[] = { "hdialog", "--filepicker", "Document:",
		"--path", "/boot/home" };
	DialogOptions options;
	std::string error;
	Expect(ParseDialogOptions(5, fileArguments, options, error),
		"file picker parses");
	Expect(options.filePicker && options.pickerLabel == "Document:",
		"file picker values");
	Expect(options.initialPath == "/boot/home", "initial picker path");

	const char* conflicting[] = { "hdialog", "--filepicker", "File:",
		"--folderpicker", "Folder:" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(5, conflicting, options, error),
		"conflicting pickers rejected");

	const char* orphanPath[] = { "hdialog", "--path", "/boot/home" };
	options = DialogOptions();
	error.clear();
	Expect(!ParseDialogOptions(3, orphanPath, options, error),
		"path without picker rejected");
}
//---------------------------------------------------------------------------------------------------------------------------------//

} // namespace

int
main()
{
	TestDefaults();
	TestAllValues();
	TestInvalidValues();
	TestFormControls();
	TestNamedFieldsAndJson();
	TestPickers();
	std::cout << "DialogOptions tests passed\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
