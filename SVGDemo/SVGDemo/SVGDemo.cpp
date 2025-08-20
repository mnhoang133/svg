#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>

#include "ParserUtils.h"
#include "SVGParser.h"
#include "SVGElement.h"
#include "SVGGroup.h"

using namespace Gdiplus;
using namespace ParserUtils;
namespace fs = std::filesystem;

#pragma comment (lib,"Gdiplus.lib")

// Bien toan cuc de luu cay SVG sau khi parse xong
SVGGroup* rootGroup = nullptr;

// Bien zoom va goc xoay
float zoomFactor = 1.0f;
float rotationAngle = 0.0f;

// Vi tri chuot hien tai (dung lam tam zoom)
POINT cursorPos = { 0, 0 };

// Ham ve
VOID OnPaint(HDC hdc)
{
    Graphics graphics(hdc);

    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    int cx = (rect.right - rect.left) / 2;
    int cy = (rect.bottom - rect.top) / 2;

    graphics.TranslateTransform((REAL)cx, (REAL)cy);
    graphics.RotateTransform(rotationAngle);
    graphics.TranslateTransform((REAL)-cx, (REAL)-cy);

    graphics.TranslateTransform((REAL)cursorPos.x, (REAL)cursorPos.y);
    graphics.ScaleTransform(zoomFactor, zoomFactor);
    graphics.TranslateTransform((REAL)-cursorPos.x, (REAL)-cursorPos.y);

    if (rootGroup) {
        rootGroup->render(&graphics);
    }
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Hàm chọn file SVG từ console
std::string SelectSVGFile() {
    std::vector<std::string> svgFiles;

    for (auto& p : fs::directory_iterator(".")) {
        if (p.path().extension() == ".svg") {
            svgFiles.push_back(p.path().filename().string());
        }
    }

    if (svgFiles.empty()) {
        std::cout << "No SVG files found in current directory." << std::endl;
        return "";
    }

    std::cout << "Available SVG files:\n";
    for (size_t i = 0; i < svgFiles.size(); ++i) {
        std::cout << i + 1 << ". " << svgFiles[i] << "\n";
    }

    int choice;
    std::cout << "Select file number to open: ";
    std::cin >> choice;

    if (choice < 1 || choice >(int)svgFiles.size()) {
        std::cout << "Invalid choice." << std::endl;
        return "";
    }

    return svgFiles[choice - 1];
}

// Ham chinh WinMain
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    // Tạo console để nhập dữ liệu
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);

    // Hỏi người dùng chọn file SVG
    std::string filename = SelectSVGFile();
    if (filename.empty()) {
        MessageBox(NULL, TEXT("No SVG selected!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Parse file SVG được chọn
    rootGroup = SVGParser::parseFile(filename);

    WNDCLASS wndClass;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("SVGWindow");
    RegisterClass(&wndClass);

    HWND hWnd = CreateWindow(
        TEXT("SVGWindow"),
        TEXT("SVG Viewer"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    delete rootGroup;
    rootGroup = nullptr;

    return static_cast<int>(msg.wParam);
}

// Xu ly su kien
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            rotationAngle -= 5.0f;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case VK_RIGHT:
            rotationAngle += 5.0f;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        break;
    case WM_MOUSEWHEEL:
        GetCursorPos(&cursorPos);
        ScreenToClient(hWnd, &cursorPos);

        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            zoomFactor *= 1.1f;
        else
            zoomFactor /= 1.1f;

        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
