#include "DialogWindow.h"

#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <CheckBox.h>
#include <Entry.h>
#include <FilePanel.h>
#include <GroupLayout.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <MessageRunner.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <NodeInfo.h>
#include <PopUpMenu.h>
#include <Path.h>
#include <Screen.h>
#include <StatusBar.h>
#include <StringView.h>
#include <TextControl.h>
#include <TextView.h>
#include <TranslationUtils.h>

#include <cerrno>
#include <cstdlib>

#include "IconView.h"
#include "Messages.h"

DialogWindow::DialogWindow(const DialogOptions& options)
	:
	BWindow(BRect(0, 0, options.width - 1, options.height - 1),
		options.title.c_str(), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE),
	fTimeoutRunner(nullptr),
	fPrimaryButton(nullptr),
	fFilePanel(nullptr),
	fCheckbox(nullptr),
	fSelectionField(nullptr),
	fProgressBar(nullptr),
	fInputControl(nullptr),
	fPathControl(nullptr),
	fMessageView(nullptr),
	fValidationMessage(nullptr),
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
	for (const NamedFieldOptions& field : options.fields) {
		BTextControl* control = new BTextControl(field.name.c_str(),
			field.label.c_str(), field.value.c_str(), nullptr);
		if (field.password)
			control->TextView()->HideTyping(true);
		control->TextView()->SetModificationMessage(
			new BMessage(kMessageValidate));
		detailGroup->GroupLayout()->AddView(control);
		fFieldControls.push_back({ field.name, control, field });
	}
	for (const NamedFieldOptions& field : options.fields) {
		if (field.required || field.numeric || field.hasMinimum
			|| field.hasMaximum) {
			fValidationMessage = new BStringView("validation", "");
			fValidationMessage->SetHighColor(190, 45, 45);
			detailGroup->GroupLayout()->AddView(fValidationMessage);
			break;
		}
	}
	if (options.filePicker || options.folderPicker) {
		fPathControl = new BTextControl("path", options.pickerLabel.c_str(),
			options.initialPath.c_str(), nullptr);
		BButton* chooseButton = new BButton("choose", "Choose…",
			new BMessage(kMessageChoosePath));
		BGroupView* pickerGroup = new BGroupView(B_HORIZONTAL,
			B_USE_DEFAULT_SPACING);
		pickerGroup->GroupLayout()->AddView(fPathControl);
		pickerGroup->GroupLayout()->AddView(chooseButton);
		detailGroup->GroupLayout()->AddView(pickerGroup);

		BMessenger target(this);
		const uint32 nodeFlavours = options.folderPicker
			? B_DIRECTORY_NODE : B_FILE_NODE;
		fFilePanel = new BFilePanel(B_OPEN_PANEL, &target, nullptr,
			nodeFlavours, false, new BMessage(kMessagePathSelected));
		if (!options.initialPath.empty())
			fFilePanel->SetPanelDirectory(options.initialPath.c_str());
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
	if (!options.iconPath.empty() && options.iconPath != "none") {
		const BuiltInIconKind kind = BuiltInIconForName(options.iconPath.c_str());
		if (kind != BuiltInIconKind::None) {
			contentGroup->GroupLayout()->AddView(new IconView(kind,
				options.iconSize));
		} else {
			BBitmap* bitmap = BTranslationUtils::GetBitmap(
				options.iconPath.c_str());
			if (bitmap == nullptr) {
				entry_ref reference;
				if (get_ref_for_path(options.iconPath.c_str(), &reference) == B_OK) {
					bitmap = new BBitmap(BRect(0, 0, 31, 31), B_RGBA32);
					if (bitmap->InitCheck() != B_OK
						|| BNodeInfo::GetTrackerIcon(&reference, bitmap,
							B_LARGE_ICON) != B_OK) {
						delete bitmap;
						bitmap = nullptr;
					}
				}
			}
			if (bitmap != nullptr) {
				contentGroup->GroupLayout()->AddView(new IconView(bitmap,
					options.iconSize));
			}
		}
	}
	contentGroup->GroupLayout()->AddView(detailGroup);

	fPrimaryButton = new BButton("primary",
		options.primaryButton.c_str(), new BMessage(kMessagePrimary));
	fPrimaryButton->MakeDefault(true);
	BGroupView* buttonGroup = new BGroupView(B_HORIZONTAL);
	BLayoutBuilder::Group<>(buttonGroup)
		.AddGlue();
	if (!options.secondaryButton.empty()) {
		buttonGroup->GroupLayout()->AddView(new BButton("secondary",
			options.secondaryButton.c_str(), new BMessage(kMessageSecondary)));
	}
	buttonGroup->GroupLayout()->AddView(fPrimaryButton);

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
	ValidateFields();
}
//---------------------------------------------------------------------------------------------------------------------------------//


DialogWindow::~DialogWindow()
{
	delete fTimeoutRunner;
	delete fFilePanel;
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
		case kMessageChoosePath:
			if (fFilePanel != nullptr)
				fFilePanel->Show();
			break;
		case kMessagePathSelected:
		{
			entry_ref reference;
			if (fPathControl != nullptr
				&& message->FindRef("refs", &reference) == B_OK) {
				BPath path(&reference);
				if (path.InitCheck() == B_OK)
					fPathControl->SetText(path.Path());
			}
			break;
		}
		case kMessageValidate:
			ValidateFields();
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
	if (fInputControl != nullptr)
		message.AddString("input", fInputControl->Text());
	for (const NamedFieldControl& field : fFieldControls) {
		message.AddString("field_name", field.name.c_str());
		message.AddString("field_value", field.control->Text());
	}
	if (fCheckbox != nullptr)
		message.AddBool("checked", fCheckbox->Value() == B_CONTROL_ON);
	if (fSelectionField != nullptr
		&& fSelectionField->Menu()->FindMarked() != nullptr) {
		message.AddString("selection",
			fSelectionField->Menu()->FindMarked()->Label());
	}
	if (fPathControl != nullptr)
		message.AddString("path", fPathControl->Text());
	be_app_messenger.SendMessage(&message);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogWindow::ValidateFields()
{
	if (fPrimaryButton == nullptr)
		return;

	std::string error;
	for (const NamedFieldControl& field : fFieldControls) {
		const std::string value = field.control->Text();
		if (field.options.required && value.empty()) {
			error = field.options.label + " is required.";
			break;
		}
		if (value.empty())
			continue;
		if (field.options.numeric) {
			errno = 0;
			char* end = nullptr;
			const double number = std::strtod(value.c_str(), &end);
			if (errno != 0 || end == value.c_str() || *end != '\0') {
				error = field.options.label + " must be a number.";
				break;
			}
			if (field.options.hasMinimum && number < field.options.minimum) {
				error = field.options.label + " is below the minimum.";
				break;
			}
			if (field.options.hasMaximum && number > field.options.maximum) {
				error = field.options.label + " exceeds the maximum.";
				break;
			}
		}
	}
	fPrimaryButton->SetEnabled(error.empty());
	if (fValidationMessage != nullptr) {
		fValidationMessage->SetText(error.c_str());
		fValidationMessage->Invalidate();
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//
