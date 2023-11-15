#include <windows.h>
#include <olectl.h>
#include <iostream>
using namespace std;


// 定义窗口类名
const char* g_WindowClassName = "MyWindowClass";

// 定义图像文件路径
char* g_ImageFilePath = "C:\\image.jpg";

const IID IID_IMyInterface =
{ 0x7bf80980, 0xbf32, 0x101a, { 0x8b, 0xbb, 0x00, 0xaa, 0x00, 0x30, 0x0c, 0xab } };

// 将图像文件读取到LPSTREAM中
LPSTREAM ReadImageToLPSTREAM()
{
    // 打开图像文件
    HANDLE hFile = CreateFile(g_ImageFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // 处理文件打开错误
        return NULL;
    }

    // 获取文件大小
    DWORD dwFileSize = GetFileSize(hFile, NULL);

    // 创建内存流
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
    LPSTREAM pStream = NULL;
    if (hGlobal != NULL && CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
    {
        // 读取文件内容到内存流
        DWORD dwBytesRead = 0;
        if (ReadFile(hFile, GlobalLock(hGlobal), dwFileSize, &dwBytesRead, NULL) && dwBytesRead == dwFileSize)
        {
            GlobalUnlock(hGlobal);
            CloseHandle(hFile);
            return pStream;
        }
    }

    // 处理读取错误
    if (pStream != NULL)
    {
        pStream->Release();
    }

    CloseHandle(hFile);
    return NULL;
}

// 加载图像并设置为窗口图标
HICON LoadAndSetWindowIcon()
{
    printf("xxxxxxxxx\n");
    // 初始化 COM
    CoInitialize(NULL);

    // 假设pStream已经被初始化并包含了图片数据
    IStream *pStream = ReadImageToLPSTREAM();

    // 图片的宽度和高度（设为0表示使用默认的图片尺寸）
    LONG width = 0;
    LONG height = 0;

    // 是否保持图片的比例
    BOOL keepAspectRatio = TRUE;

    // 加载图片
    IPicture *pPicture = NULL;
    HRESULT hr = OleLoadPictureEx(pStream, 0, FALSE, IID_IMyInterface, 0, 0, 0, (void **)&pPicture);
    printf("bbbbbbbbbbb\n");
    if (SUCCEEDED(hr))
    {
        // 获取图标句柄
        HICON hIcon = NULL;
        pPicture->get_Handle((OLE_HANDLE*)&hIcon);
        if (hIcon ==NULL) {
            printf("1111111111\n");
        }
        return hIcon;
        // 设置窗口类的图标
        HINSTANCE hInstance = GetModuleHandle(NULL);
        //HICON hPrevIcon = (HICON)SetClassLongPtr(hWnd, GCLP_HICON, (LONG_PTR)hIcon);

        // 释放先前的图标资源
        // if (hPrevIcon != NULL && hPrevIcon != LoadIcon(hInstance, IDI_APPLICATION))
        // {
        //     DestroyIcon(hPrevIcon);
        // }
        printf("cccccccccccccccc\n");
    }

    // 释放 IPicture 接口
    if (pPicture != NULL)
    {
        pPicture->Release();
    }

    // 释放 COM
    CoUninitialize();
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
            // 加载并设置窗口图标
            //LoadAndSetWindowIcon(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}


// 程序入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 注册窗口类
    HICON hIcon=LoadAndSetWindowIcon();
    WNDCLASSEX wndClass = { 0 };
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = g_WindowClassName;
    wndClass.hIcon = hIcon;
    RegisterClassEx(&wndClass);
//CreateWindowExA ( WS_EX_NOPARENTNOTIFY, 49817, "ÖØÉèÃÜÂë¡ý", WS_CHILD | WS_CLIPSIBLINGS | WS_MAXIMIZEBOX | 8192, 192, 152, 73, 25, 0x001905dc, 0x00000009, 0x64560000, NULL )
    // 创建窗口
    HWND hWnd = CreateWindowEx(WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_WINDOWEDGE|WS_EX_APPWINDOW
    , g_WindowClassName, "My Window",  WS_VISIBLE|WS_CLIPSIBLINGS|WS_SYSMENU|WS_THICKFRAME|WS_OVERLAPPED|WS_MINIMIZEBOX|WS_CAPTION, 
                               192, 152, 73, 25, NULL, NULL, hInstance, NULL);
    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}