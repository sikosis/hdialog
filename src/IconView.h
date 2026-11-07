#ifndef HDIALOG_ICON_VIEW_H
#define HDIALOG_ICON_VIEW_H

#include <View.h>

class BBitmap;

class IconView : public BView {
public:
	explicit IconView(BBitmap* bitmap);
	~IconView() override;
	void Draw(BRect updateRect) override;

private:
	BBitmap* fBitmap;
};

#endif
