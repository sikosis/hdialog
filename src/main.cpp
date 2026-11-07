#include <iostream>
#include <string>

#include "DialogApplication.h"
#include "DialogOptions.h"

int
main(int argumentCount, char** arguments)
{
	DialogOptions options;
	std::string error;
	if (!ParseDialogOptions(argumentCount, arguments, options, error)) {
		std::cerr << "hdialog: " << error << "\n\n";
		PrintUsage(std::cerr, arguments[0]);
		return 64;
	}
	if (options.showHelp) {
		PrintUsage(std::cout, arguments[0]);
		return 0;
	}

	DialogApplication application(options);
	application.Run();
	if (application.Result() == 0 && !options.inputLabel.empty())
		std::cout << application.Input() << '\n';
	if (application.Result() == 0 && !options.checkboxLabel.empty()) {
		std::cout << "checked="
			<< (application.CheckboxChecked() ? "true" : "false") << '\n';
	}
	if (application.Result() == 0 && !options.selectionItems.empty())
		std::cout << "selection=" << application.Selection() << '\n';
	return application.Result();
}
//---------------------------------------------------------------------------------------------------------------------------------//
