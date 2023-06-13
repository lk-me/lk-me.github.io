#include <windows.h>
#include <dshow.h>
#include <strmif.h>
#pragma comment(lib, "strmiids.lib")

// 列举系统中的视频捕获设备
HRESULT EnumerateVideoCaptureDevices(IEnumMoniker** ppEnumMoniker) {
    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        return hr;
    }
    ICreateDevEnum* pCreateDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (FAILED(hr)) {
        LPVOID lpMsgBuf;
        DWORD dw = GetLastError();
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
        printf("CoCreateInstance failed with error %d: %s\n", dw, lpMsgBuf);
        WCHAR XX[255];
        LPWSTR buf = XX;
        wsprintf(buf, TEXT("%d"), dw);
        MessageBox(NULL, (LPCWSTR)buf,NULL,S_OK);
        LocalFree(lpMsgBuf);
    }
    if (SUCCEEDED(hr)) {
        hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, ppEnumMoniker, 0);
        pCreateDevEnum->Release();
    }
    return hr;
}

// 获取视频捕获设备的属性页
HRESULT GetVideoCaptureDevicePropertyPages(IBaseFilter* pBaseFilter, CAUUID* pCaGUID) {
    HRESULT hr;
    MessageBox(NULL, L"xxxxxxxxxxxxxxxx", NULL, S_OK);
    ISpecifyPropertyPages* pSpecPropPages = NULL;
    hr = pBaseFilter->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pSpecPropPages);
    if (SUCCEEDED(hr)) {
        hr = pSpecPropPages->GetPages(pCaGUID);
        pSpecPropPages->Release();
    }
    else {
        MessageBox(NULL, L"bbbb", NULL, S_OK);
    }
    return hr;
}

// 弹出视频捕获设备的设置窗口
HRESULT ShowVideoCaptureDevicePropertyPages(HWND hWndParent, IBaseFilter* pBaseFilter) {
    HRESULT hr;
    ISpecifyPropertyPages* pSpecPropPages = NULL;
    hr = pBaseFilter->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pSpecPropPages);
    if (SUCCEEDED(hr)) {
        CAUUID caGUID;
        hr = pSpecPropPages->GetPages(&caGUID);
        if (SUCCEEDED(hr)) {
            IUnknown* pUnknown = NULL;
            hr = pBaseFilter->QueryInterface(IID_IUnknown, (void**)&pUnknown);
            if (SUCCEEDED(hr)) {
                OleCreatePropertyFrame(hWndParent, 0, 0, L"Video Capture Device Settings", 1, &pUnknown, caGUID.cElems, caGUID.pElems, 0, 0, NULL);
                pUnknown->Release();
            }
            CoTaskMemFree(caGUID.pElems);
        }
        pSpecPropPages->Release();
    }
    return hr;
}

// 主窗口过程
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // 列举系统中的视频捕获设备
        IEnumMoniker* pEnumMoniker = NULL;
        if (SUCCEEDED(EnumerateVideoCaptureDevices(&pEnumMoniker))) {
            if (pEnumMoniker == NULL) {
                MessageBox(NULL, L"NULL", NULL, S_OK);
            }
            ULONG cFetched;
            IMoniker* pMoniker = NULL;
            while (pEnumMoniker->Next(1, &pMoniker, &cFetched) == S_OK) {
                IBaseFilter* pBaseFilter = NULL;
                if (SUCCEEDED(pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pBaseFilter))) {
                    CAUUID caGUID;
                   // if (SUCCEEDED(GetVideoCaptureDevicePropertyPages(pBaseFilter, &caGUID))) {
                        // 弹出设置窗口
                    GetVideoCaptureDevicePropertyPages(pBaseFilter, &caGUID);
                        ShowVideoCaptureDevicePropertyPages(hWnd, pBaseFilter);
                        CoTaskMemFree(caGUID.pElems);
                   // }
                    pBaseFilter->Release();
                }
                else {
                    LPVOID lpMsgBuf;
                    DWORD dw = GetLastError();
                    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
                    printf("CoCreateInstance failed with error %d: %s\n", dw, lpMsgBuf);
                    WCHAR XX[255];
                    LPWSTR buf = XX;
                    wsprintf(buf, TEXT("nnnnn:%d"), dw);
                    MessageBox(NULL, (LPCWSTR)buf, NULL, S_OK);
                    LocalFree(lpMsgBuf);
                }
                pMoniker->Release();
            }
            pEnumMoniker->Release();
        }
        else {
            LPVOID lpMsgBuf;
            DWORD dw = GetLastError();
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
            printf("CoCreateInstance failed with error %d: %s\n", dw, lpMsgBuf);
            WCHAR XX[255];
            LPWSTR buf = XX;
            wsprintf(buf, TEXT("xxxx:%d"), dw);
            MessageBox(NULL, (LPCWSTR)buf, NULL, S_OK);
            LocalFree(lpMsgBuf);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Win32程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 注册窗口类
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"DirectShowPropertyPagesExample";
    RegisterClassEx(&wcex);

    // 创建窗口
    HWND hWnd = CreateWindow(L"DirectShowPropertyPagesExample", L"DirectShow Property Pages Example", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        return FALSE;
    }

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}