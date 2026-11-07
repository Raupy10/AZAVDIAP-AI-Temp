#include "Platform.h"
#include <atomic>
#include <cctype>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cwchar>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <objbase.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#else
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

namespace Platform {

bool ParseCombo(const std::string& text, Combo& out){
    out = Combo();
    size_t start = 0;
    std::string key;
    while(true){
        size_t p = text.find('+', start);
        std::string part = text.substr(start, p == std::string::npos ? std::string::npos : p - start);
        if(p == std::string::npos){ key = part; break; }
        if(part == "Ctrl") out.ctrl = true;
        else if(part == "Alt") out.alt = true;
        else if(part == "Shift") out.shift = true;
        start = p + 1;
    }
    if(key.empty()) return false;

    int fn = 0;
    if(key.size() >= 2 && key[0] == 'F') fn = std::atoi(key.c_str() + 1);
    if(fn >= 1 && fn <= 24){
#ifdef _WIN32
        out.key = 0x70 + fn - 1;
#else
        out.key = XK_F1 + fn - 1;
#endif
        return true;
    }
    if(key.size() == 1 && std::isalnum((unsigned char)key[0])){
        char c = key[0];
#ifdef _WIN32
        out.key = std::toupper((unsigned char)c);
#else
        out.key = std::tolower((unsigned char)c);
#endif
        return true;
    }
    return false;
}

#ifdef _WIN32

static std::string ToUtf8(const wchar_t* w){
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
    if(n <= 1) return "";
    std::string s(n - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w, -1, &s[0], n, nullptr, nullptr);
    return s;
}

static std::wstring ToWide(const std::string& s){
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    if(n <= 1) return L"";
    std::wstring w(n - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], n);
    return w;
}

static bool Down(int vk){ return (GetAsyncKeyState(vk) & 0x8000) != 0; }

bool KeysInit(){ return true; }
void KeysClose(){}
void SampleKeys(){}

bool ComboDown(const Combo& c){
    if(!Down(c.key)) return false;
    return c.ctrl == Down(VK_CONTROL) && c.alt == Down(VK_MENU) && c.shift == Down(VK_SHIFT);
}

std::string PickFile(const std::string& startDir){
    wchar_t buf[MAX_PATH * 4] = {0};
    std::wstring dir = ToWide(startDir);
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"MP3 (*.mp3)\0*.mp3\0";
    ofn.lpstrFile = buf;
    ofn.nMaxFile = MAX_PATH * 4;
    ofn.lpstrInitialDir = dir.empty() ? nullptr : dir.c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if(GetOpenFileNameW(&ofn)) return ToUtf8(buf);
    return "";
}

static int CALLBACK FolderInit(HWND hwnd, UINT msg, LPARAM, LPARAM data){
    if(msg == BFFM_INITIALIZED && data) SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, data);
    return 0;
}

std::string PickFolder(const std::string& startDir){
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    std::wstring dir = ToWide(startDir);
    wchar_t name[MAX_PATH] = {0};
    BROWSEINFOW bi = {};
    bi.pszDisplayName = name;
    bi.lpszTitle = L"Ordner ausw\u00e4hlen";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = FolderInit;
    bi.lParam = dir.empty() ? 0 : (LPARAM)dir.c_str();
    std::string result;
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if(pidl){
        wchar_t path[MAX_PATH] = {0};
        if(SHGetPathFromIDListW(pidl, path)) result = ToUtf8(path);
        CoTaskMemFree(pidl);
    }
    return result;
}

std::string RunCommand(const std::string&){ return ""; }

namespace {
constexpr UINT WM_TRAY = WM_APP + 1;
constexpr UINT ID_OPEN = 1001;
constexpr UINT ID_QUIT = 1002;
HWND g_hwnd = nullptr;
WNDPROC g_oldProc = nullptr;
NOTIFYICONDATAW g_nid = {};
bool g_added = false;
UINT g_taskbarMsg = 0;
std::atomic<bool> g_quit{false};
std::atomic<bool> g_hidden{false};

void ShowMain(){
    ShowWindow(g_hwnd, SW_SHOW);
    ShowWindow(g_hwnd, SW_RESTORE);
    SetForegroundWindow(g_hwnd);
    g_hidden = false;
}

LRESULT CALLBACK TrayProc(HWND h, UINT msg, WPARAM wp, LPARAM lp){
    if(msg == g_taskbarMsg && g_added){
        Shell_NotifyIconW(NIM_ADD, &g_nid);
        return 0;
    }
    if(msg == WM_CLOSE && g_added){
        ShowWindow(h, SW_HIDE);
        g_hidden = true;
        return 0;
    }
    if(msg == WM_TRAY){
        if(lp == WM_LBUTTONUP || lp == WM_LBUTTONDBLCLK){
            ShowMain();
        }
        else if(lp == WM_RBUTTONUP || lp == WM_CONTEXTMENU){
            HMENU menu = CreatePopupMenu();
            AppendMenuW(menu, MF_STRING, ID_OPEN, L"\u00d6ffnen");
            AppendMenuW(menu, MF_STRING, ID_QUIT, L"Beenden");
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(h);
            UINT cmd = (UINT)TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, pt.x, pt.y, 0, h, nullptr);
            DestroyMenu(menu);
            PostMessageW(h, WM_NULL, 0, 0);
            if(cmd == ID_OPEN) ShowMain();
            else if(cmd == ID_QUIT) g_quit = true;
        }
        return 0;
    }
    return CallWindowProcW(g_oldProc, h, msg, wp, lp);
}
}

bool AlreadyRunning(){
    HANDLE mutex = CreateMutexW(nullptr, TRUE, L"Local\\AZAVDIAP_SingleInstance");
    if(mutex && GetLastError() == ERROR_ALREADY_EXISTS){
        HWND other = FindWindowW(nullptr, L"AZAVDIAP");
        if(other) PostMessageW(other, WM_TRAY, 0, WM_LBUTTONUP);
        return true;
    }
    return false;
}

bool TrayInit(void* nativeWindow, const char* tip){
    g_hwnd = (HWND)nativeWindow;
    if(!g_hwnd) return false;
    g_taskbarMsg = RegisterWindowMessageW(L"TaskbarCreated");
    g_oldProc = (WNDPROC)SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)TrayProc);

    g_nid = NOTIFYICONDATAW();
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAY;
    g_nid.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(1));
    if(!g_nid.hIcon) g_nid.hIcon = LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
    std::wstring wtip = ToWide(tip);
    wcsncpy(g_nid.szTip, wtip.c_str(), sizeof(g_nid.szTip) / sizeof(wchar_t) - 1);
    g_added = Shell_NotifyIconW(NIM_ADD, &g_nid) == TRUE;
    return g_added;
}

void TrayShutdown(){
    if(g_added) Shell_NotifyIconW(NIM_DELETE, &g_nid);
    g_added = false;
}

bool TrayQuitRequested(){ return g_quit; }
bool WindowHidden(){ return g_hidden; }

#else

static Display* dpy = nullptr;
static char keymap[32] = {0};

static bool KeyDownSym(KeySym ks){
    if(!dpy) return false;
    KeyCode kc = XKeysymToKeycode(dpy, ks);
    if(!kc) return false;
    return (keymap[kc / 8] & (1 << (kc % 8))) != 0;
}

static bool Either(KeySym a, KeySym b){ return KeyDownSym(a) || KeyDownSym(b); }

bool KeysInit(){
    dpy = XOpenDisplay(nullptr);
    return dpy != nullptr;
}

void KeysClose(){
    if(dpy) XCloseDisplay(dpy);
    dpy = nullptr;
}

void SampleKeys(){
    if(dpy) XQueryKeymap(dpy, keymap);
}

bool ComboDown(const Combo& c){
    if(!KeyDownSym((KeySym)c.key)) return false;
    return c.ctrl == Either(XK_Control_L, XK_Control_R)
        && c.alt == Either(XK_Alt_L, XK_Alt_R)
        && c.shift == Either(XK_Shift_L, XK_Shift_R);
}

static std::string Quote(const std::string& s){
    std::string r = "'";
    for(char c : s){
        if(c == '\'') r += "'\\''";
        else r += c;
    }
    return r + "'";
}

std::string RunCommand(const std::string& cmd){
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return "";
    char buffer[512];
    std::string result;
    while(fgets(buffer, sizeof(buffer), pipe)) result += buffer;
    pclose(pipe);
    while(!result.empty() && (result.back() == '\n' || result.back() == '\r')) result.pop_back();
    return result;
}

std::string PickFile(const std::string& startDir){
    std::string cmd = "zenity --file-selection --file-filter='Audio files (mp3) | *.mp3' --title='Sound auswählen'";
    if(!startDir.empty()) cmd += " --filename=" + Quote(startDir + "/");
    return RunCommand(cmd + " 2>/dev/null");
}

bool AlreadyRunning(){ return false; }
bool TrayInit(void*, const char*){ return false; }
void TrayShutdown(){}
bool TrayQuitRequested(){ return false; }
bool WindowHidden(){ return false; }

std::string PickFolder(const std::string& startDir){
    std::string cmd = "zenity --file-selection --directory --title='Ordner auswählen'";
    if(!startDir.empty()) cmd += " --filename=" + Quote(startDir + "/");
    return RunCommand(cmd + " 2>/dev/null");
}

#endif

}
