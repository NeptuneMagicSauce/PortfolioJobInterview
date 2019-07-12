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

class GameLogicTester: public GameLogic
{
    public:
        GameLogicTester() :
            GameLogic({
                GameLabels({"Rock", "Paper", "Scissors"}),
                GameLabels({"Rock", "Paper", "Scissors", "Spock", "Lizard"}),
            })
        { }

        bool testInitialState() const
        {
            return state == State::WaitingForStart;
        }
        bool testChangeOpponents()
        {
            onButtonPress(CodeComputerComputer);
            if (opponentsChoice != OpponentsChoice::ComputerComputer)
            {
                return false;
            }
            onButtonPress(CodeHumanComputer);
            if (opponentsChoice != OpponentsChoice::HumanComputer)
            {
                return false;
            }
            return true;
        }
        bool testChangeType()
        {
            for (unsigned int typeIndex=0; typeIndex<gameTypeCount; ++typeIndex)
            {
                onButtonPress(CodesGameTypes[typeIndex]);
                if (gameTypeIndex != typeIndex)
                {
                    return false;
                }
            }
            return true;
        }
        bool testCpuCpuPlay()
        {
            if (state != State::WaitingForStart)
            {
                return false;
            }
            onButtonPress(CodeComputerComputer);
            onButtonPress(CodeStartRound);
            return state == State::ShowingResults;
        }
        bool testHumanCpuPlay()
        {
            if (state != State::WaitingForStart)
            {
                return false;
            }
            onButtonPress(CodeHumanComputer);
            onButtonPress(CodeStartRound);
            if (state != State::WaitingForHandChoice)
            {
                return false;
            }
            for (const auto& l: labels[gameTypeIndex].indices)
            {
                onButtonPress(l.first);
                break;
            }
            return state == State::ShowingResults;
        }
        bool testNewRound()
        {
            testCpuCpuPlay();
            onButtonPress(CodeNewRound);
            return state == State::WaitingForStart;
        }
        bool testComputeWinner()
        {
            auto indexFromName = [&] (const string& name)
            {
                const auto& l = labels[gameTypeIndex];
                for (unsigned int index=0; index<l.count; ++index)
                {
                    if (l.names[index] == name) { return index; }
                }
                assert(false, "");
                return (unsigned int)-1;
            };
            auto runOneTest = [&] (const string& a, const string& b)
            {
                choices[0] = indexFromName(a);
                choices[1] = indexFromName(b);
                const auto winnerIndex = computeWinner();
                if (winnerIndex == -1) { return string("Tie"); }
                else { return winnerIndex ? b : a; }
            };
            {
                gameTypeIndex = 0; // rock paper scissors
                assert(runOneTest("Rock", "Rock") == "Tie", "");
                assert(runOneTest("Rock", "Paper") == "Paper", "");
                assert(runOneTest("Rock", "Scissors") == "Rock", "");
                assert(runOneTest("Paper", "Rock") == "Paper", "");
                assert(runOneTest("Paper", "Paper") == "Tie", "");
                assert(runOneTest("Paper", "Scissors") == "Scissors", "");
                assert(runOneTest("Scissors", "Rock") == "Rock", "");
                assert(runOneTest("Scissors", "Paper") == "Scissors", "");
                assert(runOneTest("Scissors", "Scissors") == "Tie", "");

                gameTypeIndex = 1; // rock paper scissors spock lizard
                assert(runOneTest("Rock", "Rock") == "Tie", "");
                assert(runOneTest("Rock", "Paper") == "Paper", "");
                assert(runOneTest("Rock", "Scissors") == "Rock", "");
                assert(runOneTest("Rock", "Spock") == "Spock", "");
                assert(runOneTest("Rock", "Lizard") == "Rock", "");
                assert(runOneTest("Paper", "Rock") == "Paper", "");
                assert(runOneTest("Paper", "Paper") == "Tie", "");
                assert(runOneTest("Paper", "Scissors") == "Scissors", "");
                assert(runOneTest("Paper", "Spock") == "Paper", "");
                assert(runOneTest("Paper", "Lizard") == "Lizard", "");
                assert(runOneTest("Scissors", "Rock") == "Rock", "");
                assert(runOneTest("Scissors", "Paper") == "Scissors", "");
                assert(runOneTest("Scissors", "Scissors") == "Tie", "");
                assert(runOneTest("Scissors", "Spock") == "Spock", "");
                assert(runOneTest("Scissors", "Lizard") == "Scissors", "");
                assert(runOneTest("Spock", "Rock") == "Spock", "");
                assert(runOneTest("Spock", "Paper") == "Paper", "");
                assert(runOneTest("Spock", "Scissors") == "Spock", "");
                assert(runOneTest("Spock", "Spock") == "Tie", "");
                assert(runOneTest("Spock", "Lizard") == "Lizard", "");
                assert(runOneTest("Lizard", "Rock") == "Rock", "");
                assert(runOneTest("Lizard", "Paper") == "Lizard", "");
                assert(runOneTest("Lizard", "Scissors") == "Scissors", "");
                assert(runOneTest("Lizard", "Spock") == "Lizard", "");
                assert(runOneTest("Lizard", "Lizard") == "Tie", "");
            }
            return true;
        }
    private:
        virtual void updateView() override
        {
            // do nothing because no GUI is attached in test mode
        }
};

int main(int argc, char** argv)
{
    // testing assertSoft message
    const string testAssertSoft = "RockPaperScissors";
    std::cout << "test assertSoft(false, " << testAssertSoft << ")" << std::endl;
    const auto ret = assertSoft(false, testAssertSoft);
    std::cout << std::endl;

    // testing assertSoft result
    if (ret == true)
    {
        std::cerr << "test assertSoft result failed" << std::endl;
        std::terminate();
    }
    std::cout << "test assertSoft result success" << std::endl << std::endl;

    const string testAssert = "testing assert(true)";
    assert(true, testAssert);
    std::cout << "test assert(true) success" << std::endl << std::endl;

    {
        std::cout << "test WinGui constructor";
        Win32Gui gui(0,
        {
            "Rock Paper Scissors",
            100,
            100,
            [&] (GuiGlue* gui) { },
            [&] (const int code) { },
        });
        std::cout << " success" << std::endl << std::endl;
    }
    std::cout << "test WinGui destructor success" << std::endl << std::endl;

    std::cout << "test UniqueID::Get() twice ";
    const auto id0 = UniqueID::Get();
    const auto id1 = UniqueID::Get();
    std::cout << id0 << " " << id1;
    std::cout << " success" << std::endl << std::endl;

    {
        std::cout << "test GameLabels constructor";
        GameLabels labels({"l0", "l1", "l2"});
        std::cout << " success" << std::endl << std::endl;

        std::cout << "test GameLabels::HasControlCode()";
        for (const auto& n: labels.indices)
        {
            assert(labels.HasControlCode(n.first), "");
        }
        std::cout << " success" << std::endl << std::endl;

        std::cout << "test GameLabels::IndexFromCode()";
        for (const auto& n: labels.indices)
        {
            labels.IndexFromCode(n.first);
        }
        std::cout << " success" << std::endl << std::endl;
    }
    std::cout << "test GameLabels destructor success" << std::endl << std::endl;

    {
        std::cout << "test RandomPlay constructor";
        RandomPlay r(GameLabels({"l0", "l1", "l2"}));
        std::cout << " success" << std::endl << std::endl;

        std::cout << "test RandomPlay::Generate()";
        r.Generate();
        std::cout << " success" << std::endl << std::endl;

    }
    std::cout << "test RandomPlay destructor success" << std::endl << std::endl;

    {
        std::cout << "test GameLogic constructor";
        GameLogic(
            {
                GameLabels({"l0", "l1", "l2"}),
                GameLabels({"l0", "l1", "l2"}),
            });
        std::cout << " success" << std::endl << std::endl;
    }
    std::cout << "test GameLogic destructor success" << std::endl << std::endl;

    {
        GameLogicTester logic;
        std::cout << "test GameLogic initial state";
        assert(logic.testInitialState(), "");
        std::cout << " success" << std::endl << std::endl;

        std::cout << "test GameLogic Setting Opponents";
        assert(logic.testChangeOpponents(), "");
        std::cout << " success" << std::endl << std::endl;

        std::cout << "test GameLogic Setting Type";
        assert(logic.testChangeType(), "");
        std::cout << " success" << std::endl << std::endl;
    }
    {
        GameLogicTester logic;
        std::cout << "test GameLogic Computer-Computer round";
        assert(logic.testCpuCpuPlay(), "");
        std::cout << " success" << std::endl << std::endl;
    }
    {
        GameLogicTester logic;
        std::cout << "test GameLogic Human-Computer round";
        assert(logic.testHumanCpuPlay(), "");
        std::cout << " success" << std::endl << std::endl;
    }
    {
        GameLogicTester logic;
        std::cout << "test GameLogic NewRound";
        assert(logic.testNewRound(), "");
        std::cout << " success" << std::endl << std::endl;
    }
    {
        GameLogicTester logic;
        std::cout << "test GameLogic computeWinner()";
        assert(logic.testComputeWinner(), "");
        std::cout << " success" << std::endl << std::endl;
    }

    return 0;
}
