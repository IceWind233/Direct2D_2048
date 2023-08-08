#include <Windows.h>

#include "MainWindow.h"

#include <iostream>

MainWindow::MainWindow() = default;

LRESULT MainWindow::handle_message(UINT u_msg, WPARAM w_param, LPARAM l_param) {
	switch (u_msg) {
	case WM_CREATE: {
		if (FAILED(board_.init_paint(m_hwnd_))) {
			return -1;
		}
		board_.generate_rand();
		board_.generate_rand();
		board_.paint_block_mat(m_hwnd_);
		board_.paint_score(m_hwnd_);

		return 0;
	}

	case WM_KEYDOWN: {
		handle_keydown(w_param);
		return 0;
	}

	case WM_DESTROY: {
		board_.release_resource();
		PostQuitMessage(0);
		return 0;
	}

	case WM_PAINT: {
		on_paint();
		return 0;
	}

	}
	return DefWindowProc(m_hwnd_, u_msg, w_param, l_param);
}

LRESULT MainWindow::handle_keydown(WPARAM _w_param) {
	if(board_.get_failed()) {
		return 0;
	}
	switch (_w_param) {
	case VK_LEFT: {
		if (!board_.is_operable("kLeft")) break;
		handle("kLeft", m_hwnd_);
		break;
	}
	case VK_UP: {
		if (!board_.is_operable("kUp")) break;
		handle("kUp", m_hwnd_);
		break;
	}
	case VK_RIGHT: {
		if (!board_.is_operable("kRight")) break;
		handle("kRight", m_hwnd_);
		break;
	}
	case VK_DOWN: {
		if (!board_.is_operable("kDown")) break;
		handle("kDown", m_hwnd_);
		break;
	}
	}
	/*if (board_.failed()) {*/
	if (true) {
		board_.failed_paint(m_hwnd_);

	}

	return 0;
}

void MainWindow::handle(std::string _str, HWND _hwnd) {
	board_.handle_key(_str, m_hwnd_);
	board_.generate_rand();
	board_.paint_block_mat(m_hwnd_);
	board_.paint_score(m_hwnd_);
}

void MainWindow::on_paint() {
	board_.on_paint(m_hwnd_);
}
