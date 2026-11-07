#ifndef HDIALOG_DIALOG_APPLICATION_H
#define HDIALOG_DIALOG_APPLICATION_H

#include <Application.h>

#include <utility>
#include <vector>

#include "DialogOptions.h"

class DialogWindow;

class DialogApplication : public BApplication {
public:
	explicit DialogApplication(const DialogOptions& options);
	status_t GetSupportedSuites(BMessage* data) override;
	void MessageReceived(BMessage* message) override;
	void ReadyToRun() override;
	BHandler* ResolveSpecifier(BMessage* message, int32 index,
		BMessage* specifier, int32 form, const char* property) override;
	int Result() const;
	const std::string& Input() const;
	bool CheckboxChecked() const;
	const std::string& Selection() const;
	const std::vector<std::pair<std::string, std::string>>& Fields() const;

private:
	void HandleScriptingMessage(BMessage* message);
	void SendWindowString(uint32 command, const char* value);

	DialogOptions fOptions;
	DialogWindow* fWindow;
	std::string fInput;
	std::string fSelection;
	std::vector<std::pair<std::string, std::string>> fFields;
	bool fCheckboxChecked;
	int fResult;
};

#endif
