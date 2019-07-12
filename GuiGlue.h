#include <functional> // function()

class GuiGlue
{
    public:
        // types
        typedef unsigned int ControlCode;
        struct Parameters
        {
            typedef std::function<void(GuiGlue*)> InitCallback;
            typedef std::function<void(const int)> ButtonCallback;
            string title;
            int width;
            int height;
            InitCallback initCallback;
            ButtonCallback buttonCallback;
        };
        struct Position
        {
            int x;
            int y;
        };
        // constant values
        const int &Width, &Height;
        const int ButtonWidth = 150, ButtonHeight = 60;
        const vector<ControlCode>& ControlsCodes;

        GuiGlue(const Parameters& parameters) :
            Width(parameters.width),
            Height(parameters.height),
            ControlsCodes(controlsCodes),
            parameters(parameters)
        { }

        // to implement by inheritors :
        virtual void run() = 0;
        virtual void AddGroup(
            const string& groupTitle,
            const ControlCode code,
            const int posX,
            const int posY,
            const int sizeX,
            const int sizeY) = 0;
        virtual void AddTextArea(
            const ControlCode code,
            const int posX,
            const int posY,
            const int sizeX,
            const int sizeY) = 0;
        virtual void AddTextAreaWithGroup(
            const string& groupTitle,
            const ControlCode code,
            const int posX,
            const int posY,
            const int sizeX,
            const int sizeY) = 0;
        virtual void AddButton(
            const string& title,
            const ControlCode code,
            const int posX,
            const int posY,
            const float widthScale=1.f) = 0;
        virtual void SetButtonEnabled(const ControlCode buttonCode, const bool enabled) = 0;
        virtual void SetButtonText(const ControlCode buttonCode, const string& title, const int fontSize=0) = 0;
        virtual void SetButtonPushed(const ControlCode buttonCode, const bool pushed) = 0;
        virtual void SetButtonShown(const ControlCode buttonCode, const bool shown) = 0;
        virtual void RedrawAllControls() = 0;

    protected:
        const Parameters parameters;
        vector<ControlCode> controlsCodes;
};
