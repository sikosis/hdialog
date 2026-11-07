#ifndef HDIALOG_JSON_OUTPUT_H
#define HDIALOG_JSON_OUTPUT_H

#include <string>
#include <utility>
#include <vector>

std::string FormatJsonOutput(
	const std::vector<std::pair<std::string, std::string>>& fields,
	bool includeInput, const std::string& input, bool includeCheckbox,
	bool checked, bool includeSelection, const std::string& selection);

#endif
