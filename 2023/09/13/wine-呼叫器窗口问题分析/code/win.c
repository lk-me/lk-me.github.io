#include <windows.h>
HWND hwndChild;
HWND hwndParent;
// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        if(hwnd == hwndChild) {
            ShowWindow(hwndParent, SW_SHOW);
            UpdateWindow(hwndParent);
        }else {
            PostQuitMessage(0);
        }

        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 注册父窗口类
    const char* parentClassName = "ParentWindowClass";
    WNDCLASSEX wcParent = {};
    wcParent.cbSize = sizeof(WNDCLASSEX);
    wcParent.lpfnWndProc = WndProc;
    wcParent.hInstance = hInstance;
    wcParent.lpszClassName = parentClassName;
    RegisterClassEx(&wcParent);

    // 注册子窗口类
    const char* childClassName = "ChildWindowClass";
    WNDCLASSEX wcChild = {};
    wcChild.cbSize = sizeof(WNDCLASSEX);
    wcChild.lpfnWndProc = WndProc;
    wcChild.hInstance = hInstance;
    wcChild.lpszClassName = childClassName;
    RegisterClassEx(&wcChild);

    // 创建父窗口
    hwndParent = CreateWindowEx(0, parentClassName, "Parent", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800, NULL, NULL, hInstance, NULL);
    ShowWindow(hwndParent, nCmdShow);
    UpdateWindow(hwndParent);

    // 创建子窗口
    hwndChild = CreateWindowEx(0, childClassName, "child", WS_OVERLAPPEDWINDOW, 200, 200, 200, 150, hwndParent, NULL, hInstance, NULL);
    ShowWindow(hwndParent, SW_HIDE);
    ShowWindow(hwndChild, nCmdShow);
    UpdateWindow(hwndChild);

    // 处理消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // 当子窗口关闭时，显示父窗口
        if (msg.message == WM_DESTROY && msg.hwnd == hwndChild)
        {
            ShowWindow(hwndParent, SW_SHOW);
            UpdateWindow(hwndParent);
        }
    }

    return (int)msg.wParam;
}