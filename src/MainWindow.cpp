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
		/*board_.generate_rand();*/
		board_.paint_block_mat(m_hwnd_);

		return 0;
	}

	case WM_KEYDOWN: {
		board_.failed();
		
		switch (w_param) {
		case VK_LEFT: {
			board_.handle_key("kLeft", m_hwnd_);
			break;
		}
		case VK_UP: {
			board_.handle_key("kUp", m_hwnd_);
			break;
		}
		case VK_RIGHT: {
			board_.handle_key("kRight", m_hwnd_);
			break;
		}
		case VK_DOWN: {
			board_.handle_key("kDown", m_hwnd_);
			break;
		}
		}
		board_.generate_rand();
		board_.paint_block_mat(m_hwnd_);

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

void MainWindow::on_paint() {
	board_.on_paint(m_hwnd_);
}