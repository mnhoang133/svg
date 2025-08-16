#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>

#include "ParserUtils.h"
#include "SVGParser.h"
#include "SVGElement.h"
#include "SVGGroup.h"

using namespace Gdiplus;
using namespace ParserUtils;

#pragma comment (lib,"Gdiplus.lib")

// Bien toan cuc de luu cay SVG sau khi parse xong
SVGGroup* rootGroup = nullptr;

// Bien zoom va goc xoay
float zoomFactor = 1.0f;
float rotationAngle = 0.0f;

// Vi tri chuot hien tai (dung lam tam zoom)
POINT cursorPos = { 0, 0 };

// Ham ve duoc goi khi WM_PAINT
VOID OnPaint(HDC hdc)
{
    Graphics graphics(hdc);

    // Lay kich thuoc cua cua so ve
    RECT rect;
    GetClientRect(WindowFromDC(hdc), &rect);
    int cx = (rect.right - rect.left) / 2;
    int cy = (rect.bottom - rect.top) / 2;

    // Di chuyen tam quay ve tam cua so
    graphics.TranslateTransform((REAL)cx, (REAL)cy);
    graphics.RotateTransform(rotationAngle);
    graphics.TranslateTransform((REAL)-cx, (REAL)-cy);

    // Zoom quanh vi tri chuot
    graphics.TranslateTransform((REAL)cursorPos.x, (REAL)cursorPos.y);
    graphics.ScaleTransform(zoomFactor, zoomFactor);
    graphics.TranslateTransform((REAL)-cursorPos.x, (REAL)-cursorPos.y);

    // Ve SVG neu co
    logDebug("[MAIN] Trigger render (WM_PAINT)");
    if (rootGroup)
    {
        rootGroup->render(&graphics);
    }
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Ham chinh WinMain
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND hWnd;
    MSG msg;
    WNDCLASS wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    // Khoi tao GDI+
    logDebug("[MAIN] Start parsing SVG file: Instagram_logo_2016.svg");
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Parse file SVG thanh cay SVGGroup
    rootGroup = SVGParser::parseFile("Instagram_logo_2016.svg");

    // Dang ky lop cua so
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

    // Tao cua so
    hWnd = CreateWindow(
        TEXT("SVGWindow"),
        TEXT("SVG Viewer"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    // Vong lap xu ly message
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Giai phong GDI+
    GdiplusShutdown(gdiplusToken);

    // Giai phong bo nho SVG
    delete rootGroup;
    rootGroup = nullptr;

    return static_cast<int>(msg.wParam);
}

// Xu ly cac su kien cua so
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:  // Phim mui ten trai -> xoay goc am
            rotationAngle -= 5.0f;
            InvalidateRect(hWnd, NULL, TRUE); // Yeu cau ve lai
            break;

        case VK_RIGHT: // Phim mui ten phai -> xoay goc duong
            rotationAngle += 5.0f;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        break;
    case WM_MOUSEWHEEL:
    {
        // Lay vi tri chuot tren man hinh roi chuyen ve toa do client
        GetCursorPos(&cursorPos);
        ScreenToClient(hWnd, &cursorPos);

        // Lay delta banh xe chuot
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta > 0)
            zoomFactor *= 1.1f; // Zoom in
        else
            zoomFactor /= 1.1f; // Zoom out

        InvalidateRect(hWnd, NULL, TRUE); // Ve lai man hinh
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0); // Thoat chuong trinh
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
