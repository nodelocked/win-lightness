#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>

// DDC/CI (dxva2) minimal declarations to avoid extra headers
typedef struct _PHYSICAL_MONITOR {
    HANDLE hPhysicalMonitor;
    WCHAR szPhysicalMonitorDescription[128];
} PHYSICAL_MONITOR, *LPPHYSICAL_MONITOR;

extern "C" {
__declspec(dllimport) BOOL __stdcall GetNumberOfPhysicalMonitorsFromHMONITOR(
    HMONITOR hMonitor, LPDWORD pdwNumberOfPhysicalMonitors);
__declspec(dllimport) BOOL __stdcall GetPhysicalMonitorsFromHMONITOR(
    HMONITOR hMonitor, DWORD dwPhysicalMonitorArraySize, PHYSICAL_MONITOR* pPhysicalMonitorArray);
__declspec(dllimport) BOOL __stdcall DestroyPhysicalMonitors(
    DWORD dwPhysicalMonitorArraySize, PHYSICAL_MONITOR* pPhysicalMonitorArray);
__declspec(dllimport) BOOL __stdcall GetVCPFeatureAndVCPFeatureReply(
    HANDLE hMonitor, BYTE bVCPCode, LPDWORD pvct, LPDWORD pdwCurrentValue, LPDWORD pdwMaximumValue);
__declspec(dllimport) BOOL __stdcall SetVCPFeature(
    HANDLE hMonitor, BYTE bVCPCode, DWORD dwNewValue);
}

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dxva2.lib")

static const int kEdgePixels = 4;   // trigger zone at top edge
static const int kStep = 5;         // brightness step per wheel notch
static const bool kPassThroughScroll = false;

static HHOOK g_mouseHook = NULL;

static int Clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static bool IsAtTopEdge(POINT pt) {
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (!mon) return false;
    MONITORINFO mi = {0};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(mon, &mi)) return false;
    return pt.y <= (mi.rcMonitor.top + kEdgePixels);
}

static void AdjustBrightnessAtPoint(POINT pt, int deltaSteps) {
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (!mon) return;

    DWORD count = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(mon, &count) || count == 0) return;

    PHYSICAL_MONITOR* pMons = (PHYSICAL_MONITOR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PHYSICAL_MONITOR) * count);
    if (!pMons) return;

    if (!GetPhysicalMonitorsFromHMONITOR(mon, count, pMons)) {
        HeapFree(GetProcessHeap(), 0, pMons);
        return;
    }

    for (DWORD i = 0; i < count; ++i) {
        DWORD cur = 0, max = 0;
        if (GetVCPFeatureAndVCPFeatureReply(pMons[i].hPhysicalMonitor, 0x10, NULL, &cur, &max) && max > 0) {
            int newVal = Clamp((int)cur + deltaSteps * kStep, 0, (int)max);
            SetVCPFeature(pMons[i].hPhysicalMonitor, 0x10, (DWORD)newVal);
        }
    }

    DestroyPhysicalMonitors(count, pMons);
    HeapFree(GetProcessHeap(), 0, pMons);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_MOUSEWHEEL)) {
        MSLLHOOKSTRUCT* s = (MSLLHOOKSTRUCT*)lParam;
        if (IsAtTopEdge(s->pt)) {
            int wheelDelta = GET_WHEEL_DELTA_WPARAM(s->mouseData);
            int steps = (wheelDelta / WHEEL_DELTA);
            if (steps != 0) {
                AdjustBrightnessAtPoint(s->pt, steps);
                if (!kPassThroughScroll) return 1; // swallow scroll
            }
        }
    }
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    if (!g_mouseHook) return 1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_mouseHook);
    return 0;
}
