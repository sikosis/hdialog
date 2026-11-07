#include "DialogOptions.h"

#include <charconv>
#include <ostream>
#include <string_view>

namespace {

bool
ParseInteger(std::string_view text, int minimum, int& value)
{
	int parsedValue = 0;
	const char* end = text.data() + text.size();
	auto result = std::from_chars(text.data(), end, parsedValue);
	if (result.ec != std::errc() || result.ptr != end || parsedValue < minimum)
		return false;
	value = parsedValue;
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool
ReadValue(int& index, int argumentCount, const char* const arguments[],
	std::string_view option, bool hasInlineValue, std::string_view inlineValue,
	std::string& value, std::string& error)
{
	if (hasInlineValue) {
		value = inlineValue;
		return true;
	}
	if (index + 1 >= argumentCount) {
		error = "missing value for " + std::string(option);
		return false;
	}
	value = arguments[++index];
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//

} // namespace

bool
ParseDialogOptions(int argumentCount, const char* const arguments[],
	DialogOptions& options, std::string& error)
{
	for (int index = 1; index < argumentCount; index++) {
		std::string_view argument(arguments[index]);
		if (argument == "--help" || argument == "-h") {
			options.showHelp = true;
			continue;
		}
		if (argument.rfind("--", 0) != 0) {
			error = "unexpected argument: " + std::string(argument);
			return false;
		}

		const size_t equals = argument.find('=');
		const std::string_view name = argument.substr(0, equals);
		const bool hasInlineValue = equals != std::string_view::npos;
		const std::string_view inlineValue = hasInlineValue
			? argument.substr(equals + 1) : std::string_view();
		std::string value;
		if (!ReadValue(index, argumentCount, arguments, name, hasInlineValue,
			inlineValue, value, error)) {
			return false;
		}

		if (name == "--title")
			options.title = value;
		else if (name == "--message")
			options.message = value;
		else if (name == "--button1text")
			options.primaryButton = value;
		else if (name == "--button2text")
			options.secondaryButton = value;
		else if (name == "--icon")
			options.iconPath = value;
		else if (name == "--textfield")
			options.inputLabel = value;
		else if (name == "--value")
			options.inputValue = value;
		else if (name == "--progress") {
			if (!ParseInteger(value, 0, options.progress)
				|| options.progress > 100) {
				error = "--progress must be between 0 and 100";
				return false;
			}
		}
		else if (name == "--timeout") {
			if (!ParseInteger(value, 1, options.timeoutSeconds)) {
				error = "--timeout must be a positive whole number";
				return false;
			}
		} else if (name == "--width") {
			if (!ParseInteger(value, 320, options.width)) {
				error = "--width must be at least 320";
				return false;
			}
		} else if (name == "--height") {
			if (!ParseInteger(value, 160, options.height)) {
				error = "--height must be at least 160";
				return false;
			}
		} else {
			error = "unknown option: " + std::string(name);
			return false;
		}
	}
	if (options.primaryButton.empty()) {
		error = "--button1text cannot be empty";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
PrintUsage(std::ostream& stream, const char* programName)
{
	stream << "Usage: " << programName << " [options]\n\n"
		<< "  --title TEXT         Window title (default: hdialog)\n"
		<< "  --message TEXT       Main dialog message\n"
		<< "  --button1text TEXT   Primary button label (default: OK)\n"
		<< "  --button2text TEXT   Secondary button label\n"
		<< "  --icon PATH          Image displayed beside the message\n"
		<< "  --textfield LABEL    Show a labelled text input\n"
		<< "  --value TEXT         Initial text-field value\n"
		<< "  --progress VALUE     Show progress from 0 to 100\n"
		<< "  --timeout SECONDS    Close after a positive number of seconds\n"
		<< "  --width PIXELS       Window width (minimum: 320)\n"
		<< "  --height PIXELS      Window height (minimum: 160)\n"
		<< "  --help               Show this help\n";
}
//---------------------------------------------------------------------------------------------------------------------------------//
