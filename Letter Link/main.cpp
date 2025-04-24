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
    void UpdateColorBoxes(const vector<int>& evalBefore, const vector<int>& evalAfter);
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

    vector<wxPanel*> colorBoxBefore;
    vector<wxPanel*> colorBoxAfter;
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
    : wxFrame(nullptr, wxID_ANY, "Letter Link", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)),
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

    startword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY | wxTE_CENTER);
    startword->SetMaxLength(5); 
    for (int i = 0; i < MIN_WRITEABLE; ++i) {
        wxPanel* beforeBox = new wxPanel(this, wxID_ANY);
        beforeBox->SetBackgroundColour(*wxWHITE);
        colorBoxBefore.push_back(beforeBox);

        wxPanel* afterBox = new wxPanel(this, wxID_ANY);
        afterBox->SetBackgroundColour(*wxWHITE);
        colorBoxAfter.push_back(afterBox);

        wxTextCtrl* textBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_CENTER);
        textBox->SetMaxLength(5);
        writableTextBoxes.push_back(textBox);

        wxButton* newMinusButton = new wxButton(this, ID_RemoveTextBox, "-", wxPoint(0, 0), wxSize(0, 0));
        Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, newMinusButton->GetId());
        minusButtons.push_back(newMinusButton);
    }

    endword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY | wxTE_CENTER);
    endword->SetMaxLength(5);


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
    wxDialog* AboutDialog = new wxDialog(this, wxID_ANY, "About Letter Link", wxDefaultPosition, wxSize(windowWidth / 1.5, windowHeight / 1.5));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Define fonts for different sections
    wxFont headerFont = medFont;
    wxFont bodyFont = wxFont(wxFontInfo(windowWidth / 64).Family(wxFONTFAMILY_TELETYPE));
    wxString bulletPoint = wxString::Format("%c", 0x2022);

    // Title text as header
    wxStaticText* title = new wxStaticText(AboutDialog, wxID_ANY, wxT("Letter Link"));
    title->SetFont(bigFont);
    sizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    // Description section in body font
    wxStaticText* description1 = new wxStaticText(AboutDialog, wxID_ANY,
        wxT("Letter Link is a game where you form chains of words by switching a single letter each time.\n\nYou are given two words at the start of each game.\n\nYou have to find the word chain between them.\n"));
    description1->SetFont(bodyFont);
    sizer->Add(description1, 0, wxLEFT | wxRIGHT | wxTOP, 10);

    // Example section in smaller font
    wxStaticText* exampleTitle = new wxStaticText(AboutDialog, wxID_ANY, wxT("Example:\n"));
    exampleTitle->SetFont(headerFont);
    sizer->Add(exampleTitle, 0, wxALIGN_LEFT | wxTOP | wxLEFT, 10);

    wxStaticText* example = new wxStaticText(AboutDialog, wxID_ANY,
        wxT("If the starting wordset is (lured, roped), the chain would look like this:\n"
            + bulletPoint + " l u r e d\n"
            + bulletPoint + " c u r e d\n"
            + bulletPoint + " c u r e r\n"
            + bulletPoint + " c o r e r\n"
            + bulletPoint + " c o r e d\n"
            + bulletPoint + " c o p e d\n"
            + bulletPoint + " r o p e d\n"));
    example->SetFont(bodyFont);
    sizer->Add(example, 0, wxLEFT | wxRIGHT | wxTOP, 10);

    // Color information section with wxPanel for colored boxes
    wxStaticText* colorTitle = new wxStaticText(AboutDialog, wxID_ANY, wxT("The color of the box:"));
    colorTitle->SetFont(headerFont);
    sizer->Add(colorTitle, 0, wxTOP | wxLEFT, 10);

    // Create horizontal sizer for each color statement and box
    wxBoxSizer* redSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* redPanel = new wxPanel(AboutDialog, wxID_ANY, wxDefaultPosition, wxSize(20, 20));
    redPanel->SetBackgroundColour(*wxRED);
    wxStaticText* redText = new wxStaticText(AboutDialog, wxID_ANY, wxT(" - the word is not valid; it was not found in the dictionary."));
    redText->SetFont(bodyFont);
    redSizer->Add(redPanel, 0, wxRIGHT, 5);
    redSizer->Add(redText);
    sizer->Add(redSizer, 0, wxLEFT | wxRIGHT | wxTOP, 10);

    wxBoxSizer* yellowSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* yellowPanel = new wxPanel(AboutDialog, wxID_ANY, wxDefaultPosition, wxSize(20, 20));
    yellowPanel->SetBackgroundColour(*wxYELLOW);
    wxStaticText* yellowText = new wxStaticText(AboutDialog, wxID_ANY, wxT(" - the word is valid, but doesn't connect the chain."));
    yellowText->SetFont(bodyFont);
    yellowSizer->Add(yellowPanel, 0, wxRIGHT, 5);
    yellowSizer->Add(yellowText);
    sizer->Add(yellowSizer, 0, wxLEFT | wxRIGHT | wxTOP, 10);

    wxBoxSizer* greenSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* greenPanel = new wxPanel(AboutDialog, wxID_ANY, wxDefaultPosition, wxSize(20, 20));
    greenPanel->SetBackgroundColour(*wxGREEN);
    wxStaticText* greenText = new wxStaticText(AboutDialog, wxID_ANY, wxT(" - the word is valid and connects the chain."));
    greenText->SetFont(bodyFont);
    greenSizer->Add(greenPanel, 0, wxRIGHT, 5);
    greenSizer->Add(greenText);
    sizer->Add(greenSizer, 0, wxLEFT | wxRIGHT | wxTOP, 10);

    // Attribution section in smaller font
    wxStaticText* attributionTitle = new wxStaticText(AboutDialog, wxID_ANY, wxT("Attribution:"));
    attributionTitle->SetFont(headerFont);
    sizer->Add(attributionTitle, 0, wxTOP | wxLEFT, 10);

    wxStaticText* attributionText = new wxStaticText(AboutDialog, wxID_ANY,
        wxT("This game is based off the 'word chains' concept as seen in CodeParade's video \"Exploring Word Chains\".\n"));
    attributionText->SetFont(bodyFont);
    sizer->Add(attributionText, 0, wxLEFT | wxRIGHT | wxTOP, 10);


    // Final section with an OK button
    wxButton* okButton = new wxButton(AboutDialog, wxID_OK, wxT("OK"));
    sizer->Add(okButton, 0, wxALIGN_CENTER | wxALL, 10);

    // Set the sizer and display the dialog
    AboutDialog->SetSizer(sizer);
    AboutDialog->ShowModal();
    AboutDialog->Destroy();
}

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
    int insertPos = 0;

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

    wxPanel* newColorBoxBefore = new wxPanel(this, wxID_ANY);
    newColorBoxBefore->SetBackgroundColour(*wxWHITE);
    colorBoxBefore.push_back(newColorBoxBefore);

    wxPanel* newColorBoxAfter = new wxPanel(this, wxID_ANY);
    newColorBoxAfter->SetBackgroundColour(*wxWHITE);
    colorBoxAfter.push_back(newColorBoxAfter);

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
        writableTextBoxes[index]->Destroy();
        writableTextBoxes.erase(writableTextBoxes.begin() + index);

        colorBoxBefore[index]->Destroy();
        colorBoxBefore.erase(colorBoxBefore.begin() + index);

        colorBoxAfter[index]->Destroy();
        colorBoxAfter.erase(colorBoxAfter.begin() + index);

        minusButtons[index]->Destroy();
        minusButtons.erase(minusButtons.begin() + index);

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

    UpdateColorBoxes(evalBefore, evalAfter);

    if (isValidChain) {
        wxMessageBox("You Link the Letters!\n\nAmazing!", "Chain Validation", wxOK | wxICON_INFORMATION, this, wxTE_CENTER);
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

void LLFrame::UpdateColorBoxes(const vector<int>& evalBefore, const vector<int>& evalAfter) {
    int gPs = evalBefore.size();
    wxColour topColor, bottomColor;

    for (int i = 0; i < gPs; i++) {
        // Set color for evalBefore (top half)
        switch (evalBefore[i]) {
        case 1: topColor = *wxGREEN; break;
        case 2: topColor = *wxRED; break;
        case 3: topColor = *wxYELLOW; break;
        default: topColor = *wxWHITE; break;
        }

        // Set color for evalAfter (bottom half)
        switch (evalAfter[i]) {
        case 1: bottomColor = *wxGREEN; break;
        case 2: bottomColor = *wxRED; break;
        case 3: bottomColor = *wxYELLOW; break;
        default: bottomColor = *wxWHITE; break;
        }

        // Update color for before and after boxes
        colorBoxBefore[i]->SetBackgroundColour(topColor);
        colorBoxBefore[i]->Refresh();
        colorBoxAfter[i]->SetBackgroundColour(bottomColor);
        colorBoxAfter[i]->Refresh();
    }
}

void LLFrame::ResetButtons(int minWrite, int& numWritableBoxes) {
    // Remove all existing boxes and buttons
    for (int i = 0; i < numWritableBoxes; i++) {
        writableTextBoxes.back()->Destroy();
        writableTextBoxes.pop_back();

        colorBoxBefore.back()->Destroy();
        colorBoxBefore.pop_back();

        colorBoxAfter.back()->Destroy();
        colorBoxAfter.pop_back();

        minusButtons.back()->Destroy();
        minusButtons.pop_back();
    }

    // Add the minimum required boxes and buttons
    for (int i = 0; i < minWrite; i++) {
        wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_CENTER);
        newTextBox->SetMaxLength(5);
        writableTextBoxes.push_back(newTextBox);

        wxPanel* newColorBoxBefore = new wxPanel(this, wxID_ANY);
        newColorBoxBefore->SetBackgroundColour(*wxWHITE);
        colorBoxBefore.push_back(newColorBoxBefore);

        wxPanel* newColorBoxAfter = new wxPanel(this, wxID_ANY);
        newColorBoxAfter->SetBackgroundColour(*wxWHITE);
        colorBoxAfter.push_back(newColorBoxAfter);

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
        // Set sizes and positions for colorBoxBefore
        colorBoxBefore[i]->SetSize(wxSize(smallBoxWidth, boxHeight / 2));  // Adjust the height as needed
        colorBoxBefore[i]->SetPosition(wxPoint(xMargin / 2, topMargin + (i + 1) * (boxHeight + yMargin)));

        // Set sizes and positions for colorBoxAfter
        colorBoxAfter[i]->SetSize(wxSize(smallBoxWidth, boxHeight / 2));  // Adjust the height as needed
        colorBoxAfter[i]->SetPosition(wxPoint(xMargin / 2, topMargin + (i + 1) * (boxHeight + yMargin) + (boxHeight / 2)));

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
    bigFontSize = boxHeight / 3;
    medFontSize = boxHeight / 4;

    gameFont.SetPointSize(gameFontSize);
    gameFont.SetFamily(wxFONTFAMILY_TELETYPE);
    gameFont.SetStyle(wxFONTSTYLE_NORMAL);
    gameFont.SetWeight(wxFONTWEIGHT_BOLD);

    bigFont.SetPointSize(bigFontSize);
    bigFont.SetFamily(wxFONTFAMILY_TELETYPE);
    bigFont.SetStyle(wxFONTSTYLE_NORMAL);
    bigFont.SetWeight(wxFONTWEIGHT_NORMAL);

    medFont.SetPointSize(medFontSize);
    medFont.SetFamily(wxFONTFAMILY_TELETYPE);
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
