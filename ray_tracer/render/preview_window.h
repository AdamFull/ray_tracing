#pragma once

#include <cstdint>
#include <string>

// A few virtual-key constants so callers never need to include <windows.h>.
// Letters and digits use their ASCII uppercase code directly (e.g. 'W', 'P').
namespace preview_key
{
	constexpr int escape = 0x1B;
	constexpr int space = 0x20;
	constexpr int shift = 0x10;
	constexpr int control = 0x11;
}

// Minimal Win32 + GDI window used by the interactive preview. Every platform header is kept
// inside the .cpp so the rest of the engine never sees <windows.h> (and its min/max macros).
class CPreviewWindow
{
public:
	CPreviewWindow() = default;
	~CPreviewWindow();

	CPreviewWindow(const CPreviewWindow&) = delete;
	CPreviewWindow& operator=(const CPreviewWindow&) = delete;

	bool create(int width, int height, const std::string& title);
	void destroy();

	// Pump the OS message queue. Returns false once the window has been closed.
	bool poll();

	// Blit a top-down BGRX (0x00RRGGBB) pixel buffer, stretched to the client area.
	void present(const uint32_t* pixels, int width, int height);

	void set_title(const std::string& title);

	// --- input (state is refreshed by poll()) --------------------------------------------
	bool key_down(int vk) const;   // is a virtual key currently held
	bool key_pressed(int vk);      // pressed since the last query (rising edge, consumes it)
	bool right_mouse_down() const;
	void consume_mouse_delta(float& dx, float& dy);
	float consume_scroll();

	// Opaque implementation state; defined only in the .cpp (holds the Win32 handles/input).
	struct FImpl;
private:
	FImpl* m_impl{ nullptr };
};
