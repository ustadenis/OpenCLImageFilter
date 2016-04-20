#include "stdafx.h"
#include "ImageWnd.h"

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(CImageWnd, CStatic)

CImageWnd::CImageWnd() : _image(nullptr)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CImageWnd::~CImageWnd()
{
	delete _image;
	GdiplusShutdown(gdiplusToken);
}

BEGIN_MESSAGE_MAP(CImageWnd, CStatic)
END_MESSAGE_MAP()

void CImageWnd::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// объекты GDI+
	Graphics gr(lpDrawItemStruct->hDC);
	Rect rect(lpDrawItemStruct->rcItem.left,
		lpDrawItemStruct->rcItem.top,
		lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
		lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top);

	gr.Clear(Color::White);

	if (_image != nullptr)
	{
		// учет соотношения сторон изображения
#if 1
		if (_image->GetWidth() > _image->GetHeight())
		{
			double scale = (double)rect.Width / _image->GetWidth();
			rect.Inflate(0, -(rect.Height - (int)(scale * _image->GetHeight())) / 2);
		}
		else
		{
			double scale = (double)rect.Height / _image->GetHeight();
			rect.Inflate(-(rect.Width - (int)(scale * _image->GetWidth())) / 2, 0);
		}
#endif

		gr.DrawImage(_image, rect);
	}
}

void CImageWnd::SetImage(Bitmap *bmp)
{
	if (_image != nullptr &&
		_image != bmp) delete _image;

	_image = bmp;
	Invalidate(TRUE);
}

Bitmap *CImageWnd::GetImage() const
{
	return _image;
}

void CImageWnd::Clear()
{
	CClientDC dc(this);
	Graphics gr(dc);
	gr.Clear(Color::White);
}
