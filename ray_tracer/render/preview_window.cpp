#include "preview_window.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

struct CPreviewWindow::FImpl
{
	HWND hwnd{ nullptr };
	bool should_close{ false };

	bool keys[256]{};
	bool keys_pressed[256]{};

	bool rmb{ false };
	bool have_last{ false };
	int last_mx{ 0 }, last_my{ 0 };
	float mouse_dx{ 0.f }, mouse_dy{ 0.f };
	float scroll{ 0.f };
};

static const char* k_class_name = "RayTracerPreviewWindow";

static LRESULT CALLBACK preview_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	auto* impl = reinterpret_cast<CPreviewWindow::FImpl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		if (impl) impl->should_close = true;
		return 0;
	case WM_KILLFOCUS:
		// Avoid keys getting "stuck" if focus is lost while a key/button is held.
		if (impl)
		{
			for (bool& k : impl->keys) k = false;
			impl->rmb = false;
			impl->have_last = false;
		}
		return 0;
	case WM_KEYDOWN:
		if (impl && wparam < 256u)
		{
			if (!impl->keys[wparam]) impl->keys_pressed[wparam] = true;
			impl->keys[wparam] = true;
		}
		return 0;
	case WM_KEYUP:
		if (impl && wparam < 256u) impl->keys[wparam] = false;
		return 0;
	case WM_RBUTTONDOWN:
		if (impl) { impl->rmb = true; impl->have_last = false; SetCapture(hwnd); }
		return 0;
	case WM_RBUTTONUP:
		if (impl) { impl->rmb = false; ReleaseCapture(); }
		return 0;
	case WM_MOUSEMOVE:
		if (impl && impl->rmb)
		{
			int mx = static_cast<short>(LOWORD(lparam));
			int my = static_cast<short>(HIWORD(lparam));
			if (impl->have_last)
			{
				impl->mouse_dx += static_cast<float>(mx - impl->last_mx);
				impl->mouse_dy += static_cast<float>(my - impl->last_my);
			}
			impl->last_mx = mx;
			impl->last_my = my;
			impl->have_last = true;
		}
		return 0;
	case WM_MOUSEWHEEL:
		if (impl) impl->scroll += static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam)) / static_cast<float>(WHEEL_DELTA);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

CPreviewWindow::~CPreviewWindow()
{
	destroy();
}

bool CPreviewWindow::create(int width, int height, const std::string& title)
{
	m_impl = new FImpl();

	HINSTANCE inst = GetModuleHandle(nullptr);

	WNDCLASSA wc{};
	wc.lpfnWndProc = preview_wnd_proc;
	wc.hInstance = inst;
	wc.lpszClassName = k_class_name;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClassA(&wc); // harmless if already registered

	RECT rect{ 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_impl->hwnd = CreateWindowExA(0, k_class_name, title.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, inst, nullptr);

	if (!m_impl->hwnd)
	{
		delete m_impl;
		m_impl = nullptr;
		return false;
	}

	SetWindowLongPtr(m_impl->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(m_impl));
	ShowWindow(m_impl->hwnd, SW_SHOW);
	UpdateWindow(m_impl->hwnd);
	return true;
}

void CPreviewWindow::destroy()
{
	if (!m_impl)
		return;

	if (m_impl->hwnd)
		DestroyWindow(m_impl->hwnd);

	delete m_impl;
	m_impl = nullptr;
}

bool CPreviewWindow::poll()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return !m_impl->should_close;
}

void CPreviewWindow::present(const uint32_t* pixels, int width, int height)
{
	HDC hdc = GetDC(m_impl->hwnd);

	BITMAPINFO bmi{};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // negative => top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	RECT rc;
	GetClientRect(m_impl->hwnd, &rc);
	int cw = rc.right - rc.left;
	int ch = rc.bottom - rc.top;

	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchDIBits(hdc, 0, 0, cw, ch, 0, 0, width, height, pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(m_impl->hwnd, hdc);
}

void CPreviewWindow::set_title(const std::string& title)
{
	if (m_impl && m_impl->hwnd)
		SetWindowTextA(m_impl->hwnd, title.c_str());
}

bool CPreviewWindow::key_down(int vk) const
{
	return (vk >= 0 && vk < 256) ? m_impl->keys[vk] : false;
}

bool CPreviewWindow::key_pressed(int vk)
{
	if (vk < 0 || vk >= 256)
		return false;
	bool pressed = m_impl->keys_pressed[vk];
	m_impl->keys_pressed[vk] = false;
	return pressed;
}

bool CPreviewWindow::right_mouse_down() const
{
	return m_impl->rmb;
}

void CPreviewWindow::consume_mouse_delta(float& dx, float& dy)
{
	dx = m_impl->mouse_dx;
	dy = m_impl->mouse_dy;
	m_impl->mouse_dx = 0.f;
	m_impl->mouse_dy = 0.f;
}

float CPreviewWindow::consume_scroll()
{
	float s = m_impl->scroll;
	m_impl->scroll = 0.f;
	return s;
}
