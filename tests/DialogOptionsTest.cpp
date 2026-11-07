#include "DialogOptions.h"

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

} // namespace

int
main()
{
	TestDefaults();
	TestAllValues();
	TestInvalidValues();
	TestFormControls();
	std::cout << "DialogOptions tests passed\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
