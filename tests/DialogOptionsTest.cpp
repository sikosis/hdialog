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
		"--timeout", "30", "--width", "640", "--height=320" };
	DialogOptions options;
	std::string error;
	Expect(ParseDialogOptions(12, arguments, options, error), "custom parse");
	Expect(options.title == "Update", "custom title");
	Expect(options.message == "Ready", "custom message");
	Expect(options.primaryButton == "Install", "custom primary button");
	Expect(options.secondaryButton == "Later", "custom secondary button");
	Expect(options.timeoutSeconds == 30, "custom timeout");
	Expect(options.width == 640 && options.height == 320, "custom dimensions");
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
}
//---------------------------------------------------------------------------------------------------------------------------------//

} // namespace

int
main()
{
	TestDefaults();
	TestAllValues();
	TestInvalidValues();
	std::cout << "DialogOptions tests passed\n";
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------//
