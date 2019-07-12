#include <array>
using std::array;

class GameLogic
{
    public:
        GameLogic(const vector<GameLabels>& labels) :
            CodeHumanComputer(UniqueID::Get()),
            CodeComputerComputer(UniqueID::Get()),
            CodeHeaderPrompt(UniqueID::Get()),
            CodeTitleChoicePlayer1(UniqueID::Get()),
            CodeTitleChoicePlayer2(UniqueID::Get()),
            CodeValueChoicePlayer1(UniqueID::Get()),
            CodeValueChoicePlayer2(UniqueID::Get()),
            CodeResults(UniqueID::Get()),
            CodeStartRound(UniqueID::Get()),
            CodeNewRound(UniqueID::Get()),
            CodesGameTypes(computeCodesGameTypes(labels.size())),
            NamesGameTypes(computeNamesGameTypes(labels)),
            labels(labels),
            state(State::WaitingForStart),
            opponentsChoice(OpponentsChoice::HumanComputer),
            gameTypeCount(labels.size()),
            gameTypeIndex(0),
            choices({0, 0})
        {
            assert(gameTypeCount, "needs at least one set of labels");
            for (unsigned int i=0; i<gameTypeCount; ++i)
            {
                randomPlays.emplace_back(labels[i]);
            }
        }
        void onInitialize(GuiGlue* gui)
        {
            this->gui = gui;
            initView();
            updateView();
        }
        void onButtonPress(const GuiGlue::ControlCode code)
        {
            // change game type
            for (unsigned int newGameTypeIndex=0; newGameTypeIndex<gameTypeCount; ++newGameTypeIndex)
            {
                if (code == CodesGameTypes[newGameTypeIndex])
                {
                    assert(state == State::WaitingForStart, "invalid state");
                    gameTypeIndex = newGameTypeIndex;
                    break;
                }
            }

            // change human / computer
            if (code == CodeHumanComputer ||
                code == CodeComputerComputer)
            {
                assert(state == State::WaitingForStart, "invalid state");
                opponentsChoice =
                    code == CodeHumanComputer
                    ? OpponentsChoice::HumanComputer
                    : OpponentsChoice::ComputerComputer;
            }

            // start round
            if (code == CodeStartRound)
            {
                assert(state == State::WaitingForStart, "invalid state");
                choices[1] = randomPlays[gameTypeIndex].Generate();
                if (opponentsChoice == OpponentsChoice::HumanComputer)
                {
                    state = State::WaitingForHandChoice;
                }
                else
                {
                    state = State::ShowingResults;
                    choices[0] = randomPlays[gameTypeIndex].Generate();
                }
            }

            // choose hand play
            auto& currentLabels = labels[gameTypeIndex];
            if (currentLabels.HasControlCode(code))
            {
                assert(state == State::WaitingForHandChoice, "invalid state");
                choices[0] = currentLabels.IndexFromCode(code);
                state = State::ShowingResults;
            }

            // new round
            if (code == CodeNewRound)
            {
                state = State::WaitingForStart;
            }

            updateView();
        }

    protected:
        // types
        enum struct State
        {
            WaitingForStart,
            WaitingForHandChoice,
            ShowingResults,
        };
        enum struct OpponentsChoice
        {
            HumanComputer,
            ComputerComputer,
        };
        // constant values
        const GuiGlue::ControlCode
            CodeHumanComputer,
            CodeComputerComputer,
            CodeHeaderPrompt,
            CodeTitleChoicePlayer1,
            CodeTitleChoicePlayer2,
            CodeValueChoicePlayer1,
            CodeValueChoicePlayer2,
            CodeResults,
            CodeStartRound,
            CodeNewRound;
        const vector<GuiGlue::ControlCode> CodesGameTypes;
        const vector<string> NamesGameTypes;
        const map<State, string> prompts =
        {
            { State::WaitingForStart, "Choose settings and start a round" },
            { State::WaitingForHandChoice, "Choose your play" },
            { State::ShowingResults, "Start another round" },
        };
        const map<OpponentsChoice, array<string, 2>> opponentsNames =
        {
            { OpponentsChoice::HumanComputer, {"Human", "Computer"} },
            { OpponentsChoice::ComputerComputer, {"Computer1", "Computer2"} },
        };
        const vector<GameLabels> labels; // can not be a reference because it will be destructed
        // state
        State state;
        OpponentsChoice opponentsChoice;
        size_t gameTypeCount;
        unsigned int gameTypeIndex;
        array<int, 2> choices;
        // members
        vector<RandomPlay> randomPlays;
        GuiGlue* gui = nullptr;
        // functions
        static vector<GuiGlue::ControlCode> computeCodesGameTypes(const size_t gameTypeCount)
        {
            vector<GuiGlue::ControlCode> ret;
            for (unsigned int i=0; i<gameTypeCount; ++i)
            {
                ret.emplace_back(UniqueID::Get());
            }
            return ret;
        }
        static vector<string> computeNamesGameTypes(const vector<GameLabels>& labels)
        {
            vector<string> ret;
            for (const auto& l: labels)
            {
                string s = "";
                int index=0;
                for (const auto& n: l.names)
                {
                    s += n;
                    ++index;
                    if (index % 3)
                    {
                        s += " ";
                    }
                    else
                    {
                        s += "\n";
                    }
                }
                ret.emplace_back(s);
            }
            return ret;
        }
        int computeWinner()
        {
            if (choices[0] == choices[1])
            {
                return -1;
            }
            // cf https://en.wikipedia.org/wiki/Rock%E2%80%93paper%E2%80%93scissors

            // algo 1
            // const auto& currentLabels = labels[gameTypeIndex];
            // static auto moduloPositive = [] (const int a, const int b)
            // {
            //     auto ret = a % b;
            //     return (ret < 0) ? (ret + b) : ret;
            // };
            // return 1 - (moduloPositive(choices[0] - choices[1], currentLabels.count) % 2);

            // algo 2
            const auto c0lessThanC1 = choices[0] < choices[1];
            return
                (choices[0] % 2) != (choices[1] % 2)
                ? c0lessThanC1
                : !c0lessThanC1;
        }
        void initView()
        {
            // add prompt location
            const auto yPrompt = 80;
            gui->AddTextAreaWithGroup(
                "Prompt",
                CodeHeaderPrompt,
                gui->Width / 2,
                yPrompt,
                gui->Width * 4 / 5,
                gui->Height / 10);

            // add settings location
            const auto ySettings = yPrompt + 150;
            const auto heightSettings = gui->ButtonHeight * 3;
            gui->AddGroup(
                "Settings",
                -1,
                gui->Width * 1 / 2,
                ySettings,
                gui->Width * 4 / 5,
                heightSettings);
            const auto yOpponents = ySettings - heightSettings / 5;
            gui->AddButton(
                "Human / Computer",
                CodeHumanComputer,
                gui->Width * 1 / 3,
                yOpponents);
            gui->AddButton(
                "Computer / Computer",
                CodeComputerComputer,
                gui->Width * 2 / 3,
                yOpponents);
            const auto yMode = ySettings + heightSettings / 5;
            for (unsigned int gameTypeIndex=0; gameTypeIndex<CodesGameTypes.size(); ++gameTypeIndex)
            {
                gui->AddButton(
                    NamesGameTypes[gameTypeIndex],
                    CodesGameTypes[gameTypeIndex],
                    gui->Width * (gameTypeIndex + 1) / (CodesGameTypes.size() + 1),
                    yMode);
            }

            // add button new game
            const auto yStartRound = ySettings + 150;
            gui->AddButton(
                "Start round",
                CodeStartRound,
                gui->Width / 2,
                yStartRound,
                2.5f);

            // add hand choice buttons
            const auto yHandChoice = yStartRound + 150;
            const auto heightHandChoice = gui->ButtonHeight * 3;
            gui->AddGroup(
                "Your choice",
                -1,
                gui->Width * 1 / 2,
                yHandChoice,
                gui->Width * 4 / 5,
                heightHandChoice);
            const auto itemsPerLine = 3;
            const auto widthScale = 0.8f;
            for (const auto& currentLabels: labels)
            {
                for (const auto& item: currentLabels.indices)
                {
                    const auto& code = item.first;
                    const auto& i = item.second;
                    const auto row = i / itemsPerLine;
                    const auto column = i % itemsPerLine;
                    gui->AddButton(
                        currentLabels.names[i],
                        code,
                        gui->Width * (column + 1) / (itemsPerLine + 1)
                        + (row ? (gui->ButtonWidth * widthScale / 2) : 0),
                        yHandChoice + (gui->ButtonHeight * 1.1f * (row - 0.5f)),
                        widthScale);
                }
            }

            // add current play location
            const auto yCurrentPlay = yHandChoice + 150;
            GuiGlue::Position
                position0 = {gui->Width * 1 / 4, yCurrentPlay},
                position1 = {gui->Width * 3 / 4, yCurrentPlay},
                sizeCurrentPlay = {gui->Width * 1 / 5, gui->ButtonHeight * 3 / 2};
            gui->AddTextArea(
                CodeValueChoicePlayer1,
                position0.x,
                position0.y,
                sizeCurrentPlay.x,
                sizeCurrentPlay.y);
            gui->AddTextArea(
                CodeValueChoicePlayer2,
                position1.x,
                position1.y,
                sizeCurrentPlay.x,
                sizeCurrentPlay.y);
            gui->AddGroup(
                "Player 1",
                CodeTitleChoicePlayer1,
                position0.x,
                position0.y,
                sizeCurrentPlay.x,
                sizeCurrentPlay.y);
            gui->AddGroup(
                "Player 2",
                CodeTitleChoicePlayer2,
                position1.x,
                position1.y,
                sizeCurrentPlay.x,
                sizeCurrentPlay.y);
            gui->AddTextArea(
                CodeResults,
                gui->Width / 2,
                yCurrentPlay,
                gui->Width / 4,
                sizeCurrentPlay.y);

            // add button new round
            const auto yNewRound = yCurrentPlay + 90;
            gui->AddButton(
                "New round",
                CodeNewRound,
                gui->Width / 2,
                yNewRound,
                2.5f);
        }
        virtual void updateView()
        {
            // helper lambdas
            static auto isCodeHandChoice = [&] (const GuiGlue::ControlCode code)
            {
                for (const auto& l: labels)
                {
                    if (l.HasControlCode(code)) { return true; }
                }
                return false;
            };
            static auto isCodeGameType = [&] (const GuiGlue::ControlCode code)
            {
                for (const auto& codeGameType: CodesGameTypes)
                {
                    if (code == codeGameType) { return true; }
                }
                return false;
            };

            // disable buttons not relevant in current state
            for (const auto& buttonCode: gui->ControlsCodes)
            {
                if (buttonCode == CodeHeaderPrompt) { continue; }

                auto enabled = true;
                if (isCodeHandChoice(buttonCode))
                {
                    enabled =
                        state == State::WaitingForHandChoice &&
                        opponentsChoice == OpponentsChoice::HumanComputer;
                }
                else if (buttonCode == CodeHumanComputer ||
                         buttonCode == CodeComputerComputer ||
                         isCodeGameType(buttonCode) ||
                         buttonCode == CodeStartRound)
                {
                    enabled = (state == State::WaitingForStart);
                }
                else if (buttonCode == CodeNewRound)
                {
                    enabled = (state != State::WaitingForStart);
                }

                gui->SetButtonEnabled(buttonCode, enabled);
            }

            // hide buttons not relevant in current gameType
            for (unsigned int labelIndex=0; labelIndex<gameTypeCount; ++labelIndex)
            {
                for (const auto& code: labels[labelIndex].indices)
                {
                    gui->SetButtonShown(code.first, labelIndex == gameTypeIndex);
                }
            }

            // set prompt
            gui->SetButtonText(CodeHeaderPrompt, prompts.at(state), 22);

            // push enabled radio buttons
            for (const auto& opponentsCode : { CodeHumanComputer, CodeComputerComputer })
            {
                gui->SetButtonPushed(
                    opponentsCode,
                    (opponentsChoice == OpponentsChoice::HumanComputer) ==
                    (opponentsCode == CodeHumanComputer));
            }
            for (unsigned int labelIndex=0; labelIndex<gameTypeCount; ++labelIndex)
            {
                gui->SetButtonPushed(
                    CodesGameTypes[labelIndex],
                    labelIndex == gameTypeIndex);
            }

            // set opponent names
            const auto& opponentsCurrentNames = opponentsNames.at(opponentsChoice);
            gui->SetButtonText(CodeTitleChoicePlayer1, opponentsCurrentNames[0]);
            gui->SetButtonText(CodeTitleChoicePlayer2, opponentsCurrentNames[1]);

            // set play choices and results
            const auto& currentLabels = labels[gameTypeIndex];
            if (state == State::ShowingResults)
            {
                gui->SetButtonText(CodeValueChoicePlayer1, currentLabels.names[choices[0]]);
                gui->SetButtonText(CodeValueChoicePlayer2, currentLabels.names[choices[1]]);
                const auto winnerIndex = computeWinner();
                const auto winnerLabel =
                    (winnerIndex < 0)
                    ? string("Tie")
                    : ("Winner: " + opponentsCurrentNames[winnerIndex]);
                gui->SetButtonText(CodeResults, winnerLabel);
            }
            else
            {
                gui->SetButtonText(CodeValueChoicePlayer1, "");
                gui->SetButtonText(CodeValueChoicePlayer2, "");
                gui->SetButtonText(CodeResults, "");
            }

            // update view
            gui->RedrawAllControls();
        }
};
