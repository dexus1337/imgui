#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_x11.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
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

static ImGuiKey ImGui_ImplX11_KeyCode_to_ImKey(int key)
{
    switch( key )
    {
        case 67:    return ImGuiKey_F1;
        case 68:    return ImGuiKey_F2;
        case 69:    return ImGuiKey_F3;
        case 70:    return ImGuiKey_F4;
        case 71:    return ImGuiKey_F5;
        case 72:    return ImGuiKey_F6;
        case 73:    return ImGuiKey_F7;
        case 74:    return ImGuiKey_F8;
        case 75:    return ImGuiKey_F9;
        case 76:    return ImGuiKey_F10;
        case 95:    return ImGuiKey_F11;
        case 96:    return ImGuiKey_F12;

        case 10:    return ImGuiKey_1;
        case 11:    return ImGuiKey_2;
        case 12:    return ImGuiKey_3;
        case 13:    return ImGuiKey_4;
        case 14:    return ImGuiKey_5;
        case 15:    return ImGuiKey_6;
        case 16:    return ImGuiKey_7;
        case 17:    return ImGuiKey_8;
        case 18:    return ImGuiKey_9;
        case 19:    return ImGuiKey_0;
        case 20:    return ImGuiKey_Minus;
        case 21:    return ImGuiKey_Equal;
        case 22:    return ImGuiKey_Backspace;

        case 24:    return ImGuiKey_Q;
        case 25:    return ImGuiKey_W;
        case 26:    return ImGuiKey_E;
        case 27:    return ImGuiKey_R;
        case 28:    return ImGuiKey_T;
        case 29:    return ImGuiKey_Y;
        case 30:    return ImGuiKey_U;
        case 31:    return ImGuiKey_I;
        case 32:    return ImGuiKey_O;
        case 33:    return ImGuiKey_P;
        case 34:    return ImGuiKey_LeftBracket;
        case 35:    return ImGuiKey_RightBracket;
        //case 35:    return ImGuiKey_Bar;

        case 36:    return ImGuiKey_Enter;

        case 38:    return ImGuiKey_A;
        case 39:    return ImGuiKey_S;
        case 40:    return ImGuiKey_D;
        case 41:    return ImGuiKey_F;
        case 42:    return ImGuiKey_G;
        case 43:    return ImGuiKey_H;
        case 44:    return ImGuiKey_J;
        case 45:    return ImGuiKey_K;
        case 46:    return ImGuiKey_L;

        case 118:   return ImGuiKey_Insert;
        case 110:   return ImGuiKey_Home;
        case 112:   return ImGuiKey_PageUp;
        case 119:   return ImGuiKey_Delete;
        case 115:   return ImGuiKey_End;
        case 117:   return ImGuiKey_PageDown;

        case 1337:  return ImGuiKey_PrintScreen;
        case 78:    return ImGuiKey_ScrollLock;
        case 127:   return ImGuiKey_Pause;

        case 113:   return ImGuiKey_LeftArrow;
        case 116:   return ImGuiKey_DownArrow;
        case 114:   return ImGuiKey_RightArrow;
        case 111:   return ImGuiKey_UpArrow;

        case 50:    return ImGuiKey_LeftShift;
        case 62:    return ImGuiKey_RightShift;

        case 37:    return ImGuiKey_LeftSuper;
        //case 62:    return ImGuiKey_RightSuper;

        case 64:    return ImGuiKey_LeftAlt;
        case 65:    return ImGuiKey_Space;
        case 108:   return ImGuiKey_RightAlt;

        case 66:    return ImGuiKey_CapsLock;
        case 23:    return ImGuiKey_Tab;

        case 9:     return ImGuiKey_Escape;

        default:
            return ImGuiKey_None;
    }
}


IMGUI_IMPL_API void ImGui_ImplX11_KeyEvent( int native_keycode, bool down )
{
    ImGuiIO& io     = ImGui::GetIO( );
    ImGuiKey key    = ImGui_ImplX11_KeyCode_to_ImKey( native_keycode );

    io.AddKeyEvent( key, down );
    io.SetKeyEventNativeData( key, native_keycode, 0 ); // To support legacy indexing (<1.87 user code)

    if ( down )
    {
        ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData( );

        char test = XKeycodeToKeysym( bd->display, native_keycode, 0 );

        //printf( "io: %d -> %c\n", test, test );

        if( test > 0 )
            io.AddInputCharacter( test );
    }
}

IMGUI_IMPL_API void ImGui_ImplX11_MouseEvent( int native_buttoncode, bool down )
{
    ImGuiIO& io             = ImGui::GetIO();
    ImGuiMouseButton curbtn = 0;
    int mode                = 0;
    float wheel             = 0.f;

    switch( native_buttoncode )
    {
        case 1:   mode = 1;  curbtn = ImGuiMouseButton_Left;    break;
        case 2:   mode = 1;  curbtn = ImGuiMouseButton_Middle;  break;
        case 3:   mode = 1;  curbtn = ImGuiMouseButton_Right;   break;
        case 4:   mode = 2;  wheel  = 1.f;                      break;
        case 5:   mode = 2;  wheel  = -1.f;                     break;
        default:  return;
    }

    if( mode = 1 )
        io.AddMouseButtonEvent( curbtn, down );
    else if( mode = 2 )
        io.AddMouseWheelEvent( 0.f, wheel );
}

IMGUI_IMPL_API bool ImGui_ImplX11_OnEvent(void* event)
{
    if( !ImGui::GetCurrentContext( ) )
        return true;

    XEvent* evt = reinterpret_cast< XEvent* >( event );
    ImGuiIO& io = ImGui::GetIO( );
    ImGui_ImplX11_Data* bd = ImGui_ImplX11_GetBackendData( );

    switch( evt->type )
    {
        case KeyPress:
            ImGui_ImplX11_KeyEvent( evt->xkey.keycode, true );
            break;
        case KeyRelease:
            ImGui_ImplX11_KeyEvent( evt->xkey.keycode, false );
            break;
        case ButtonPress:
            ImGui_ImplX11_MouseEvent( evt->xbutton.button, true );
            break;
        case ButtonRelease:
            ImGui_ImplX11_MouseEvent( evt->xbutton.button, false );
            break;
        case MotionNotify:
            /* TODO: check when to use _root variables */
            io.AddMousePosEvent( static_cast< float >( evt->xmotion.x ), static_cast< float >( evt->xmotion.y ) );
            break;
        default:
            break;
    }

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