#include "JsonOutput.h"

#include <sstream>

namespace {

std::string
EscapeJson(const std::string& value)
{
	std::ostringstream escaped;
	for (unsigned char character : value) {
		switch (character) {
			case '"':
				escaped << "\\\"";
				break;
			case '\\':
				escaped << "\\\\";
				break;
			case '\b':
				escaped << "\\b";
				break;
			case '\f':
				escaped << "\\f";
				break;
			case '\n':
				escaped << "\\n";
				break;
			case '\r':
				escaped << "\\r";
				break;
			case '\t':
				escaped << "\\t";
				break;
			default:
				if (character < 0x20) {
					const char hexadecimal[] = "0123456789abcdef";
					escaped << "\\u00" << hexadecimal[character >> 4]
						<< hexadecimal[character & 0x0f];
				} else
					escaped << character;
				break;
		}
	}
	return escaped.str();
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
AddStringProperty(std::ostringstream& output, bool& needsComma,
	const char* name, const std::string& value)
{
	if (needsComma)
		output << ',';
	output << '"' << EscapeJson(name) << "\":\"" << EscapeJson(value) << '"';
	needsComma = true;
}
//---------------------------------------------------------------------------------------------------------------------------------//

} // namespace

std::string
FormatJsonOutput(
	const std::vector<std::pair<std::string, std::string>>& fields,
	bool includeInput, const std::string& input, bool includeCheckbox,
	bool checked, bool includeSelection, const std::string& selection,
	bool includePath, const std::string& path)
{
	std::ostringstream output;
	output << "{\"fields\":{";
	bool fieldComma = false;
	for (const auto& field : fields)
		AddStringProperty(output, fieldComma, field.first.c_str(), field.second);
	output << '}';
	bool propertyComma = true;
	if (includeInput)
		AddStringProperty(output, propertyComma, "input", input);
	if (includeCheckbox) {
		output << ",\"checked\":" << (checked ? "true" : "false");
		propertyComma = true;
	}
	if (includeSelection)
		AddStringProperty(output, propertyComma, "selection", selection);
	if (includePath)
		AddStringProperty(output, propertyComma, "path", path);
	output << '}';
	return output.str();
}
//---------------------------------------------------------------------------------------------------------------------------------//
