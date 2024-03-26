#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _MSC_VER
#include <windows.h>
#endif
#include "lvgl/lvgl.h"

#include <stdio.h>
#include <png.h>
#include <exception>

//#include "raw.h"
unsigned char *head_bmp = {};

#define TEST_PIXEL 1

//---------------------------------------------
#ifdef _MSC_VER

static HDC s_hBackBufferDC;
static unsigned char *s_pixel;
static int s_width;
static int s_height;

static void draw(HWND hAppWnd, int width, int height)
{
	int x, y;
	unsigned char *pixel = s_pixel;
	//draw
	Rectangle(s_hBackBufferDC, 0, 0, width, height);
	for (y = 0; y < s_height; y++)
	{
		for (x = 0; x < s_width; x++)
		{
			unsigned char b = *(pixel++);
			unsigned char g = *(pixel++);
			unsigned char r = *(pixel++);
			SetPixel(s_hBackBufferDC, x, s_height - y, RGB(r, g, b));
		}
	}
	InvalidateRect(hAppWnd, 0, FALSE);
}

static LRESULT CALLBACK WndProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	static HBITMAP s_hBitmap;
	static HBITMAP s_hOldBitmap;
	static HPEN s_hPen;
	static HPEN s_hOldPen;
	static HBRUSH s_hBrush;
	static HBRUSH s_hOldBrush;
	static HDC s_hCanvas;

	switch (message)
	{
	case WM_COMMAND:
		break;

	case WM_CREATE:
	{
					  RECT rect;
					  HDC hDC;
					  GetClientRect(hWnd, &rect);
					  hDC = GetDC(hWnd);
					  s_hBitmap = CreateCompatibleBitmap(hDC,
						  rect.right - rect.left,
						  rect.bottom - rect.left);
					  s_hPen = CreatePen(PS_SOLID, 0, 0);
					  s_hBackBufferDC = CreateCompatibleDC(hDC);
					  s_hBrush = (HBRUSH)CreateSolidBrush(0);
					  /*
					  * NOTE: bitmap object must be selected
					  */
					  s_hOldBitmap = (HBITMAP)SelectObject(s_hBackBufferDC, s_hBitmap);
					  s_hOldPen = (HPEN)SelectObject(s_hBackBufferDC, s_hPen);
					  s_hOldBrush = (HBRUSH)SelectObject(s_hBackBufferDC, s_hBrush);
					  ReleaseDC(hWnd, hDC);
					  s_hCanvas = s_hBackBufferDC;
	}
		break;

	case WM_DESTROY:
	{
					   SelectObject(s_hBackBufferDC, s_hOldPen);
					   SelectObject(s_hBackBufferDC, s_hOldBitmap);
					   SelectObject(s_hBackBufferDC, s_hOldBrush);
					   DeleteObject(s_hBackBufferDC);
					   DeleteObject(s_hBitmap);
					   DeleteObject(s_hPen);
					   DeleteObject(s_hBrush);
	}
		PostQuitMessage(0);
		break;

	case WM_PAINT:
	{
					 PAINTSTRUCT ps;
					 HDC hdc = BeginPaint(hWnd, &ps);
					 BitBlt(hdc,
						 ps.rcPaint.left, ps.rcPaint.top,
						 ps.rcPaint.right - ps.rcPaint.left,
						 ps.rcPaint.bottom - ps.rcPaint.top,
						 s_hCanvas,
						 ps.rcPaint.left, ps.rcPaint.top,
						 SRCCOPY);
					 EndPaint(hWnd, &ps);
	}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void testwin(unsigned char *pixel, int w/*pixel*/, int h/*pixel*/,
	int width/*window*/, int height/*window*/)
{
	const DWORD WINDOW_STYLE = WS_OVERLAPPED | WS_CAPTION |
		WS_MINIMIZEBOX | WS_SYSMENU;
	const char *WINDOW_CLASS = "pixelwin";
	const char *WINDOW_TITLE = "pixelwin";
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hAppWnd = NULL;
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0, 0,
		hInstance,
		NULL, NULL,
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		WINDOW_CLASS,
		NULL
	};
	RECT R = { 0, 0, width, height };
	s_pixel = pixel;
	s_width = w;
	s_height = h;
	if (!RegisterClassEx(&wcex))
	{
		return;
	}
	AdjustWindowRect(&R, WINDOW_STYLE, FALSE);
	if (!(hAppWnd = CreateWindow(WINDOW_CLASS,
		WINDOW_TITLE,
		WINDOW_STYLE,
		CW_USEDEFAULT,
		0, R.right - R.left, R.bottom - R.top,
		GetForegroundWindow(), NULL, hInstance, NULL)))
	{
		return;
	}
	ShowWindow(hAppWnd, SW_SHOW);
	UpdateWindow(hAppWnd);
	while (1)
	{
		MSG msg;
		if (1)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				draw(hAppWnd, width, height);
				continue;
			}
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			if (!GetMessage(&msg, 0, 0, 0))
			{
				break;
			}
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			draw(hAppWnd, width, height);
		}
	}
	return;
}

#endif

//---------------------------------------------

unsigned char *PixelCreate(int width, int height)
{
	return (unsigned char *)malloc(width * height * 3);
}

void PixelDelete(unsigned char *pixel)
{
	free(pixel);
}

void PixelSetPixel(unsigned char *pixel, int width, int height,
	int x, int y, unsigned int color)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
	{
		return;
	}
	else
	{
#if 0
		unsigned char b = color;
		unsigned char g = color >> 8;
		unsigned char r = color >> 16;
		pixel += (height - y - 1) * width * 3 + x * 3;
		*(pixel++) = r;
		*(pixel++) = b;
		*(pixel++) = g;
#else
		unsigned char b = color >> 16;
		unsigned char g = color >> 8;
		unsigned char r = color >> 0;
		pixel += y * width * 3 + x * 3;
		*(pixel++) = b;
		*(pixel++) = g;
		*(pixel++) = r;
#endif
	}
}

void PixelClearColor(unsigned char *pixel, int width, int height,
	unsigned int color)
{
	int x, y;
	unsigned char b = color;
	unsigned char g = color >> 8;
	unsigned char r = color >> 16;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			*(pixel++) = r;
			*(pixel++) = b;
			*(pixel++) = g;
		}
	}
}

void PixelBlt(unsigned char *dest, int width, int height,
	unsigned char *src, int sw, int sh,
	int clipx, int clipy, int clipw, int cliph)
{
	int x, y;
	for (y = 0; y < sh; y++)
	{
		unsigned char *psrc = src + y * sw * 3;
		unsigned char *pdest = dest +
			(height - ((sh - y) + clipy)) * width * 3 +
			(clipx + 0) * 3;
		for (x = 0; x < sw; x++)
		{
			if (x < clipx || x >= (clipx + clipw) ||
				(sh - y) < clipy || (sh - y) >= (clipy + cliph))
			{
				continue;
			}
			*(pdest++) = *(psrc++);
			*(pdest++) = *(psrc++);
			*(pdest++) = *(psrc++);
		}
	}
}

void PixelFillMask(unsigned char *pixel, int width, int height)
{
	int x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			unsigned char gray;
			gray = ((8 - y) % 8) * 32;
			gray &= 0xFF;
			*(pixel++) = gray;
			*(pixel++) = gray;
			*(pixel++) = gray;
		}
	}
}

int PixelDumpFile(unsigned char *pixel, int width, int height,
	const char *filename)
{
	FILE *fp = NULL;
	//43200
	//43200 + 54 + 2 = 43256
#if 0
	//wrong method
	unsigned int imagesize = width * height * 3;
	unsigned int filesize = imagesize + 56;
#else
	unsigned int zeroByteSize = 0;
	zeroByteSize = 4 - ((width * 3) % 4);
	if (zeroByteSize == 4)
	{
		zeroByteSize = 0;
	}
	unsigned int imagesize = (width * 3 + zeroByteSize) * height;
	unsigned int filesize = imagesize + 56;
#endif
	unsigned char header[54] = { 0 };
	header[0] = 0x42; // 'B'
	header[1] = 0x4D; // 'M'
	header[2] = filesize; // bytes num
	header[3] = filesize >> 8; //
	header[4] = filesize >> 16; //
	header[5] = filesize >> 24; //
	//printf("0x%0x, 0x%0x, 0x%0x, 0x%0x\n", header[2], header[3], header[4], header[5]);
	header[8 + 2] = 0x36; // data offset = 54
	header[8 + 6] = 0x28;
	header[16 + 2] = width;
	header[16 + 3] = width >> 8;
	header[16 + 4] = width >> 16;
	header[16 + 5] = width >> 24;
	header[16 + 6] = height;
	header[16 + 7] = height >> 8;
	header[24 + 0] = height >> 16;
	header[24 + 1] = height >> 24;
	header[24 + 2] = 0x01; // biPlane = 1
	header[24 + 4] = 0x18; // biBitCount = 24
	header[32 + 2] = imagesize; // biSizeImage
	header[32 + 3] = imagesize >> 8;
	header[32 + 4] = imagesize >> 16;
	header[32 + 5] = imagesize >> 24;
	header[32 + 6] = 0x12; // biXPelsMeter = B12H
	header[32 + 7] = 0x0B;
	header[40 + 2] = 0x12; // biYPelsMeter = B12H
	header[40 + 3] = 0x0B;
	if (NULL == (fp = fopen(filename, "wb+")))
	{
		fprintf(stderr, "open file error\n");
		return 1;
	}
	fwrite(header, sizeof(header), 1, fp);

#if 0
	//wrong method;
	fwrite(pixel, width * height * 3, 1, fp);
#else
	unsigned char pZero[] = { 0x0, 0x0, 0x0, 0x0 };
	int j;
	unsigned char * p = pixel;
	for (j = 0; j < height; j++)
	{
		unsigned int linesize = width * 3;
		fwrite(p, 1, linesize, fp);
		p += linesize;
		if (zeroByteSize > 0)
		{
			fwrite(pZero, 1, zeroByteSize, fp);
		}
	}
#endif

	fclose(fp);
	return 0;
}

#if TEST_PIXEL

void test1(void)
{
	int w = 800;
	int h = 480;
	int i;
	unsigned char *pixel = PixelCreate(w, h);
	//PixelFillMask(pixel, w, h);
	PixelClearColor(pixel, w, h, 0xCCCCCC);
	for (i = 0; i < 40000; i++)
	{
		PixelSetPixel(pixel, w, h, i, i, 0xff0000);
	}
	PixelDumpFile(pixel, w, h, "output.bmp");
	testwin(pixel, w, h, w, h);
	PixelDelete(pixel);
}

void test2(void)
{
	int w = 120;
	int h = 120;
	unsigned char *pixel = head_bmp;
	PixelDumpFile(pixel, w, h, "output.bmp");
	testwin(pixel, w, h, w, h);
}

void test3(void)
{
	int w = 180;
	int h = 240;
	unsigned char *pixel = PixelCreate(w, h);
	PixelClearColor(pixel, w, h, 0xCCCCCC);
	PixelBlt(pixel, w, h, head_bmp, 120, 120,
		10, 30, 120, 60);
	PixelDumpFile(pixel, w, h, "output.bmp");
	testwin(pixel, w, h, w, h);
	PixelDelete(pixel);
}

BOOL DIBSavePng(png_byte *DIBGetBits, const char *path, png_uint_32 DIBWidth, png_uint_32 DIBHeight)
{
	png_bytepp row = NULL;
	FILE *fp = fopen(path, "wb");
	if (!fp)
	{
		return FALSE;
	}
	try
	{
		png_structp png_ptr;
		png_infop info_ptr;
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
		if (png_ptr == 0)
		{
			throw std::exception("not enough memory");
		}
		else
		{
			info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == 0)
			{
				if (png_ptr)
				{
					png_destroy_write_struct(&png_ptr, info_ptr ? &info_ptr : 0);
				}
				png_ptr = 0;
				info_ptr = 0;
				throw std::exception("not enough memory");
			}
		}
		png_init_io(png_ptr, fp);
		if (false)
		{
			png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
			png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);
		}
		png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_FILTER_SUB);
		png_set_IHDR(png_ptr, info_ptr,
			DIBWidth, DIBHeight, 8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		png_write_info(png_ptr, info_ptr);
		row = new png_byte*[DIBHeight];
		for (png_uint_32 y = 0; y < DIBHeight; y++)
		{
			row[y] = (png_byte *)(DIBGetBits + (DIBHeight - y - 1) * 3 * DIBWidth);
		}
		png_set_bgr(png_ptr);
		png_write_image(png_ptr, row);
		png_write_end(png_ptr, info_ptr);
	}
	catch (...)
	{
		if (row)
		{
			delete[] row;
			row = NULL;
		}
		fclose(fp);
		return FALSE;
	}
	if (row)
	{
		delete[] row;
		row = NULL;
	}
	fclose(fp);
	return TRUE;
}

BOOL DIBSavePngRGBA(png_byte *DIBGetBits, const char *path, png_uint_32 DIBWidth, png_uint_32 DIBHeight)
{
	png_bytepp row = NULL;
	FILE *fp = fopen(path, "wb");
	if (!fp)
	{
		return FALSE;
	}
	try
	{
		png_structp png_ptr;
		png_infop info_ptr;
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
		if (png_ptr == 0)
		{
			throw std::exception("not enough memory");
		}
		else
		{
			info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == 0)
			{
				if (png_ptr)
				{
					png_destroy_write_struct(&png_ptr, info_ptr ? &info_ptr : 0);
				}
				png_ptr = 0;
				info_ptr = 0;
				throw std::exception("not enough memory");
			}
		}
		png_init_io(png_ptr, fp);
		if (false)
		{
			png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
			png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_ALL_FILTERS);
		}
		png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_FILTER_SUB);
		png_set_IHDR(png_ptr, info_ptr,
			DIBWidth, DIBHeight, 8, PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		png_write_info(png_ptr, info_ptr);
		row = new png_byte*[DIBHeight];
		for (png_uint_32 y = 0; y < DIBHeight; y++)
		{	
			row[y] = (png_byte *)(DIBGetBits + (DIBHeight - y - 1) * 4 * DIBWidth);
		}
		png_set_bgr(png_ptr);
		png_write_image(png_ptr, row);
		png_write_end(png_ptr, info_ptr);
	}
	catch (...)
	{
		if (row)
		{
			delete[] row;
			row = NULL;
		}
		fclose(fp);
		return FALSE;
	}
	if (row)
	{
		delete[] row;
		row = NULL;
	}
	fclose(fp);
	return TRUE;
}

void test4(const lv_img_dsc_t *img_lv_demo_music_logo, const char *prefix)
{
	int USE_BMP = 0;

	int i, j;
	//LV_IMG_DECLARE(img_lv_demo_music_logo);
	int w = img_lv_demo_music_logo->header.w;
	int h = img_lv_demo_music_logo->header.h;
	unsigned char *pixel = PixelCreate(w, h);
	PixelClearColor(pixel, w, h, 0x000000);
	unsigned char *pixel_rgba = 0;
	if (img_lv_demo_music_logo->header.cf == 5) {
		//LV_IMG_CF_TRUE_COLOR_ALPHA, the 3rd byte is alpha
		pixel_rgba = (unsigned char *)malloc(w * h * 4);
		int x, y;
		unsigned char* pixel_rgba_p = pixel_rgba;
		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				*(pixel_rgba_p++) = 0;
				*(pixel_rgba_p++) = 0;
				*(pixel_rgba_p++) = 0;
				*(pixel_rgba_p++) = 0;
			}
		}
	}

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			unsigned short color = 0;

			if (img_lv_demo_music_logo->header.cf == 5)
			{
				//LV_IMG_CF_TRUE_COLOR_ALPHA, the 3rd byte is alpha
				color =
					(img_lv_demo_music_logo->data[w * 3 * (h - j - 1) + i * 3 + 0] << 0) |
					(img_lv_demo_music_logo->data[w * 3 * (h - j - 1) + i * 3 + 1] << 8);
			}
			else
			{
				color =
					(img_lv_demo_music_logo->data[w * 2 * (h - j - 1) + i * 2 + 0] << 0) |
					(img_lv_demo_music_logo->data[w * 2 * (h - j - 1) + i * 2 + 1] << 8);
			}
			//if (color != 0 && color != 0xffff) {
			//	printf("hit\n");
			//}
			unsigned char R = (color & 0xF800) >> 11;
			unsigned char G = (color & 0x07E0) >> 5;
			unsigned char B = (color & 0x001F);
			R <<= 3;
			G <<= 2;
			B <<= 3;
			unsigned int color2 = (B << 16) | (G << 8) | (R << 0);
			//PixelSetPixel(pixel, w, h, i, j, 0xff0000); //0xff0000=blue
			PixelSetPixel(pixel, w, h, i, j, color2);

			if (img_lv_demo_music_logo->header.cf == 5)
			{
#if 1
				unsigned char A = img_lv_demo_music_logo->data[w * 3 * (h - j - 1) + i * 3 + 2];
				unsigned char* pixel_rgba_p = pixel_rgba;
				pixel_rgba_p += j * w * 4 + i * 4;
				*(pixel_rgba_p++) = B;
				*(pixel_rgba_p++) = G;
				*(pixel_rgba_p++) = R;
				*(pixel_rgba_p++) = A;
#endif
			}
		}
	}
	char filename[256] = { 0 };
	if (USE_BMP) 
	{
		_snprintf(filename, sizeof(filename) - 1, "%s.bmp", prefix);
		PixelDumpFile(pixel, w, h, filename); //"img_lv_demo_music_logo.bmp"
	}
	else 
	{
		_snprintf(filename, sizeof(filename) - 1, "%s.png", prefix);
		if (img_lv_demo_music_logo->header.cf == 5)
		{
			DIBSavePngRGBA(pixel_rgba, filename, w, h);
		}
		else
		{
			DIBSavePng(pixel, filename, w, h);
		}
	}
	//testwin(pixel, w, h, w, h);
	PixelDelete(pixel);
	if (pixel_rgba != 0)
	{
		free(pixel_rgba);
		pixel_rgba = 0;
	}
}

void test5(void)
{
	//extern void test4(const lv_img_dsc_t *img_lv_demo_music_logo, const char *filename);
	LV_IMG_DECLARE(img_lv_demo_music_logo);
	LV_IMG_DECLARE(img_lv_demo_music_wave_top);
	LV_IMG_DECLARE(img_lv_demo_music_wave_bottom);
	LV_IMG_DECLARE(img_lv_demo_music_corner_left);
	LV_IMG_DECLARE(img_lv_demo_music_corner_right);
	LV_IMG_DECLARE(img_lv_demo_music_icon_1);
	LV_IMG_DECLARE(img_lv_demo_music_icon_2);
	LV_IMG_DECLARE(img_lv_demo_music_icon_3);
	LV_IMG_DECLARE(img_lv_demo_music_icon_4);
	LV_IMG_DECLARE(img_lv_demo_music_btn_loop);
	LV_IMG_DECLARE(img_lv_demo_music_btn_rnd);
	LV_IMG_DECLARE(img_lv_demo_music_btn_next);
	LV_IMG_DECLARE(img_lv_demo_music_btn_prev);
	LV_IMG_DECLARE(img_lv_demo_music_btn_play);
	LV_IMG_DECLARE(img_lv_demo_music_btn_pause);
	LV_IMG_DECLARE(img_lv_demo_music_slider_knob);
	LV_IMG_DECLARE(img_lv_demo_music_cover_1);
	LV_IMG_DECLARE(img_lv_demo_music_cover_2);
	LV_IMG_DECLARE(img_lv_demo_music_cover_3);
	LV_IMG_DECLARE(img_lv_demo_music_btn_list_play);
	LV_IMG_DECLARE(img_lv_demo_music_btn_list_pause);
	LV_IMG_DECLARE(img_lv_demo_music_list_border);

	test4(&img_lv_demo_music_logo, "img_lv_demo_music_logo");
	test4(&img_lv_demo_music_wave_top, "img_lv_demo_music_wave_top");
	test4(&img_lv_demo_music_wave_bottom, "img_lv_demo_music_wave_bottom");
	test4(&img_lv_demo_music_corner_left, "img_lv_demo_music_corner_left");
	test4(&img_lv_demo_music_corner_right, "img_lv_demo_music_corner_right");
	test4(&img_lv_demo_music_icon_1, "img_lv_demo_music_icon_1");
	test4(&img_lv_demo_music_icon_2, "img_lv_demo_music_icon_2");
	test4(&img_lv_demo_music_icon_3, "img_lv_demo_music_icon_3");
	test4(&img_lv_demo_music_icon_4, "img_lv_demo_music_icon_4");
	test4(&img_lv_demo_music_btn_loop, "img_lv_demo_music_btn_loop");
	test4(&img_lv_demo_music_btn_rnd, "img_lv_demo_music_btn_rnd");
	test4(&img_lv_demo_music_btn_next, "img_lv_demo_music_btn_next");
	test4(&img_lv_demo_music_btn_prev, "img_lv_demo_music_btn_prev");
	test4(&img_lv_demo_music_btn_play, "img_lv_demo_music_btn_play");
	test4(&img_lv_demo_music_btn_pause, "img_lv_demo_music_btn_pause");
	test4(&img_lv_demo_music_slider_knob, "img_lv_demo_music_slider_knob");
	test4(&img_lv_demo_music_cover_1, "img_lv_demo_music_cover_1");
	test4(&img_lv_demo_music_cover_2, "img_lv_demo_music_cover_2");
	test4(&img_lv_demo_music_cover_3, "img_lv_demo_music_cover_3");
	test4(&img_lv_demo_music_btn_list_play, "img_lv_demo_music_btn_list_play");
	test4(&img_lv_demo_music_btn_list_pause, "img_lv_demo_music_btn_list_pause");
	test4(&img_lv_demo_music_list_border, "img_lv_demo_music_list_border");
}

int main___(int argc, char **argv)
{
	test3();
	return 0;
}

#endif /* TEST_PIXEL */

