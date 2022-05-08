#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include <d3d9.h>

#include "Engine/CRayCaster.h"
#include "Engine/CPlayer.h"
#include "Engine/Math.h"

#include <array>
#include "imgui/imgui_internal.h"
#include <vector>
#include <fmt/format.h>
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Engine::CPlayer g_Player = Engine::CPlayer(ImVec2(300, 300) / 2, 1.f, 60.f);

ImVec2 g_GameScreenSize = ImVec2(1280, 720);


std::array<std::array<int, 11>, 11> map;

Engine::CRayCaster g_traceRay = Engine::CRayCaster(250, &map);


int main(int, char**)
{
    map[0]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    map[1]  = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    map[2]  = { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
    map[3]  = { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 };
    map[4]  = { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 };
    map[5]  = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    map[6]  = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    map[7]  = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 };
    map[8]  = { 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1 };
    map[9]  = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    map[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    RegisterClassEx(&wc);


    HWND hwnd = CreateWindow(wc.lpszClassName, L"RayCast app", WS_OVERLAPPEDWINDOW, 100, 100, 1920, 1080, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

    while (true)
    {

        if (GetAsyncKeyState((int)'W') & 0x8000)
        {
            g_Player.MoveForward();

            auto hitPoint = g_traceRay.TraceRay(g_Player.m_vecOrigin, g_Player.m_fViewAngle).endPoint;
            if (Engine::ImVec2DistTo(g_Player.m_vecOrigin, hitPoint) <= 0.f)
            {
                g_Player.MoveBackWard();
            }

        }
        if (GetAsyncKeyState((int)'S') & 0x8000)
        {
            g_Player.MoveBackWard();

            auto hitPoint = g_traceRay.TraceRay(g_Player.m_vecOrigin, g_Player.m_fViewAngle).endPoint;
            if (Engine::ImVec2DistTo(g_Player.m_vecOrigin, hitPoint) <= 0.f)
            {
                g_Player.MoveForward();
            }
        }
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                exit(0);
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Window");
        {
            ImGui::SliderFloat("Roation", &g_Player.m_fViewAngle, 0, 360);
            ImGui::SliderFloat("Fov", &g_Player.m_fFieldOfView, 1, 120);
            ImGui::SliderFloat("Ray Limit", &g_traceRay.m_fLimit, 1, 500);
            ImGui::SliderFloat("Ray step size", &g_traceRay.m_fStepSize, 0.1, 10.f);
            ImGui::End();
        }
        auto pDrawList = ImGui::GetBackgroundDrawList();


        float step = g_Player.m_fFieldOfView / g_GameScreenSize.x;

        ImVec2 camsceenstart = ImVec2(341, 1);

        for (float diff = g_Player.m_fViewAngle - g_Player.m_fFieldOfView / 2.f; diff < g_Player.m_fViewAngle + g_Player.m_fFieldOfView / 2; diff += step)
        {

            auto hit = g_traceRay.TraceRay(g_Player.m_vecOrigin, diff);

            if (hit.isHit)
            {
                float legth = Engine::ImVec2DistTo(g_Player.m_vecOrigin, hit.endPoint);

                float halfScreenHeight = g_GameScreenSize.y / 2;
                float brightnes = 1 - legth / g_traceRay.m_fLimit;
                float size = g_GameScreenSize.y * (1 - legth / g_traceRay.m_fLimit);

                pDrawList->AddLine(camsceenstart + ImVec2(0, halfScreenHeight - size / 2.f), camsceenstart + ImVec2(0, halfScreenHeight + size / 2.f),
                    ImColor(brightnes, brightnes, brightnes), 1);
            }


            camsceenstart.x += 1.f;
        }
        auto start = ImVec2();


        for (int i = 0; i < map.size(); ++i)
        {
            start.x = 0;
            for (int j = 0; j < map[0].size(); ++j)
            {
                if (map[i][j] == 0)
                    pDrawList->AddRect(start, start + ImVec2(30, 30), ImColor(0, 255, 0));

                if (map[i][j] == 1)
                    pDrawList->AddRectFilled(start, start + ImVec2(30, 30), ImColor(0, 0, 255));

                start.x += 30;
            }
            start.y += 30;
        }

        //pDrawList->AddRectFilled(ImVec2(340, 0), screen + ImVec2(1, 1), ImColor(255, 0, 0));

        pDrawList->AddText(ImVec2(), ImColor(255, 255, 255), fmt::format("FPS: {}", ImGui::GetIO().Framerate).c_str());
        pDrawList->AddCircleFilled(g_Player.m_vecOrigin, 6, ImColor(255, 0, 0));


        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);


        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
