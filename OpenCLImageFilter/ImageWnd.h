#pragma once

#include <afxwin.h>
#include <gdiplus.h>

class CImageWnd : public CStatic
{
	Gdiplus::Bitmap *_image;

	DECLARE_DYNAMIC(CImageWnd)
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	ULONG_PTR gdiplusToken;

public:

	CImageWnd();
	virtual ~CImageWnd();

	void SetImage(Gdiplus::Bitmap *bmp);
	Gdiplus::Bitmap *GetImage() const;

	void Clear();
};
