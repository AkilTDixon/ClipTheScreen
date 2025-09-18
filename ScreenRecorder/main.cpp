#pragma comment(lib, "rpcrt4.lib") 
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <WinUser.h>
#include <windowsx.h>
#include <chrono>
#include <shellapi.h>
#include "curl/curl.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <fstream>
#include <filesystem>
#include <map>
#include "Window.h"
using namespace std;
using namespace cv;


int main();
bool holding = false;
POINT p, startP, endP;
void capture(int startX, int startY, int endX, int endY, Window* timeWindow);

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    
    switch (msg)
    {
    case WM_MOUSEMOVE:
        if (holding)
        {
            endP.x = LOWORD(lParam);
            endP.y = HIWORD(lParam);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;
    case WM_LBUTTONDOWN:
        return 0;
    case WM_RBUTTONDOWN:
        return 0;
    case WM_PAINT:
        {
        PAINTSTRUCT ps;
        HDC h = BeginPaint(hwnd, &ps);
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        RECT r;
        GetClientRect(hwnd, &r);
        FillRect(h, &r, brush);
        DeleteObject(brush);

        SetBkMode(h, TRANSPARENT);
        SetROP2(h, R2_NOT);
        if (holding)
            Rectangle(h, startP.x, startP.y, endP.x, endP.y);
        

        EndPaint(hwnd, &ps);

        }
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main() {
#ifdef _WIN32
    SetProcessDPIAware();  
#endif


    this_thread::sleep_for(chrono::seconds(1));
     
    p.x = 0;
    p.y = 0;

    int width, height;

    width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    height = GetSystemMetrics(SM_CYVIRTUALSCREEN);


    Window wdo(L"CursorOverlay", WndProc, L"Overlay", WS_POPUP, 0, 0, width, height, true);
    MSG msg;

    
    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (GetKeyState(VK_LBUTTON) < 0) {
            cout << GetKeyState(VK_LBUTTON) << endl;
            if (GetCursorPos(&p)) {
                if (!holding) {
                    holding = true;
                    startP = p;
                    
                }
            }
        }
        else
        {
            if (holding) {
                endP = p;              
                wdo.~Window();

                break;
            }
        }
        
    }
    destroyAllWindows();
    Window t(L"timer", WindowProc, L"Duration", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, endP.x, endP.y, 50, 52, true);
    capture(startP.x, startP.y, endP.x, endP.y, &t);
    t.~Window();
    return 0;
}




void capture(int startX, int startY, int endX, int endY, Window* timeWindow)
{
    map<string, string> config;
    int FPS = 30;
    int clipDuration = 10;
    string outputPath;
    ifstream f;
    string line;


    f.open("config.txt");
    while (getline(f, line))
    {
        auto i = line.find("=");

        auto value = line.substr(i + 1, line.length() - 1);
        
        config[line.substr(0, i)] = value;
    }
    f.close();
    FPS = stoi(config["[fps]"]);
    clipDuration = stoi(config["[clipduration]"]);
    outputPath = config["[outputpath]"];


    if (outputPath != "")
        if (!filesystem::is_directory(outputPath) || outputPath[outputPath.length()-1] != '\\')
        {
            Window error(L"error", WindowProc, L"ERROR", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, startX, startY, 500, 100, false);
            HWND e = error.getHandle();
            HDC hdcTime = GetDC(e);
            RECT rect;
            GetClientRect(e, &rect);
            string eMsg = "ERROR: Output Path doesn't exist (Check config.txt)";
            DrawTextA(hdcTime, eMsg.c_str(), eMsg.length(), &rect, DT_CENTER | DT_VCENTER);
            system("pause");
            return;
        }


    if (clipDuration < 1)
    {
        Window error(L"error", WindowProc, L"ERROR", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, startX, startY, 500, 100, false);
        HWND e = error.getHandle();
        HDC hdcTime = GetDC(e);
        RECT rect;
        GetClientRect(e, &rect);
        string eMsg = "ERROR: Clip duration is less than 1 second (Check config.txt)";
        DrawTextA(hdcTime, eMsg.c_str(), eMsg.length(), &rect, DT_CENTER | DT_VCENTER);
        system("pause");
        return;
    }
    if (FPS < 1)
    {
        Window error(L"error", WindowProc, L"ERROR", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, startX, startY, 500, 100, false);
        HWND e = error.getHandle();
        HDC hdcTime = GetDC(e);
        RECT rect;
        GetClientRect(e, &rect);
        string eMsg = "ERROR: Target FPS is less than 1 frame per second (Check config.txt)";
        DrawTextA(hdcTime, eMsg.c_str(), eMsg.length(), &rect, DT_CENTER | DT_VCENTER);
        system("pause");
        return;
    }


    HWND hwnd = GetDesktopWindow();
    HDC hdcScreen = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen);

    int screenX = abs(startX - endX);
    int screenY = abs(startY - endY);
    if (screenX <= 5 || screenY <= 5)
        return;



    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screenX;
    bmi.bmiHeader.biHeight = -screenY;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    SelectObject(hdcMemDC, hBitmap);

    
    double ms = (1.0 / (double)FPS) * 1000;
    string videoName = outputPath+"vid.mp4";
    wstring wVideoName;
    size_t size;
    wVideoName.resize(videoName.length());
    mbstowcs_s(&size, &wVideoName[0], wVideoName.size() + 1, videoName.c_str(), videoName.size());
   
    VideoWriter writer(videoName,
        VideoWriter::fourcc('H', '2', '6', '4'),
        FPS, Size(screenX, screenY));
    

    double elapsed = ms;
    chrono::steady_clock::time_point last = std::chrono::high_resolution_clock::now();
    chrono::steady_clock::time_point now;

    chrono::steady_clock::time_point absoluteStart = std::chrono::high_resolution_clock::now();

    HWND t = timeWindow->getHandle();
    HDC hdcTime = GetDC(t);
    RECT rect;
    GetClientRect(t, &rect);
    long long totalTime = 0;
    string strTotalTime;

    while (true) {

        if (elapsed >= ms) {
            
            BitBlt(hdcMemDC, 0, 0, screenX, screenY, hdcScreen, startX, startY, SRCCOPY);
        
            Mat mat(screenY, screenX, CV_8UC4, pBits);

            Mat matBGR;
            cvtColor(mat, matBGR, COLOR_BGRA2BGR);

            writer.write(matBGR);
            last = now;
            elapsed = 0;
            
            strTotalTime = to_string(totalTime) + "s / " + to_string(clipDuration-1) + "s";
            DrawTextA(hdcTime, strTotalTime.c_str(), strTotalTime.length(), &rect, DT_CENTER | DT_VCENTER); 
        }
       
        now = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        totalTime = std::chrono::duration_cast<std::chrono::seconds>(now - absoluteStart).count();
        

        if (!timeWindow->ProcessMessages())
            break;
        if (totalTime >= clipDuration)
            break;
        
    }
    
    ReleaseDC(t, hdcTime);
    DeleteObject(hBitmap);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcScreen);
    writer.release();
    //uploadRequest(videoName);
    ShellExecuteW(NULL, L"open", (LPCWSTR)wVideoName.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

string getSignature(string token, string expire, string key)
{
    string digest = token + expire;
    unsigned int theLength = 0;
    auto result = HMAC(EVP_sha1(), key.c_str(), key.length(), reinterpret_cast<const unsigned char*>(digest.c_str()), digest.length(), NULL, &theLength);

    stringstream s;
    s << hex << setfill('0');

    for (unsigned int i = 0; i < theLength; i++)
        s << setw(2) << static_cast<int>(digest[i]);

    return s.str();

}

void uploadRequest(string filename)
{
    UUID uuidT;
    UuidCreate(&uuidT);
    string token;
    UuidToString(&uuidT, (RPC_WSTR*)&token);
    string expire = to_string(time(NULL) + 1000);


    string signature = getSignature(token, expire, "privateKeyHere");
    


    CURL* hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://upload.imagekit.io/api/v1/files/upload");

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: multipart/form-data; boundary=---011000010111000001101001");
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Authorization: Basic 123");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, 
        "-----011000010111000001101001\r\n"
        "Content-Disposition: form-data; name = \"file\"; filename=\""+filename+"\"\r\n"
        "Content-Type: video\r\n"
        "\r\n"
        ""+filename+"-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"fileName\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"publicKey\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"signature\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"expire\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"token\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"useUniqueFileName\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"tags\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"folder\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"isPrivateFile\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"isPublished\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"customCoordinates\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"responseFields\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"extensions\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"webhookUrl\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"overwriteFile\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"overwriteAITags\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"overwriteTags\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"overwriteCustomMetadata\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"customMetadata\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"transformation\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"checks\"\r\n\r\n\r\n-----011000010111000001101001\r\nContent-Disposition: form-data; name=\"description\"\r\n\r\n\r\n-----011000010111000001101001--\r\n");


    CURLcode ret = curl_easy_perform(hnd);
}