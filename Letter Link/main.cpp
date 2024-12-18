#include "letterlink.h"

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
    void OnNewGame();
    void OnAbout(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnSubmit(wxCommandEvent& event);
    void OnShowChain(wxCommandEvent& event);
    void DrawGame();
    void DrawGame(int minWrite);
    void SetInteractable(bool interactable);
    void OnResize(wxSizeEvent& event);
    void UpdateColorBoxes(const vector<int>& evalBefore);
    void OnAddTextBox(wxCommandEvent& event);
    void OnRemoveTextBox(wxCommandEvent& event);
    void UpdateButtonAccessibility();
    void ResetButtons(int minWrite, int& numWritableBoxes);
    void UpdateLayout();
    void OnDifficulty(wxCommandEvent& event);

   

    wxButton* plusButton;

    vector<wxTextCtrl*> writableTextBoxes;
    wxButton* newGameButton;  // New Game button
    wxButton* aboutButton;    // About button
    wxButton* difficultyButton; // Difficulty button
    wxButton* optionsButton; // Additional button 1
    wxButton* quitButton; // Additional button 2
    vector<wxPanel*> colorBoxes;
    vector<wxButton*> minusButtons;

    int xMargin, yMargin, topMargin;
    const int MAX_WRITEABLE = 10;
    int SOFT_MAX_WRITEABLE = 6, MIN_WRITEABLE = 4;
    int textBoxWidth, medBoxWidth, smallBoxWidth, boxHeight;
    int gameFontSize, bigFontSize, medFontSize;
    wxFont gameFont;
    wxFont bigFont;
    wxFont medFont;

    wxTextCtrl* startword;
    wxTextCtrl* endword;
    wxButton* submitButton;
    LetterLink LLGame;

    int windowWidth, windowHeight;
};

enum
{
    ID_NewGame = 1,
    ID_AddTextBox,
    ID_RemoveTextBox,
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


LLFrame::LLFrame()
    : wxFrame(nullptr, wxID_ANY, "Letter Link", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER),
    xMargin(0), yMargin(0), topMargin(0),
      textBoxWidth(0), medBoxWidth(0), smallBoxWidth(0), boxHeight(0),
      gameFontSize(0), bigFontSize(0)
{

    Maximize(true);

    GetClientSize(&windowWidth, &windowHeight);

    windowWidth /= 2.5;
    windowHeight += (windowHeight / 32);

    Maximize(false);

    SetSize(wxSize(windowWidth, windowHeight));

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

        wxButton* newMinusButton = new wxButton(this, ID_RemoveTextBox, "-", wxPoint(0, 0), wxSize(0, 0));
        Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, newMinusButton->GetId());
        minusButtons.push_back(newMinusButton);
    }

    endword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY | wxTE_CENTER);
    endword->SetMaxLength(5);


    // New buttons
    newGameButton = new wxButton(this, ID_NewGame, "New Game");
    aboutButton = new wxButton(this, ID_About, "About");
    difficultyButton = new wxButton(this, ID_Difficulty, "Difficulty");
    optionsButton = new wxButton(this, ID_Options, "Options");
    quitButton = new wxButton(this, ID_Quit, "QUIT");
    submitButton = new wxButton(this, ID_Submit, "Submit");
    plusButton = new wxButton(this, ID_AddTextBox, "+");
    

    // Bind buttons to event handlers
    Bind(wxEVT_BUTTON, &LLFrame::OnSubmit, this, ID_Submit);
    Bind(wxEVT_BUTTON, &LLFrame::OnNewGame, this, ID_NewGame);
    Bind(wxEVT_BUTTON, &LLFrame::OnAbout, this, ID_About);
    Bind(wxEVT_BUTTON, &LLFrame::OnDifficulty, this, ID_Difficulty);
    Bind(wxEVT_BUTTON, &LLFrame::OnOptions, this, ID_Options);
    Bind(wxEVT_BUTTON, &LLFrame::OnQuit, this, ID_Quit);
    
    Bind(wxEVT_BUTTON, &LLFrame::OnAddTextBox, this, ID_AddTextBox);

    SetInteractable(false);

    Bind(wxEVT_SIZE, &LLFrame::OnResize, this);

    LLGame.setDifficulty(1);

    UpdateLayout();
    DrawGame();
}

void LLFrame::OnNewGame()
{
    wxCommandEvent dummyEvent;
    OnNewGame(dummyEvent);
}

void LLFrame::OnNewGame(wxCommandEvent& event)
{
    MIN_WRITEABLE = LLGame.getMinWrite();
    DrawGame(MIN_WRITEABLE);
    SOFT_MAX_WRITEABLE = LLGame.getMaxChain();

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

        "The color of the box on the left tell you about the word:\n"
        "- RED - not a word\n"
        "- YELLOW - a word, doesn't connect chain\n"
        "- GREEN - valid chain word\n\n"

        "\nThis game is based off the 'word chains' concept as seen in CodeParade's Exploring Word Chain video.\n"
    );

    wxMessageBox(aboutText, "About Letter Link", wxOK | wxICON_INFORMATION);
}


// Inside the OnDifficulty function
void LLFrame::OnDifficulty(wxCommandEvent& event)
{
    wxDialog* difficultyDialog = new wxDialog(this, wxID_ANY, "Select Difficulty", wxDefaultPosition, wxSize(windowWidth / 2.25, windowHeight / 2.5));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);


    // Create buttons for each difficulty level
    wxButton* difficulty0Button = new wxButton(difficultyDialog, wxID_ANY, "Easy");
    wxButton* difficulty1Button = new wxButton(difficultyDialog, wxID_ANY, "Normal");
    wxButton* difficulty2Button = new wxButton(difficultyDialog, wxID_ANY, "Hard");
    wxButton* difficulty3Button = new wxButton(difficultyDialog, wxID_ANY, "Impossible");

    // Bind buttons to the respective difficulty level functions directly
    difficulty0Button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { LLGame.setDifficulty(0); OnNewGame(); });
    difficulty1Button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { LLGame.setDifficulty(1); OnNewGame(); });
    difficulty2Button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { LLGame.setDifficulty(2); OnNewGame(); });
    difficulty3Button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { LLGame.setDifficulty(3); OnNewGame(); });

    wxButton* backButton = new wxButton(difficultyDialog, wxID_ANY, "Back");
    backButton->Bind(wxEVT_BUTTON, [difficultyDialog](wxCommandEvent&) { difficultyDialog->Destroy(); });

    // Add buttons to the sizer
    sizer->Add(difficulty0Button, 0, wxALL | wxLEFT, 10);
    sizer->Add(difficulty1Button, 0, wxALL | wxLEFT, 10);
    sizer->Add(difficulty2Button, 0, wxALL | wxLEFT, 10);
    sizer->Add(difficulty3Button, 0, wxALL | wxLEFT, 10);
    sizer->Add(backButton, 0, wxALL | wxLEFT, 10);

    difficulty0Button->SetFont(bigFont);
    difficulty1Button->SetFont(bigFont);
    difficulty2Button->SetFont(bigFont);
    difficulty3Button->SetFont(bigFont);


    // Set the sizer and show the dialog
    difficultyDialog->SetSizer(sizer);
    difficultyDialog->ShowModal();
    difficultyDialog->Destroy();
}


void LLFrame::OnOptions(wxCommandEvent& event)
{
    wxDialog* optionsDialog = new wxDialog(this, wxID_ANY, "Options", wxDefaultPosition, wxSize(windowWidth / 4, windowHeight/4));

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
    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        textBox->Enable(interactable);
    }

    for (wxButton* minusButton : minusButtons)
    {
        minusButton->Enable(interactable && writableTextBoxes.size() > 1);
    }
    
    plusButton->Enable(interactable && writableTextBoxes.size() < MAX_WRITEABLE);
    submitButton->Enable(interactable);
}

void LLFrame::OnAddTextBox(wxCommandEvent& event)
{
    vector<string> reguess;
    int insertPos;

    if (writableTextBoxes.size() >= MAX_WRITEABLE || writableTextBoxes.size() >= SOFT_MAX_WRITEABLE) {
        return;
    }

    reguess.clear();
    for (auto it = writableTextBoxes.rbegin(); it != writableTextBoxes.rend(); ++it)
    {
        reguess.push_back((*it)->GetValue().ToStdString());
    }

    wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0,0), wxSize(0,0), wxTE_CENTER);
    newTextBox->SetMaxLength(5);
    writableTextBoxes.push_back(newTextBox);

    wxPanel* newColorBox = new wxPanel(this, wxID_ANY);
    newColorBox->SetBackgroundColour(*wxWHITE);
    colorBoxes.push_back(newColorBox);

    wxButton* newMinusButton = new wxButton(this, wxID_ANY, "-", wxPoint(0,0), wxSize(0,0));
    Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, newMinusButton->GetId());
    minusButtons.push_back(newMinusButton);

    DrawGame();
    UpdateButtonAccessibility();
}

void LLFrame::OnRemoveTextBox(wxCommandEvent& event) {
    // Get the ID of the button that triggered the event
    int buttonId = event.GetId();

    // Find the index of the button in the minusButtons vector
    auto it = std::find_if(minusButtons.begin(), minusButtons.end(),
        [buttonId](wxButton* btn) { return btn->GetId() == buttonId; });

    if (it != minusButtons.end()) {
        int index = std::distance(minusButtons.begin(), it);  // Get the index of the button

        // Remove the corresponding text box, color box, and minus button
        wxTextCtrl* textBox = writableTextBoxes[index];
        writableTextBoxes.erase(writableTextBoxes.begin() + index);
        textBox->Destroy();

        wxPanel* colorBox = colorBoxes[index];
        colorBoxes.erase(colorBoxes.begin() + index);
        colorBox->Destroy();

        wxButton* minusButton = minusButtons[index];
        minusButtons.erase(minusButtons.begin() + index);
        minusButton->Destroy();

        // Redraw the game to reflect the updated layout
        DrawGame();
        UpdateButtonAccessibility();
    }
}

void LLFrame::UpdateButtonAccessibility()
{
    for (wxButton* minusButton : minusButtons)
    {
        if (writableTextBoxes.size() > 1) {
            minusButton->Enable();
        }
        else {
            minusButton->Disable();
        }
    }
   

    if (writableTextBoxes.size() >= MAX_WRITEABLE || writableTextBoxes.size() >= SOFT_MAX_WRITEABLE) {
        plusButton->Disable();
    }
    else {
        plusButton->Enable();
    }
}

void LLFrame::OnSubmit(wxCommandEvent& event)
{
    vector<string> guess;

    guess.clear();

    guess.push_back(startword->GetValue().ToStdString());

    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        guess.push_back(textBox->GetValue().ToStdString());
    }

    guess.push_back(endword->GetValue().ToStdString());

    vector<int> evalBefore, evalAfter;

    for (int i = 0; i < guess.size() - 2; i++) {
        evalBefore.push_back(1);
        evalAfter.push_back(1);
    }

    bool isValidChain = LLGame.evalChain(guess, evalBefore, evalAfter);

    UpdateColorBoxes(evalBefore);

    if (isValidChain) {
        wxMessageBox("You Link the Letters!\n\nDeleting C:\Windows\System32....", "Chain Validation", wxOK | wxICON_INFORMATION, this, wxTE_CENTER);
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

void LLFrame::UpdateColorBoxes(const vector<int>& evalBefore)
{
    int gPs = evalBefore.size();
    for (int i = 0; i < gPs; i++) {
        wxColour color;
        switch (evalBefore[i]) {
        case 1: color = *wxGREEN; break;
        case 2: color = *wxRED; break;
        case 3: color = *wxYELLOW; break;
        default: color = *wxWHITE; break;
        }
        colorBoxes[i]->SetBackgroundColour(color);
        colorBoxes[i]->Refresh();
    }
}

void LLFrame::ResetButtons(int minWrite, int& numWritableBoxes)
{
    for (int i = 0; i < numWritableBoxes;i++) {
        wxTextCtrl* lastTextBox = writableTextBoxes.back();
        writableTextBoxes.pop_back();
        lastTextBox->Destroy();

        wxPanel* lastColorBox = colorBoxes.back();
        colorBoxes.pop_back();
        lastColorBox->Destroy();

        wxButton* lastButton = minusButtons.back();
        minusButtons.pop_back();
        lastButton->Destroy();
    }


    for (int i = 0; i < minWrite; i++) {
        wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_CENTER);
        newTextBox->SetMaxLength(5);
        writableTextBoxes.push_back(newTextBox);

        wxPanel* newColorBox = new wxPanel(this, wxID_ANY);
        newColorBox->SetBackgroundColour(*wxWHITE);
        colorBoxes.push_back(newColorBox);

        wxButton* newMinusButton = new wxButton(this, wxID_ANY, "-", wxPoint(0, 0), wxSize(0, 0));
        Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, newMinusButton->GetId());
        minusButtons.push_back(newMinusButton);
    }

    numWritableBoxes = minWrite;
}


void LLFrame::DrawGame()
{
    DrawGame(0);
}

void LLFrame::DrawGame(int minWrite)
{
    int numWritableBoxes = writableTextBoxes.size();
    if (minWrite) {
        ResetButtons(minWrite, numWritableBoxes);
    }

    startword->SetSize(wxSize(textBoxWidth, boxHeight));
    startword->SetPosition(wxPoint(xMargin + smallBoxWidth, topMargin));
    startword->SetFont(gameFont);

    endword->SetSize(wxSize(textBoxWidth, boxHeight));
    endword->SetPosition(wxPoint(xMargin + smallBoxWidth, topMargin + (numWritableBoxes + 1) * (boxHeight + yMargin)));
    endword->SetFont(gameFont);

    for (int i = 0; i < numWritableBoxes; ++i)
    {
        colorBoxes[i]->SetSize(wxSize(smallBoxWidth, boxHeight));
        colorBoxes[i]->SetPosition(wxPoint(xMargin / 2, topMargin + (i + 1) * (boxHeight + yMargin)));

        writableTextBoxes[i]->SetSize(wxSize(textBoxWidth, boxHeight));
        writableTextBoxes[i]->SetPosition(wxPoint(xMargin + smallBoxWidth, topMargin + (i + 1) * (boxHeight + yMargin)));
        writableTextBoxes[i]->SetFont(gameFont);

        minusButtons[i]->SetSize(wxSize(textBoxWidth / 4, boxHeight));
        minusButtons[i]->SetPosition(wxPoint((xMargin * 1.5) + smallBoxWidth + textBoxWidth, topMargin + (i + 1) * (boxHeight + yMargin)));
        minusButtons[i]->SetFont(bigFont);
    }

    //minusButton->SetSize(wxSize(textBoxWidth / 4, boxHeight));
    //minusButton->SetPosition(wxPoint((xMargin * 1.5) + smallBoxWidth + textBoxWidth, topMargin + (numWritableBoxes + 1) * (boxHeight + yMargin)));
    //minusButton->SetFont(bigFont);

    plusButton->SetSize(wxSize(textBoxWidth / 4, boxHeight));
    plusButton->SetPosition(wxPoint((xMargin * 1.5) + smallBoxWidth + textBoxWidth, topMargin + (numWritableBoxes + 1) * (boxHeight + yMargin)));
    plusButton->SetFont(bigFont);

    Refresh();
}

void LLFrame::UpdateLayout()
{ 
    xMargin = windowWidth / 16;
    yMargin = windowHeight / 128;
    topMargin = windowHeight / 64;

    textBoxWidth = (windowWidth / 2) - (2 * xMargin);
    medBoxWidth = textBoxWidth / 1.32;
    smallBoxWidth = textBoxWidth / 4;
    boxHeight = windowHeight / 14;
    gameFontSize = boxHeight / 1.75;
    bigFontSize = boxHeight / 2.5;
    medFontSize = boxHeight / 4;

    gameFont.SetPointSize(gameFontSize);
    gameFont.SetFamily(wxFONTFAMILY_TELETYPE);
    gameFont.SetStyle(wxFONTSTYLE_NORMAL);
    gameFont.SetWeight(wxFONTWEIGHT_BOLD);

    bigFont.SetPointSize(bigFontSize);
    bigFont.SetFamily(wxFONTFAMILY_DEFAULT);
    bigFont.SetStyle(wxFONTSTYLE_NORMAL);
    bigFont.SetWeight(wxFONTWEIGHT_NORMAL);

    medFont.SetPointSize(medFontSize);
    medFont.SetFamily(wxFONTFAMILY_DEFAULT);
    medFont.SetStyle(wxFONTSTYLE_NORMAL);
    medFont.SetWeight(wxFONTWEIGHT_NORMAL);

    // Positioning new buttons
    newGameButton->SetSize(wxSize(medBoxWidth, boxHeight));
    newGameButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin));
    newGameButton->SetFont(bigFont);

    difficultyButton->SetSize(wxSize(medBoxWidth, boxHeight));
    difficultyButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + yMargin + boxHeight));
    difficultyButton->SetFont(bigFont);

    aboutButton->SetSize(wxSize(medBoxWidth, boxHeight));
    aboutButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 2 * (yMargin + boxHeight)));
    aboutButton->SetFont(bigFont);

    optionsButton->SetSize(wxSize(medBoxWidth, boxHeight));
    optionsButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 3 * (yMargin + boxHeight)));
    optionsButton->SetFont(bigFont);

    
    submitButton->SetSize(wxSize(medBoxWidth, boxHeight));
    submitButton->SetFont(bigFont);
    submitButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, topMargin + 5 * (yMargin + boxHeight)));

    quitButton->SetSize(wxSize(medBoxWidth/1.5, boxHeight/1.5));
    quitButton->SetPosition(wxPoint(windowWidth - (smallBoxWidth*2) - xMargin / 2, windowHeight - (windowHeight/8)));
    quitButton->SetFont(medFont);

    Refresh();
}

void LLFrame::OnQuit(wxCommandEvent& event)
{
    Close(true);
}

void LLFrame::OnResize(wxSizeEvent& event)
{
    DrawGame();
    event.Skip();
}

bool LLApp::OnInit()
{
    /*
    AllocConsole();

    // Redirect standard output (stdout) to the console
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);

    // Optional: Set the console title
    SetConsoleTitle(TEXT("Console Output"));
    */

    LLFrame* frame = new LLFrame();
    frame->Show(true);
    return true;
}
