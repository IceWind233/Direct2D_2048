#include <Windows.h>

#include "MainWindow.h"

#include <iostream>

MainWindow::MainWindow() = default;

LRESULT MainWindow::handle_message(UINT u_msg, WPARAM w_param, LPARAM l_param) {
	switch (u_msg) {
	case WM_CREATE: {
		board_controller_.init_paint(m_hwnd_);
		board_controller_.generate_rand();
		board_controller_.generate_rand();

		return 0;
	}

	case WM_KEYDOWN: {
		handle_keydown(w_param);
		return 0;
	}

	case WM_COMMAND: {
		switch (LOWORD(w_param)) {
		case BN_CLICKED: {
			board_controller_.reset_board();
			SetFocus(m_hwnd_);
			break;
		}
		}
		return 0;
	}

	case WM_DESTROY: {
		board_controller_.save_highest_score();
		board_controller_.release_resource();
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(m_hwnd_, u_msg, w_param, l_param);
}

LRESULT MainWindow::handle_keydown(WPARAM _w_param) {
	if(board_controller_.get_failed()) {
		board_controller_.reset_board();
		return 0;
	}
	switch (_w_param) {
	case VK_LEFT: {
		if (!board_controller_.is_operable("kLeft")) break;
		handle("kLeft");
		break;
	}
	case VK_UP: {
		if (!board_controller_.is_operable("kUp")) break;
		handle("kUp");
		break;
	}
	case VK_RIGHT: {
		if (!board_controller_.is_operable("kRight")) break;
		handle("kRight");
		break;
	}
	case VK_DOWN: {
		if (!board_controller_.is_operable("kDown")) break;
		handle("kDown");
		break;
	}
	default: {
		return 0;
	}
	}

	return 0;
}

void MainWindow::handle(const std::string& _str) {
	board_controller_.handle_key(_str);

}

void MainWindow::on_paint() {
	board_controller_.on_paint();
}
