#ifndef HDIALOG_DIALOG_APPLICATION_H
#define HDIALOG_DIALOG_APPLICATION_H

#include <Application.h>

#include "DialogOptions.h"

class DialogApplication : public BApplication {
public:
	explicit DialogApplication(const DialogOptions& options);
	void MessageReceived(BMessage* message) override;
	void ReadyToRun() override;
	int Result() const;

private:
	DialogOptions fOptions;
	int fResult;
};

#endif
