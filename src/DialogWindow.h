#ifndef HDIALOG_DIALOG_WINDOW_H
#define HDIALOG_DIALOG_WINDOW_H

#include <Window.h>

#include "DialogOptions.h"

class BMessageRunner;

class DialogWindow : public BWindow {
public:
	explicit DialogWindow(const DialogOptions& options);
	~DialogWindow() override;
	void MessageReceived(BMessage* message) override;
	bool QuitRequested() override;

private:
	void SendResult(int32 result);

	BMessageRunner* fTimeoutRunner;
	bool fResultSent;
};

#endif
