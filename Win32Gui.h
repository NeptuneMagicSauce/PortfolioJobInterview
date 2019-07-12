#include <windowsx.h> // Button_Enable()
#include <set>
using std::set;

class Win32Gui : public GuiGlue
{
    public:
        Win32Gui(const HINSTANCE hInst, const Parameters& parameters) :
            GuiGlue(parameters),
            hInst(hInst)
        {
            instance = this;

            WNDCLASSEX wClass;
            ZeroMemory(&wClass, sizeof(WNDCLASSEX));
            wClass.cbClsExtra = 0;
            wClass.cbSize = sizeof(WNDCLASSEX);
            wClass.cbWndExtra = 0;
            wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
            wClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wClass.hIcon = nullptr;
            wClass.hIconSm = nullptr;
            wClass.hInstance = hInst;
            wClass.lpfnWndProc = (WNDPROC)&WinProc;
            wClass.lpszClassName = "Window Class";
            wClass.lpszMenuName = nullptr;
            wClass.style = CS_HREDRAW|CS_VREDRAW;

            assert(RegisterClassEx(&wClass), "Window class creation failed");

            SetDPIAwareness();

            handleWindow = CreateWindowEx(
                0,
                "Window Class",
                parameters.title.c_str(),
                WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                (GetSystemMetrics(SM_CXSCREEN) - parameters.width) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - parameters.height) / 2,
                parameters.width,
                parameters.height,
                nullptr,
                nullptr,
                hInst,
                0);

            assert(handleWindow, "Window creation failed");

            ShowWindow(handleWindow, SW_SHOWNORMAL);

        }
        void run() override
        {
            MSG msg;
            ZeroMemory(&msg, sizeof(MSG));

            while(GetMessage(&msg, nullptr, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        void AddGroup(
            const string& groupTitle,
            const ControlCode code,
            const int posX,
            const int posY,
            const int sizeX,
            const int sizeY) override
        {
            const auto offsetPosition = getOffsetPosition(posX, posY, sizeX, sizeY);
            auto group = CreateWindowEx(
                WS_EX_WINDOWEDGE,
                "BUTTON",
                groupTitle.c_str(),
                WS_VISIBLE | WS_CHILD|BS_GROUPBOX,
                offsetPosition.x, offsetPosition.y,
                sizeX, sizeY,
                handleProc,
                (HMENU)0,
                0,
                nullptr);
            assert(group, "failed to create group");
            SetFont(group);
            registerControl(code, group);
        }
        void AddTextArea(
            const ControlCode code,
            const int posX,
            const int posY,
            const int sizeX,
            const int sizeY) override
        {
            const auto control = CreateWindowEx(
                WS_EX_WINDOWEDGE,
                "STATIC",
                "",
                WS_VISIBLE | WS_CHILD,
                posX - sizeX / 2, posY,
                sizeX, sizeY,
                handleProc, 0, 0, nullptr);
            assert(control, "failed to create text area");
            auto style = GetWindowLongPtr(control, GWL_STYLE );
            style = ( style & ~( SS_LEFT | SS_RIGHT | SS_LEFTNOWORDWRAP ) ) | SS_CENTER;
            SetWindowLongPtr(control, GWL_STYLE, style);
            SetFont(control);
            registerControl(code, control);
        }
        void AddTextAreaWithGroup(
            const string& groupTitle,
            const ControlCode code,
            const int posX,
            const int posY,
            const int sizeX,
            const int sizeY) override
        {
            AddTextArea(code, posX, posY, sizeX, sizeY);
            AddGroup(groupTitle, -1, posX, posY, sizeX, sizeY);
        }
        void AddButton(
            const string& title,
            const ControlCode code,
            const int posX,
            const int posY,
            const float widthScale=1.f) override
        {
            const auto offsetPosition = getOffsetPosition(posX, posY, ButtonWidth, ButtonHeight, widthScale);
            assert(!alreadyDefinedCodes.count(code), "code already defined");
            alreadyDefinedCodes.insert(code);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
            HWND button = CreateWindowEx(
                WS_EX_WINDOWEDGE,
                "BUTTON",
                title.c_str(),
                WS_VISIBLE | WS_CHILD | BS_MULTILINE,
                offsetPosition.x, offsetPosition.y,
                ButtonWidth * widthScale, ButtonHeight,
                handleProc,
                (HMENU)code,
                0,
                nullptr);
#pragma GCC diagnostic pop
            assert(button, "failed to create button");
            SetFont(button);
            registerControl(code, button);
        }
        void SetButtonEnabled(const ControlCode buttonCode, const bool enabled) override
        {
            assert(controlsMap.count(buttonCode), "buttonCode not found");
            Button_Enable(controlsMap[buttonCode], enabled);
        }
        void SetButtonText(const ControlCode buttonCode, const string& title, const int fontSize=0) override
        {
            assert(controlsMap.count(buttonCode), "buttonCode not found");
            Button_SetText(controlsMap[buttonCode], title.c_str());
            if (fontSize != 0)
            {
                SetFont(controlsMap[buttonCode], fontSize);
            }
        }
        void SetButtonPushed(const ControlCode buttonCode, const bool pushed) override
        {
            assert(controlsMap.count(buttonCode), "buttonCode not found");
            Button_SetState(controlsMap[buttonCode], pushed);
        }
        void SetButtonShown(const ControlCode buttonCode, const bool shown) override
        {
            assert(controlsMap.count(buttonCode), "buttonCode not found");
            ShowWindow(controlsMap[buttonCode], shown ? SW_SHOWNORMAL : SW_HIDE);
        }
        void RedrawAllControls() override
        {
            for (const auto& c: allControls)
            {
                InvalidateRect(c, nullptr, TRUE);
            }
        }
    private:
        typedef map<ControlCode, HWND> ControlsMap;
        static Win32Gui* instance;
        const HINSTANCE hInst;
        ControlsMap controlsMap;
        vector<HWND> allControls;
        set<ControlCode> alreadyDefinedCodes;
        HWND handleWindow;
        HWND handleProc;
        static LRESULT CALLBACK WinProc(HWND handleProc, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            assert(instance != nullptr, "nullptr");
            instance->handleProc = handleProc;
            switch(msg)
            {
                case WM_CREATE:
                {
                    instance->parameters.initCallback(instance);
                }
                break;
                case WM_COMMAND:
                {
                    instance->parameters.buttonCallback(LOWORD(wParam));
                }
                break;
                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                    return 0;
                }
                break;
            }
            return DefWindowProc(handleProc, msg, wParam, lParam);
        }
        void SetFont(const HWND handle, const int size=16)
        {
            static map<int, HGDIOBJ> fonts;
            if (!fonts.count(size))
            {
                fonts[size] = CreateFont(
                    size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                    OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                    VARIABLE_PITCH, TEXT("Arial"));
            }
            SendMessage(handle, WM_SETFONT, (WPARAM)fonts[size], MAKELPARAM(FALSE, 0));
        }
        void SetDPIAwareness()
        {
            HINSTANCE dllH = LoadLibrary("Shcore.dll");
            if (!assertSoft(dllH, "load library shcore"))
            {
                return;
            }
            typedef enum PROCESS_DPI_AWARENESS {
                PROCESS_DPI_UNAWARE = 0,
                PROCESS_SYSTEM_DPI_AWARE = 1,
                PROCESS_PER_MONITOR_DPI_AWARE = 2
            } PROCESS_DPI_AWARENESS;
            typedef HRESULT ( * SetProcessDpiAwarenessFunc ) ( PROCESS_DPI_AWARENESS );
            SetProcessDpiAwarenessFunc setProcessDpiAwarenessFunc = (SetProcessDpiAwarenessFunc) GetProcAddress( dllH, "SetProcessDpiAwareness" );
            if (!assertSoft(setProcessDpiAwarenessFunc, "getProcAdress SetProcessDpiAwareness"))
            {
                return;
            }
            HRESULT ret = setProcessDpiAwarenessFunc(PROCESS_PER_MONITOR_DPI_AWARE);
            if (!assertSoft(ret == S_OK, "return of SetProcessDpiAwareness"))
            {
                return;
            }
        }
        void registerControl(const ControlCode code, const HWND control)
        {
            if (int(code) >= 0)
            {
                controlsMap[code] = control;
                controlsCodes.emplace_back(code);
            }
            allControls.emplace_back(control);
        }
        static Position getOffsetPosition(
            const int posX, const int posY,
            const int sizeX, const int sizeY,
            const float widthScale=1.f)
        {
            return
            {
                posX - int(sizeX * widthScale / 2),
                posY - sizeY / 2,
            };
        }
};
Win32Gui* Win32Gui::instance = nullptr;
