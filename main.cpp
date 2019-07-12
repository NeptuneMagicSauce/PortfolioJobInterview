#include <windows.h>
#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;
using std::size_t;

#include "ErrorHandling.h"
#include "GuiGlue.h"
#include "Win32Gui.h"
#include "UniqueID.h"
#include "GameLabels.h"
#include "RandomPlay.h"
#include "GameLogic.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
    GameLogic gameLogic(
        {
            GameLabels({ "Rock", "Paper", "Scissors" }),
            GameLabels({ "Rock", "Paper", "Scissors", "Spock", "Lizard" }),
        });

    Win32Gui(
        hInst,
        {
            "Rock Paper Scissors",
            500,
            850,
            [&] (GuiGlue* gui)
            {
                gameLogic.onInitialize(gui);
            },
            [&] (const int code)
            {
                gameLogic.onButtonPress(code);
            }
        }).run();

    return 0;
}

// BUILD instructions : 'make' command with gcc in path and windows OS
// RUN instructions : launch rock-paper-scissors.exe

/* DESIGN PATTERNS
   model-view-controller
   inheritance / polymorphism
   const-correctness
   resource-acquisition-is-initialization
   do-not-repeat-yourself
   singleton
 */
