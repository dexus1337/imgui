#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_x11.h"
#include <X11/Xlib.h>
#include <stdio.h>


struct ImGui_ImplX11_Data
{
    Display*                    display;
	Window                      window;
	
    ImGui_ImplX11_Data()		{ ::memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
// FIXME: multi-context support is not well tested and probably dysfunctional in this backend.
// FIXME: some shared resources (mouse cursor shape, gamepad) are mishandled when using multi-context.
static ImGui_ImplX11_Data* ImGui_ImplX11_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplX11_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

IMGUI_IMPL_API bool ImGui_ImplX11_Init(void* display, int window)
{
	ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

	ImGui_ImplX11_Data* bd = IM_NEW(ImGui_ImplX11_Data)();

    bd->display = reinterpret_cast< Display* >(display);
    bd->window  = static_cast< Window >(window);

    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_x11";
	
	return true;
}

IMGUI_IMPL_API void ImGui_ImplX11_Shutdown()
{
	ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad);
    IM_DELETE(bd);
}

IMGUI_IMPL_API void ImGui_ImplX11_NewFrame()
{
	ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplWin32_Init()?");

    XWindowAttributes attr ={};
    XGetWindowAttributes( bd->display, bd->window, &attr);

    io.DisplaySize.x = static_cast<float>(attr.width);
    io.DisplaySize.y = static_cast<float>(attr.height);
}


#endif // #ifndef IMGUI_DISABLE