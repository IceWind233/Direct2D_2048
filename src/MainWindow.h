#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <Windows.h>
#include <map>

#include "BaseWin.h"
#include "Board.h"

class MainWindow : public Base_Win<MainWindow> {

public:

	MainWindow();

public:

	PCWSTR  class_name() const { return L"Circle Window Class"; }

	LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT handle_keydown(WPARAM _wparam);

	void handle(const std::string& _str);

	void on_paint();

private:

	BoardController board_controller_;

};

#endif