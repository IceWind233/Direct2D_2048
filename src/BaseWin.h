#ifndef BASEWIN_H
#define BASEWIN_H

#include <Windows.h>

template <typename DERIVED_Ty>
class Base_Win {

public:

	Base_Win();

	virtual ~Base_Win() = default;

public:

	static LRESULT CALLBACK window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

	BOOL create(
		PCWSTR lp_window_name,
		DWORD dw_style,
		DWORD dw_exstyle = NULL,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int win_width = CW_USEDEFAULT,
		int win_height = CW_USEDEFAULT,
		HWND hwnd_parent = nullptr,
		HMENU menu = nullptr
	);

	HWND window() const { return m_hwnd_; }

protected:
	virtual PCWSTR class_name() const = 0;

	virtual  LRESULT handle_message(UINT u_msg, WPARAM w_param, LPARAM l_param) = 0;

protected:
	HWND m_hwnd_;
};


template <typename DERIVED_Ty>
Base_Win<DERIVED_Ty>::Base_Win() :
	m_hwnd_(nullptr) {
}

template <typename DERIVED_Ty>
LRESULT Base_Win<DERIVED_Ty>::window_proc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
	DERIVED_Ty* p_this = nullptr;

	if (u_msg == WM_NCCREATE) {
		CREATESTRUCT* pCreate = (CREATESTRUCT*)l_param;
		p_this = static_cast<DERIVED_Ty*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_this);

		p_this->m_hwnd_ = hwnd;
	}
	else {
		p_this = (DERIVED_Ty*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	if (p_this) {
		return p_this->handle_message(u_msg, w_param, l_param);
	}
	else {
		return DefWindowProc(hwnd, u_msg, w_param, l_param);
	}
}

template <typename DERIVED_Ty>
BOOL Base_Win<DERIVED_Ty>::create(
	PCWSTR	lp_window_name,
	DWORD	dw_style,
	DWORD	dw_exstyle,
	int		x,
	int		y,
	int		win_width,
	int		win_height,
	HWND	hwnd_parent,
	HMENU 	menu) {

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = DERIVED_Ty::window_proc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = class_name();

	RegisterClass(&wc);

	m_hwnd_ = CreateWindowEx(
		dw_exstyle,
		class_name(),
		lp_window_name,
		dw_style,
		x,
		y,
		win_width,
		win_height,
		hwnd_parent,
		menu,
		GetModuleHandle(nullptr),
		this
	);

	return m_hwnd_ ? TRUE : FALSE;
}


#endif
