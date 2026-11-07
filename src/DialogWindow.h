#ifndef HDIALOG_DIALOG_WINDOW_H
#define HDIALOG_DIALOG_WINDOW_H

#include <Window.h>

#include <string>
#include <vector>

#include "DialogOptions.h"

class BMessageRunner;
class BFilePanel;
class BCheckBox;
class BMenuField;
class BStatusBar;
class BTextControl;
class BTextView;

struct NamedFieldControl {
	std::string name;
	BTextControl* control;
};

class DialogWindow : public BWindow {
public:
	explicit DialogWindow(const DialogOptions& options);
	~DialogWindow() override;
	void MessageReceived(BMessage* message) override;
	bool QuitRequested() override;

private:
	void SendResult(int32 result);

	BMessageRunner* fTimeoutRunner;
	BFilePanel* fFilePanel;
	BCheckBox* fCheckbox;
	BMenuField* fSelectionField;
	BStatusBar* fProgressBar;
	BTextControl* fInputControl;
	BTextControl* fPathControl;
	BTextView* fMessageView;
	std::vector<NamedFieldControl> fFieldControls;
	bool fResultSent;
};

#endif
