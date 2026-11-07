#include "DialogWindow.h"

#include <Application.h>
#include <Button.h>
#include <GroupLayout.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Screen.h>
#include <TextView.h>

#include "Messages.h"

DialogWindow::DialogWindow(const DialogOptions& options)
	:
	BWindow(BRect(0, 0, options.width - 1, options.height - 1),
		options.title.c_str(), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE),
	fTimeoutRunner(nullptr),
	fResultSent(false)
{
	BTextView* messageView = new BTextView("message");
	messageView->SetText(options.message.c_str());
	messageView->MakeEditable(false);
	messageView->MakeSelectable(true);
	messageView->SetWordWrap(true);
	messageView->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	BButton* primaryButton = new BButton("primary",
		options.primaryButton.c_str(), new BMessage(kMessagePrimary));
	primaryButton->MakeDefault(true);
	BGroupView* buttonGroup = new BGroupView(B_HORIZONTAL);
	BLayoutBuilder::Group<>(buttonGroup)
		.AddGlue();
	if (!options.secondaryButton.empty()) {
		buttonGroup->GroupLayout()->AddView(new BButton("secondary",
			options.secondaryButton.c_str(), new BMessage(kMessageSecondary)));
	}
	buttonGroup->GroupLayout()->AddView(primaryButton);

	BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_DEFAULT_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(messageView)
		.Add(buttonGroup);

	BScreen screen(this);
	BRect screenFrame = screen.Frame();
	MoveTo(screenFrame.left + (screenFrame.Width() - Frame().Width()) / 2,
		screenFrame.top + (screenFrame.Height() - Frame().Height()) / 2);

	if (options.timeoutSeconds > 0) {
		fTimeoutRunner = new BMessageRunner(BMessenger(this),
			new BMessage(kMessageTimeout), options.timeoutSeconds * 1000000LL, 1);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//


DialogWindow::~DialogWindow()
{
	delete fTimeoutRunner;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMessagePrimary:
			SendResult(kResultPrimary);
			break;
		case kMessageSecondary:
			SendResult(kResultSecondary);
			break;
		case kMessageTimeout:
			SendResult(kResultTimeout);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool
DialogWindow::QuitRequested()
{
	if (!fResultSent)
		SendResult(kResultSecondary);
	return true;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogWindow::SendResult(int32 result)
{
	if (fResultSent)
		return;
	fResultSent = true;
	BMessage message(kMessageResult);
	message.AddInt32("result", result);
	be_app_messenger.SendMessage(&message);
}
//---------------------------------------------------------------------------------------------------------------------------------//
