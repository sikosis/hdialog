#ifndef HDIALOG_ICON_VIEW_H
#define HDIALOG_ICON_VIEW_H

#include <View.h>

class BBitmap;

enum class BuiltInIconKind {
	None,
	Info,
	Warning,
	Error,
	Question,
	Success
};

BuiltInIconKind BuiltInIconForName(const char* name);

class IconView : public BView {
public:
	IconView(BBitmap* bitmap, float size);
	IconView(BuiltInIconKind kind, float size);
	~IconView() override;
	void Draw(BRect updateRect) override;

private:
	void DrawBuiltInIcon();
	void DrawCircleIcon(rgb_color colour);

	BBitmap* fBitmap;
	BuiltInIconKind fKind;
};

#endif
