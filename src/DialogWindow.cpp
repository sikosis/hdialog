#include "DialogWindow.h"

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <GroupLayout.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <MessageRunner.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Screen.h>
#include <StatusBar.h>
#include <TextControl.h>
#include <TextView.h>
#include <TranslationUtils.h>

#include "IconView.h"
#include "Messages.h"

DialogWindow::DialogWindow(const DialogOptions& options)
	:
	BWindow(BRect(0, 0, options.width - 1, options.height - 1),
		options.title.c_str(), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE),
	fTimeoutRunner(nullptr),
	fCheckbox(nullptr),
	fSelectionField(nullptr),
	fProgressBar(nullptr),
	fInputControl(nullptr),
	fMessageView(nullptr),
	fResultSent(false)
{
	fMessageView = new BTextView("message");
	fMessageView->SetText(options.message.c_str());
	fMessageView->MakeEditable(false);
	fMessageView->MakeSelectable(true);
	fMessageView->SetWordWrap(true);
	fMessageView->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);

	BGroupView* detailGroup = new BGroupView(B_VERTICAL,
		B_USE_DEFAULT_SPACING);
	detailGroup->GroupLayout()->AddView(fMessageView);
	if (!options.inputLabel.empty()) {
		fInputControl = new BTextControl("input", options.inputLabel.c_str(),
			options.inputValue.c_str(), nullptr);
		if (options.passwordInput)
			fInputControl->TextView()->HideTyping(true);
		detailGroup->GroupLayout()->AddView(fInputControl);
	}
	if (!options.checkboxLabel.empty()) {
		fCheckbox = new BCheckBox("checkbox", options.checkboxLabel.c_str(),
			nullptr);
		fCheckbox->SetValue(options.checkboxChecked
			? B_CONTROL_ON : B_CONTROL_OFF);
		detailGroup->GroupLayout()->AddView(fCheckbox);
	}
	if (!options.selectionItems.empty()) {
		BPopUpMenu* menu = new BPopUpMenu("selection");
		for (const std::string& itemText : options.selectionItems) {
			BMenuItem* item = new BMenuItem(itemText.c_str(), nullptr);
			menu->AddItem(item);
			if (itemText == options.selectedItem)
				item->SetMarked(true);
		}
		if (menu->FindMarked() == nullptr)
			menu->ItemAt(0)->SetMarked(true);
		fSelectionField = new BMenuField("selection", "Choose:", menu);
		detailGroup->GroupLayout()->AddView(fSelectionField);
	}
	if (options.progress >= 0) {
		fProgressBar = new BStatusBar("progress");
		fProgressBar->SetMaxValue(100);
		fProgressBar->SetTo(options.progress);
		detailGroup->GroupLayout()->AddView(fProgressBar);
	}

	BGroupView* contentGroup = new BGroupView(B_HORIZONTAL,
		B_USE_DEFAULT_SPACING);
	if (!options.iconPath.empty()) {
		BBitmap* bitmap = BTranslationUtils::GetBitmap(options.iconPath.c_str());
		if (bitmap != nullptr)
			contentGroup->GroupLayout()->AddView(new IconView(bitmap));
	}
	contentGroup->GroupLayout()->AddView(detailGroup);

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
		.Add(contentGroup)
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
		case kMessageSetTitle:
		{
			const char* title;
			if (message->FindString("value", &title) == B_OK)
				SetTitle(title);
			break;
		}
		case kMessageSetMessage:
		{
			const char* text;
			if (message->FindString("value", &text) == B_OK)
				fMessageView->SetText(text);
			break;
		}
		case kMessageSetProgress:
		{
			float value;
			if (fProgressBar != nullptr
				&& message->FindFloat("value", &value) == B_OK) {
				fProgressBar->SetTo(value);
			}
			break;
		}
		case kMessageGetInput:
		{
			BMessage reply(B_REPLY);
			reply.AddString("result", fInputControl != nullptr
				? fInputControl->Text() : "");
			message->SendReply(&reply);
			break;
		}
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
	if (fInputControl != nullptr)
		message.AddString("input", fInputControl->Text());
	if (fCheckbox != nullptr)
		message.AddBool("checked", fCheckbox->Value() == B_CONTROL_ON);
	if (fSelectionField != nullptr
		&& fSelectionField->Menu()->FindMarked() != nullptr) {
		message.AddString("selection",
			fSelectionField->Menu()->FindMarked()->Label());
	}
	be_app_messenger.SendMessage(&message);
}
//---------------------------------------------------------------------------------------------------------------------------------//
