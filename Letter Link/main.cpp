#include "letterlink.h"
#include <wx/panel.h>

class LLApp : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(LLApp);

class LLFrame : public wxFrame
{
public:
    LLFrame();

private:
    void OnNewGame(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnSubmit(wxCommandEvent& event);
    void OnShowChain(wxCommandEvent& event);
    void UpdateLayout();
    void UpdateLayout(int minWrite);
    void SetInteractable(bool interactable);
    void OnResize(wxSizeEvent& event);
    void UpdateColorBoxes(const vector<int>& guessResponse);

    void OnDifficulty(wxCommandEvent& event);
    void OnSetDifficulty0(wxCommandEvent& event);
    void OnSetDifficulty1(wxCommandEvent& event);
    void OnSetDifficulty2(wxCommandEvent& event);
    void OnSetDifficulty3(wxCommandEvent& event);
    void CloseDifficultyDialog();

    // Additional private members for difficulty buttons
    wxButton* difficulty0Button;
    wxButton* difficulty1Button;
    wxButton* difficulty2Button;
    wxButton* difficulty3Button;

    vector<wxTextCtrl*> writableTextBoxes;
    wxButton* newGameButton;  // New Game button
    wxButton* aboutButton;    // About button
    wxButton* difficultyButton; // Difficulty button
    wxButton* optionsButton; // Additional button 1
    wxButton* quitButton; // Additional button 2
    vector<wxPanel*> colorBoxes;

    int xMargin, yMargin, topMargin;
    int MAX_WRITEABLE = 8, MIN_WRITEABLE = 2;

    wxTextCtrl* startword;
    wxTextCtrl* endword;
    wxButton* submitButton;
    LetterLink LLGame;

};

enum
{
    ID_NewGame = 1,
    ID_Submit,
    ID_ShowChain,
    ID_About,
    ID_Difficulty, // Existing ID for difficulty
    ID_Difficulty0, // New ID for difficulty level 0
    ID_Difficulty1, // New ID for difficulty level 1
    ID_Difficulty2, // New ID for difficulty level 2
    ID_Difficulty3, // New ID for difficulty level 3
    ID_Options,
    ID_Quit
};

bool LLApp::OnInit()
{
    LLFrame* frame = new LLFrame();
    frame->Show(true);
    return true;
}

LLFrame::LLFrame()
    : wxFrame(nullptr, wxID_ANY, "Letter Link")
{
    int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    SetSize(wxSize(screenWidth / 2.25, screenHeight));

    xMargin = screenWidth / 16;
    yMargin = screenHeight / 64;
    topMargin = screenHeight / 32;

    // Create UI elements
    startword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY | wxTE_CENTER);
    startword->SetMaxLength(5);

    for (int i = 0; i < MIN_WRITEABLE; ++i) {
        wxPanel* colorBox = new wxPanel(this, wxID_ANY);
        colorBox->SetBackgroundColour(*wxWHITE);
        colorBoxes.push_back(colorBox);

        wxTextCtrl* textBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_CENTER);
        textBox->SetMaxLength(5);
        writableTextBoxes.push_back(textBox);
    }



    endword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY | wxTE_CENTER);
    endword->SetMaxLength(5);


    // New buttons
    newGameButton = new wxButton(this, ID_NewGame, "New Game");
    aboutButton = new wxButton(this, ID_About, "About");
    difficultyButton = new wxButton(this, ID_Difficulty, "Difficulty");
    optionsButton = new wxButton(this, ID_Options, "Options");
    quitButton = new wxButton(this, ID_Quit, "Quit");
    submitButton = new wxButton(this, ID_Submit, "Submit");

    // Bind buttons to event handlers
    Bind(wxEVT_BUTTON, &LLFrame::OnSubmit, this, ID_Submit);
    Bind(wxEVT_BUTTON, &LLFrame::OnNewGame, this, ID_NewGame);
    Bind(wxEVT_BUTTON, &LLFrame::OnAbout, this, ID_About);
    Bind(wxEVT_BUTTON, &LLFrame::OnDifficulty, this, ID_Difficulty);
    Bind(wxEVT_BUTTON, &LLFrame::OnOptions, this, ID_Options);
    Bind(wxEVT_BUTTON, &LLFrame::OnQuit, this, ID_Quit);

    SetInteractable(false);

    Bind(wxEVT_SIZE, &LLFrame::OnResize, this);

    UpdateLayout();
}

void LLFrame::OnNewGame(wxCommandEvent& event)
{
    LLGame.newChain();
    vector<string> wordset = LLGame.getWordset();

    startword->SetValue(wordset.size() > 0 ? wordset[0] : "");
    endword->SetValue(wordset.size() > 1 ? wordset[1] : "");

    SetInteractable(true);
}

void LLFrame::OnAbout(wxCommandEvent& event)
{
    wxString aboutText = wxT(
        "Letter Link\n\n"
        "A word-chain forming game!\n\n"
        "Description:\n"
        "Letter Link is a game where you form chains of words by switching a single letter each time.\n"
        "At the start of each game, you are given two words, and you have to find the accompanying word chain.\n\n"
        "For example, if the starting wordset is (lured, roped), then the chain would look like this:\n"
        "- lured\n"
        "- cured\n"
        "- curer\n"
        "- corer\n"
        "- cored\n"
        "- coped\n"
        "- roped\n\n"
        "This game is based off the 'word chains' concept as seen in CodeParade's Exploring Word Chain videos.\n"
    );

    wxMessageBox(aboutText, "About Letter Link", wxOK | wxICON_INFORMATION);
}


void LLFrame::OnDifficulty(wxCommandEvent& event)
{
    wxDialog* difficultyDialog = new wxDialog(this, wxID_ANY, "Select Difficulty", wxDefaultPosition, wxSize(xMargin * 8, yMargin * 32));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Create buttons for each difficulty level
    difficulty0Button = new wxButton(difficultyDialog, ID_Difficulty0, "Easy");
    difficulty1Button = new wxButton(difficultyDialog, ID_Difficulty1, "Normal");
    difficulty2Button = new wxButton(difficultyDialog, ID_Difficulty2, "Hard");
    difficulty3Button = new wxButton(difficultyDialog, ID_Difficulty3, "Impossible");

    // Bind buttons to their event handlers
    difficulty0Button->Bind(wxEVT_BUTTON, &LLFrame::OnSetDifficulty0, this);
    difficulty1Button->Bind(wxEVT_BUTTON, &LLFrame::OnSetDifficulty1, this);
    difficulty2Button->Bind(wxEVT_BUTTON, &LLFrame::OnSetDifficulty2, this);
    difficulty3Button->Bind(wxEVT_BUTTON, &LLFrame::OnSetDifficulty3, this);

    // Add buttons to the sizer
    sizer->Add(difficulty0Button, 0, wxALL | wxCENTER, 10);
    sizer->Add(difficulty1Button, 0, wxALL | wxCENTER, 10);
    sizer->Add(difficulty2Button, 0, wxALL | wxCENTER, 10);
    sizer->Add(difficulty3Button, 0, wxALL | wxCENTER, 10);

    // Set the sizer and show the dialog
    difficultyDialog->SetSizer(sizer);
    difficultyDialog->ShowModal();
}

void LLFrame::OnSetDifficulty0(wxCommandEvent& event)
{
    LLGame.setDifficulty(0);
    MIN_WRITEABLE = LLGame.getMinWrite();
    CloseDifficultyDialog();
    UpdateLayout(MIN_WRITEABLE);
    OnNewGame(event);
}

void LLFrame::OnSetDifficulty1(wxCommandEvent& event)
{
    LLGame.setDifficulty(1);
    MIN_WRITEABLE = LLGame.getMinWrite();
    CloseDifficultyDialog();
    UpdateLayout(MIN_WRITEABLE);
    OnNewGame(event);
}

void LLFrame::OnSetDifficulty2(wxCommandEvent& event)
{
    LLGame.setDifficulty(2);
    MIN_WRITEABLE = LLGame.getMinWrite();
    CloseDifficultyDialog();
    UpdateLayout(MIN_WRITEABLE);
    OnNewGame(event);
}

void LLFrame::OnSetDifficulty3(wxCommandEvent& event)
{
    LLGame.setDifficulty(3);
    MIN_WRITEABLE = LLGame.getMinWrite();
    CloseDifficultyDialog();
    UpdateLayout(MIN_WRITEABLE);
    OnNewGame(event);
}

void LLFrame::CloseDifficultyDialog()
{
    wxWindow* parent = wxDynamicCast(wxWindow::FindWindowById(wxID_ANY, this), wxDialog);
    if (parent) {
        parent->Destroy();
    }
}


void LLFrame::OnOptions(wxCommandEvent& event)
{
    wxDialog* optionsDialog = new wxDialog(this, wxID_ANY, "Options", wxDefaultPosition, wxSize(xMargin*8, yMargin*32));

    // Create sizer for vertical layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Create the "cheat" button
    wxButton* cheatButton = new wxButton(optionsDialog, wxID_ANY, "cheat");
    cheatButton->Bind(wxEVT_BUTTON, &LLFrame::OnShowChain, this);
    sizer->Add(cheatButton, 0, wxALL | wxCENTER, 10);

    // Create the "Back" button
    wxButton* backButton = new wxButton(optionsDialog, wxID_ANY, "Back");
    backButton->Bind(wxEVT_BUTTON, [optionsDialog](wxCommandEvent&) {
        optionsDialog->Destroy();
        });
    sizer->Add(backButton, 0, wxALL | wxCENTER, 10);

    // Set the sizer for the dialog
    optionsDialog->SetSizer(sizer);

    // Show the dialog window modally
    optionsDialog->ShowModal();
}


void LLFrame::SetInteractable(bool interactable)
{
    startword->Enable(interactable);
    endword->Enable(interactable);

    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        textBox->Enable(interactable);
    }

    submitButton->Enable(interactable);
}

void LLFrame::OnSubmit(wxCommandEvent& event)
{
    LLGame.guess.clear();

    LLGame.guess.push_back(startword->GetValue().ToStdString());

    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        LLGame.guess.push_back(textBox->GetValue().ToStdString());
    }

    LLGame.guess.push_back(endword->GetValue().ToStdString());

    vector<int> guessResponse;

    for (int i = 0; i < LLGame.guess.size() - 1; i++) {
        guessResponse.push_back(0);
    }

    bool isValidChain = LLGame.evalChain(LLGame.guess, guessResponse);

    UpdateColorBoxes(guessResponse);

    if (isValidChain) {
        wxMessageBox("You made a valid chain!", "Chain Validation", wxOK | wxICON_INFORMATION, this);
    }
    else {
        wxMessageBox("Your chain is wrong!", "Chain Validation", wxOK | wxICON_ERROR, this);
    }
}

void LLFrame::OnShowChain(wxCommandEvent& event)
{
    string chainText;
    for (const string& word : LLGame.autochain)
    {
        chainText += word + "\n";
    }

    wxMessageBox(chainText, "Auto-generated Chain", wxOK | wxICON_INFORMATION, this);
}

void LLFrame::UpdateColorBoxes(const vector<int>& guessResponse)
{
    for (size_t i = 0; i < guessResponse.size() - 1; ++i) {
        wxColour color;
        switch (guessResponse[i]) {
        case 0: color = *wxGREEN; break;
        case 1: color = *wxRED; break;
        case 2: color = *wxYELLOW; break;
        default: color = *wxWHITE; break;
        }
        colorBoxes[i]->SetBackgroundColour(color);
        colorBoxes[i]->Refresh();
    }
}

void LLFrame::UpdateLayout() {
    UpdateLayout(0);
}

void LLFrame::UpdateLayout(int minWrite)
{
    int numWritableBoxes = writableTextBoxes.size();
    if (minWrite) {


        if (numWritableBoxes > minWrite) {
            for (int i = 0; i < numWritableBoxes;i++) {
                wxTextCtrl* lastTextBox = writableTextBoxes.back();
                writableTextBoxes.pop_back();
                lastTextBox->Destroy();

                wxPanel* lastColorBox = colorBoxes.back();
                colorBoxes.pop_back();
                lastColorBox->Destroy();
            }
        }

        for (int i = 0; i < minWrite; i++) {
            wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_CENTER);
            newTextBox->SetMaxLength(5);
            writableTextBoxes.push_back(newTextBox);

            wxPanel* newColorBox = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(0, 0));
            newColorBox->SetBackgroundColour(*wxWHITE);
            colorBoxes.push_back(newColorBox);
        }
        numWritableBoxes = minWrite;
    }

    int windowWidth, windowHeight;
    GetClientSize(&windowWidth, &windowHeight);

    xMargin = windowWidth / 16;
    yMargin = windowHeight / 64;
    topMargin = windowHeight / 32;

    int textBoxWidth = (windowWidth / 2) - (2 * xMargin);
    int medBoxWidth = textBoxWidth / 1.25;
    int smallBoxWidth = textBoxWidth / 4;
    int buttonHeight = windowHeight / 14;
    int bigFontSize = buttonHeight / 2;
    int smallFontSize = buttonHeight / 2.25;

    wxFont bigFont(bigFontSize, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont smallFont(smallFontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    startword->SetSize(wxSize(textBoxWidth, buttonHeight));
    startword->SetPosition(wxPoint(xMargin + smallBoxWidth, topMargin));
    startword->SetFont(bigFont);

    endword->SetSize(wxSize(textBoxWidth, buttonHeight));
    endword->SetPosition(wxPoint(xMargin + smallBoxWidth, topMargin + (numWritableBoxes + 1) * (buttonHeight + yMargin)));
    endword->SetFont(bigFont);

    for (int i = 0; i < numWritableBoxes; ++i)
    {
        colorBoxes[i]->SetSize(wxSize(smallBoxWidth, buttonHeight));
        colorBoxes[i]->SetPosition(wxPoint(xMargin / 2, topMargin + (i + 1) * (buttonHeight + yMargin)));

        writableTextBoxes[i]->SetSize(wxSize(textBoxWidth, buttonHeight));
        writableTextBoxes[i]->SetPosition(wxPoint(xMargin + smallBoxWidth, topMargin + (i + 1) * (buttonHeight + yMargin)));
        writableTextBoxes[i]->SetFont(bigFont);
    }

    // Positioning new buttons
    newGameButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    newGameButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin));
    newGameButton->SetFont(smallFont);

    difficultyButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    difficultyButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + yMargin + buttonHeight));
    difficultyButton->SetFont(smallFont);

    aboutButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    aboutButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 2 * (yMargin + buttonHeight)));
    aboutButton->SetFont(smallFont);

    optionsButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    optionsButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 3 * (yMargin + buttonHeight)));
    optionsButton->SetFont(smallFont);

    
    submitButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    submitButton->SetFont(smallFont);
    submitButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 5 * (yMargin + buttonHeight)));


    quitButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    quitButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 7 * (yMargin + buttonHeight)));
    quitButton->SetFont(smallFont);

    Refresh();
}

void LLFrame::OnQuit(wxCommandEvent& event)
{
    Close(true);
}

void LLFrame::OnResize(wxSizeEvent& event)
{
    UpdateLayout();
    event.Skip();
}