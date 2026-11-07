#include "DialogApplication.h"

#include <Message.h>
#include <Messenger.h>
#include <PropertyInfo.h>

#include <cstring>

#include "DialogWindow.h"
#include "Messages.h"

namespace {

property_info kProperties[] = {
	{ "Title", { B_GET_PROPERTY, B_SET_PROPERTY },
		{ B_DIRECT_SPECIFIER }, "The dialog window title", 0,
		{ B_STRING_TYPE }, {}, {} },
	{ "Message", { B_GET_PROPERTY, B_SET_PROPERTY },
		{ B_DIRECT_SPECIFIER }, "The dialog message", 0,
		{ B_STRING_TYPE }, {}, {} },
	{ "Progress", { B_GET_PROPERTY, B_SET_PROPERTY },
		{ B_DIRECT_SPECIFIER }, "Progress from 0 to 100", 0,
		{ B_FLOAT_TYPE, B_INT32_TYPE }, {}, {} },
	{ "Input", { B_GET_PROPERTY }, { B_DIRECT_SPECIFIER },
		"The current text-field value", 0, { B_STRING_TYPE }, {}, {} },
	{ nullptr, {}, {}, nullptr, 0, {}, {}, {} }
};

BPropertyInfo kPropertyInfo(kProperties);

} // namespace

DialogApplication::DialogApplication(const DialogOptions& options)
	:
	BApplication("application/x-vnd.hdialog"),
	fOptions(options),
	fWindow(nullptr),
	fInput(options.inputValue),
	fSelection(options.selectedItem),
	fCheckboxChecked(options.checkboxChecked),
	fResult(kResultSecondary)
{
	for (const NamedFieldOptions& field : options.fields)
		fFields.push_back({ field.name, field.value });
}
//---------------------------------------------------------------------------------------------------------------------------------//


status_t
DialogApplication::GetSupportedSuites(BMessage* data)
{
	status_t status = data->AddString("suites", "suite/vnd.hdialog");
	if (status == B_OK)
		status = data->AddFlat("messages", &kPropertyInfo);
	if (status != B_OK)
		return status;
	return BApplication::GetSupportedSuites(data);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogApplication::MessageReceived(BMessage* message)
{
	if (message->what == B_GET_PROPERTY || message->what == B_SET_PROPERTY) {
		HandleScriptingMessage(message);
		return;
	}
	if (message->what == kMessageResult) {
		int32 result;
		if (message->FindInt32("result", &result) == B_OK)
			fResult = result;
		const char* input;
		if (message->FindString("input", &input) == B_OK)
			fInput = input;
		const char* selection;
		if (message->FindString("selection", &selection) == B_OK)
			fSelection = selection;
		message->FindBool("checked", &fCheckboxChecked);
		fFields.clear();
		for (int32 index = 0;; index++) {
			const char* name;
			const char* value;
			if (message->FindString("field_name", index, &name) != B_OK
				|| message->FindString("field_value", index, &value) != B_OK) {
				break;
			}
			fFields.push_back({ name, value });
		}
		PostMessage(B_QUIT_REQUESTED);
		return;
	}
	BApplication::MessageReceived(message);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogApplication::ReadyToRun()
{
	fWindow = new DialogWindow(fOptions);
	fWindow->Show();
}
//---------------------------------------------------------------------------------------------------------------------------------//


BHandler*
DialogApplication::ResolveSpecifier(BMessage* message, int32 index,
	BMessage* specifier, int32 form, const char* property)
{
	if (kPropertyInfo.FindMatch(message, index, specifier, form, property) >= 0)
		return this;
	return BApplication::ResolveSpecifier(message, index, specifier, form,
		property);
}
//---------------------------------------------------------------------------------------------------------------------------------//


int
DialogApplication::Result() const
{
	return fResult;
}
//---------------------------------------------------------------------------------------------------------------------------------//


const std::string&
DialogApplication::Input() const
{
	return fInput;
}
//---------------------------------------------------------------------------------------------------------------------------------//


bool
DialogApplication::CheckboxChecked() const
{
	return fCheckboxChecked;
}
//---------------------------------------------------------------------------------------------------------------------------------//


const std::string&
DialogApplication::Selection() const
{
	return fSelection;
}
//---------------------------------------------------------------------------------------------------------------------------------//


const std::vector<std::pair<std::string, std::string>>&
DialogApplication::Fields() const
{
	return fFields;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogApplication::HandleScriptingMessage(BMessage* message)
{
	int32 index;
	BMessage specifier;
	int32 form;
	const char* property;
	BMessage reply(B_REPLY);
	status_t status = message->GetCurrentSpecifier(&index, &specifier, &form,
		&property);
	if (status != B_OK) {
		reply.AddInt32("error", status);
		message->SendReply(&reply);
		return;
	}

	if (message->what == B_GET_PROPERTY) {
		if (strcmp(property, "Title") == 0)
			reply.AddString("result", fOptions.title.c_str());
		else if (strcmp(property, "Message") == 0)
			reply.AddString("result", fOptions.message.c_str());
		else if (strcmp(property, "Progress") == 0)
			reply.AddFloat("result", fOptions.progress);
		else if (strcmp(property, "Input") == 0 && fWindow != nullptr) {
			BMessage request(kMessageGetInput);
			BMessage inputReply;
			status = BMessenger(fWindow).SendMessage(&request, &inputReply);
			const char* input;
			if (status == B_OK
				&& inputReply.FindString("result", &input) == B_OK) {
				reply.AddString("result", input);
			} else
				status = B_ERROR;
		} else
			status = B_BAD_SCRIPT_SYNTAX;
	} else {
		if (strcmp(property, "Title") == 0) {
			const char* value;
			status = message->FindString("data", &value);
			if (status == B_OK) {
				fOptions.title = value;
				SendWindowString(kMessageSetTitle, value);
			}
		} else if (strcmp(property, "Message") == 0) {
			const char* value;
			status = message->FindString("data", &value);
			if (status == B_OK) {
				fOptions.message = value;
				SendWindowString(kMessageSetMessage, value);
			}
		} else if (strcmp(property, "Progress") == 0) {
			float value;
			status = message->FindFloat("data", &value);
			if (status != B_OK) {
				int32 integerValue = 0;
				status = message->FindInt32("data", &integerValue);
				value = integerValue;
			}
			if (status == B_OK && value >= 0 && value <= 100) {
				fOptions.progress = static_cast<int>(value);
				BMessage update(kMessageSetProgress);
				update.AddFloat("value", value);
				BMessenger(fWindow).SendMessage(&update);
			} else if (status == B_OK)
				status = B_BAD_VALUE;
		} else
			status = B_BAD_SCRIPT_SYNTAX;
	}

	reply.AddInt32("error", status);
	message->SendReply(&reply);
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
DialogApplication::SendWindowString(uint32 command, const char* value)
{
	if (fWindow == nullptr)
		return;
	BMessage update(command);
	update.AddString("value", value);
	BMessenger(fWindow).SendMessage(&update);
}
//---------------------------------------------------------------------------------------------------------------------------------//
