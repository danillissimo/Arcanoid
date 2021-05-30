#pragma once

#include <windows.h>

namespace Console {
    void SetCursorVisibility(bool value) {
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_CURSOR_INFO     cursorInfo;

        GetConsoleCursorInfo(out, &cursorInfo);
        cursorInfo.bVisible = value; // set the cursor visibility
        SetConsoleCursorInfo(out, &cursorInfo);
    }

    void Ñlean() {
        system("cls");
        //COORD topLeft = { 0, 0 };
        //HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        //CONSOLE_SCREEN_BUFFER_INFO screen;
        //DWORD written;

        //GetConsoleScreenBufferInfo(console, &screen);
        //FillConsoleOutputCharacterA(
        //    console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        //);
        //FillConsoleOutputAttribute(
        //    console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        //    screen.dwSize.X * screen.dwSize.Y, topLeft, &written
        //);
        //SetConsoleCursorPosition(console, topLeft);
    }
}