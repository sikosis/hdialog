#include "IconView.h"

#include <Bitmap.h>
#include <Font.h>

#include <algorithm>
#include <cstring>

BuiltInIconKind
BuiltInIconForName(const char* name)
{
	if (strcmp(name, "info") == 0)
		return BuiltInIconKind::Info;
	if (strcmp(name, "warning") == 0)
		return BuiltInIconKind::Warning;
	if (strcmp(name, "error") == 0)
		return BuiltInIconKind::Error;
	if (strcmp(name, "question") == 0)
		return BuiltInIconKind::Question;
	if (strcmp(name, "success") == 0)
		return BuiltInIconKind::Success;
	return BuiltInIconKind::None;
}
//---------------------------------------------------------------------------------------------------------------------------------//


IconView::IconView(BBitmap* bitmap, float size)
	:
	BView("icon", B_WILL_DRAW),
	fBitmap(bitmap),
	fKind(BuiltInIconKind::None)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetExplicitPreferredSize(BSize(size, size));
	SetExplicitMaxSize(BSize(size, size));
}
//---------------------------------------------------------------------------------------------------------------------------------//


IconView::IconView(BuiltInIconKind kind, float size)
	:
	BView("icon", B_WILL_DRAW),
	fBitmap(nullptr),
	fKind(kind)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetExplicitPreferredSize(BSize(size, size));
	SetExplicitMaxSize(BSize(size, size));
}
//---------------------------------------------------------------------------------------------------------------------------------//


IconView::~IconView()
{
	delete fBitmap;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
IconView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);
	if (fBitmap != nullptr) {
		const BRect source = fBitmap->Bounds();
		const float scale = std::min(Bounds().Width() / source.Width(),
			Bounds().Height() / source.Height());
		const float width = source.Width() * scale;
		const float height = source.Height() * scale;
		BRect destination(0, 0, width, height);
		destination.OffsetTo((Bounds().Width() - width) / 2,
			(Bounds().Height() - height) / 2);
		DrawBitmap(fBitmap, source, destination, B_FILTER_BITMAP_BILINEAR);
	} else
		DrawBuiltInIcon();
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
IconView::DrawBuiltInIcon()
{
	const float width = Bounds().Width();
	const float height = Bounds().Height();
	const float penSize = std::max(3.0f, width / 14.0f);
	SetPenSize(penSize);
	SetLineMode(B_ROUND_CAP, B_ROUND_JOIN);

	if (fKind == BuiltInIconKind::Warning) {
		SetHighColor(255, 190, 35);
		FillTriangle(BPoint(width / 2, 4), BPoint(width - 4, height - 5),
			BPoint(4, height - 5));
		SetHighColor(70, 55, 20);
		StrokeLine(BPoint(width / 2, height * 0.32f),
			BPoint(width / 2, height * 0.64f));
		FillEllipse(BPoint(width / 2, height * 0.79f), penSize / 2,
			penSize / 2);
		return;
	}

	if (fKind == BuiltInIconKind::Info)
		DrawCircleIcon({ 45, 125, 225, 255 });
	else if (fKind == BuiltInIconKind::Error)
		DrawCircleIcon({ 210, 55, 55, 255 });
	else if (fKind == BuiltInIconKind::Question)
		DrawCircleIcon({ 120, 85, 190, 255 });
	else if (fKind == BuiltInIconKind::Success)
		DrawCircleIcon({ 45, 160, 90, 255 });
	else
		return;

	SetHighColor(255, 255, 255);
	if (fKind == BuiltInIconKind::Info) {
		StrokeLine(BPoint(width / 2, height * 0.42f),
			BPoint(width / 2, height * 0.75f));
		FillEllipse(BPoint(width / 2, height * 0.25f), penSize / 2,
			penSize / 2);
	} else if (fKind == BuiltInIconKind::Error) {
		StrokeLine(BPoint(width * 0.31f, height * 0.31f),
			BPoint(width * 0.69f, height * 0.69f));
		StrokeLine(BPoint(width * 0.69f, height * 0.31f),
			BPoint(width * 0.31f, height * 0.69f));
	} else if (fKind == BuiltInIconKind::Success) {
		StrokeLine(BPoint(width * 0.25f, height * 0.52f),
			BPoint(width * 0.43f, height * 0.69f));
		StrokeLine(BPoint(width * 0.43f, height * 0.69f),
			BPoint(width * 0.76f, height * 0.32f));
	} else if (fKind == BuiltInIconKind::Question) {
		BFont font;
		GetFont(&font);
		font.SetSize(height * 0.62f);
		font.SetFace(B_BOLD_FACE);
		SetFont(&font);
		const float questionWidth = StringWidth("?");
		DrawString("?", BPoint((width - questionWidth) / 2, height * 0.72f));
	}
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
IconView::DrawCircleIcon(rgb_color colour)
{
	SetHighColor(colour);
	BRect circle = Bounds();
	circle.InsetBy(4, 4);
	FillEllipse(circle);
}
//---------------------------------------------------------------------------------------------------------------------------------//
