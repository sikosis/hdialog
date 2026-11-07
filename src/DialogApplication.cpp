#include "DialogApplication.h"

#include <Message.h>

#include "DialogWindow.h"
#include "Messages.h"

DialogApplication::DialogApplication(const DialogOptions& options)
	:
	BApplication("application/x-vnd.hdialog"),
	fOptions(options),
	fResult(kResultSecondary)
{
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogApplication::MessageReceived(BMessage* message)
{
	if (message->what == kMessageResult) {
		int32 result;
		if (message->FindInt32("result", &result) == B_OK)
			fResult = result;
		PostMessage(B_QUIT_REQUESTED);
		return;
	}
	BApplication::MessageReceived(message);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogApplication::ReadyToRun()
{
	(new DialogWindow(fOptions))->Show();
}
//---------------------------------------------------------------------------------------------------------------------------------//


int
DialogApplication::Result() const
{
	return fResult;
}
//---------------------------------------------------------------------------------------------------------------------------------//
