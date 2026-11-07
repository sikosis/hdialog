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


bool
SplitAssignment(const std::string& value, std::string& name,
	std::string& assignedValue)
{
	const size_t equals = value.find('=');
	if (equals == std::string::npos || equals == 0)
		return false;
	name = value.substr(0, equals);
	assignedValue = value.substr(equals + 1);
	return !assignedValue.empty();
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
		if (argument == "--password") {
			options.passwordInput = true;
			continue;
		}
		if (argument == "--checked") {
			options.checkboxChecked = true;
			continue;
		}
		if (argument == "--json") {
			options.jsonOutput = true;
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
		else if (name == "--iconsize") {
			if (!ParseInteger(value, 32, options.iconSize)
				|| options.iconSize > 256) {
				error = "--iconsize must be between 32 and 256";
				return false;
			}
		}
		else if (name == "--textfield")
			options.inputLabel = value;
		else if (name == "--value")
			options.inputValue = value;
		else if (name == "--checkbox")
			options.checkboxLabel = value;
		else if (name == "--select")
			options.selectionItems.push_back(value);
		else if (name == "--selected")
			options.selectedItem = value;
		else if (name == "--filepicker") {
			options.filePicker = true;
			options.pickerLabel = value;
		} else if (name == "--folderpicker") {
			options.folderPicker = true;
			options.pickerLabel = value;
		} else if (name == "--path")
			options.initialPath = value;
		else if (name == "--field" || name == "--passwordfield") {
			NamedFieldOptions field;
			if (!SplitAssignment(value, field.name, field.label)) {
				error = std::string(name) + " must use NAME=LABEL";
				return false;
			}
			field.password = name == "--passwordfield";
			options.fields.push_back(field);
		} else if (name == "--fieldvalue") {
			std::string fieldName;
			std::string fieldValue;
			if (!SplitAssignment(value, fieldName, fieldValue)) {
				error = "--fieldvalue must use NAME=VALUE";
				return false;
			}
			bool found = false;
			for (NamedFieldOptions& field : options.fields) {
				if (field.name == fieldName) {
					field.value = fieldValue;
					found = true;
				}
			}
			if (!found) {
				error = "--fieldvalue must follow its --field";
				return false;
			}
		}
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
	if (options.passwordInput && options.inputLabel.empty()) {
		error = "--password requires --textfield";
		return false;
	}
	if (options.checkboxChecked && options.checkboxLabel.empty()) {
		error = "--checked requires --checkbox";
		return false;
	}
	if (options.filePicker && options.folderPicker) {
		error = "--filepicker and --folderpicker cannot be combined";
		return false;
	}
	if (!options.initialPath.empty()
		&& !options.filePicker && !options.folderPicker) {
		error = "--path requires --filepicker or --folderpicker";
		return false;
	}
	if (!options.selectedItem.empty()) {
		bool selectionExists = false;
		for (const std::string& item : options.selectionItems) {
			if (item == options.selectedItem)
				selectionExists = true;
		}
		if (!selectionExists) {
			error = "--selected must match a --select item";
			return false;
		}
	}
	for (size_t index = 0; index < options.fields.size(); index++) {
		for (size_t other = index + 1; other < options.fields.size(); other++) {
			if (options.fields[index].name == options.fields[other].name) {
				error = "field names must be unique";
				return false;
			}
		}
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
		<< "  --icon ICON          Built-in name, image path, or none\n"
		<< "  --iconsize PIXELS    Icon size from 32 to 256 (default: 64)\n"
		<< "  --textfield LABEL    Show a labelled text input\n"
		<< "  --value TEXT         Initial text-field value\n"
		<< "  --password           Hide characters entered in the text field\n"
		<< "  --checkbox LABEL     Show a checkbox\n"
		<< "  --checked            Initially check the checkbox\n"
		<< "  --select ITEM        Add a selection item (may be repeated)\n"
		<< "  --selected ITEM      Initially selected item\n"
		<< "  --filepicker LABEL   Choose a file with BFilePanel\n"
		<< "  --folderpicker LABEL Choose a folder with BFilePanel\n"
		<< "  --path PATH          Initial picker path\n"
		<< "  --field NAME=LABEL   Add a named text field (may be repeated)\n"
		<< "  --passwordfield NAME=LABEL  Add a named password field\n"
		<< "  --fieldvalue NAME=VALUE     Set a named field's initial value\n"
		<< "  --json               Write submitted values as JSON\n"
		<< "  --progress VALUE     Show progress from 0 to 100\n"
		<< "  --timeout SECONDS    Close after a positive number of seconds\n"
		<< "  --width PIXELS       Window width (minimum: 320)\n"
		<< "  --height PIXELS      Window height (minimum: 160)\n"
		<< "  --help               Show this help\n";
}
//---------------------------------------------------------------------------------------------------------------------------------//
