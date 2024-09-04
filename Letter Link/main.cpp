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
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAddTextBox(wxCommandEvent& event);
    void OnRemoveTextBox(wxCommandEvent& event);
    void OnSubmit(wxCommandEvent& event);
    void OnShowChain(wxCommandEvent& event);
    void OnDifficulty(wxCommandEvent& event); // New event handler for difficulty
    void UpdateLayout();
    void SetInteractable(bool interactable);
    void OnResize(wxSizeEvent& event);
    void UpdateButtonAccessibility();
    void UpdateColorBoxes(const vector<int>& guessResponse);

    vector<wxTextCtrl*> writableTextBoxes;
    wxButton* minusButton;
    wxButton* newGameButton;  // New Game button
    wxButton* aboutButton;    // About button
    wxButton* difficultyButton; // Difficulty button
    wxButton* optionsButton; // Additional button 1
    wxButton* quitButton; // Additional button 2
    vector<wxPanel*> colorBoxes;
    int xMargin, yMargin;
    int const MAX_WRITEABLE = 8, MIN_WRITEABLE = 3;

    wxTextCtrl* startword;
    wxTextCtrl* endword;
    wxButton* plusButton;
    wxButton* submitButton;
    LetterLink LLGame;
};

enum
{
    ID_NewGame = 1,
    ID_AddTextBox,
    ID_RemoveTextBox,
    ID_Submit,
    ID_ShowChain,
    ID_About,
    ID_Difficulty, // New ID for difficulty
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
    SetSize(wxSize(screenWidth / 2, screenHeight));

    xMargin = screenWidth / 32;
    yMargin = screenHeight / 64;

    CreateStatusBar();
    SetStatusText("Welcome to Letter Link!");

    // Create UI elements
    startword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY);
    startword->SetMaxLength(5);

    for (int i = 0; i < 3; ++i) {
        wxPanel* colorBox = new wxPanel(this, wxID_ANY);
        colorBox->SetBackgroundColour(*wxWHITE);
        colorBoxes.push_back(colorBox);

        wxTextCtrl* textBox = new wxTextCtrl(this, wxID_ANY, "");
        textBox->SetMaxLength(5);
        writableTextBoxes.push_back(textBox);
    }

    minusButton = new wxButton(this, ID_RemoveTextBox, "-", wxPoint(0, 0), wxSize(0, 0));
    Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, ID_RemoveTextBox);

    endword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(0, 0), wxTE_READONLY);
    endword->SetMaxLength(5);

    plusButton = new wxButton(this, ID_AddTextBox, "+");
    submitButton = new wxButton(this, ID_Submit, "Submit");

    // New buttons
    newGameButton = new wxButton(this, ID_NewGame, "New Game");
    aboutButton = new wxButton(this, ID_About, "About");
    difficultyButton = new wxButton(this, ID_Difficulty, "Difficulty");
    optionsButton = new wxButton(this, ID_Options, "Options");
    quitButton = new wxButton(this, ID_Quit, "Quit");

    // Bind buttons to event handlers
    Bind(wxEVT_BUTTON, &LLFrame::OnAddTextBox, this, ID_AddTextBox);
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
    wxMessageBox("Explain the freaking game", "About Letter Link", wxOK | wxICON_INFORMATION);
}

void LLFrame::OnDifficulty(wxCommandEvent& event)
{
    wxMessageBox("This is a dummy Difficulty popup", "Difficulty", wxOK | wxICON_INFORMATION);
}

void LLFrame::OnOptions(wxCommandEvent& event)
{
    wxMessageBox("This is a dummy Options popup", "Options", wxOK | wxICON_INFORMATION);
}

void LLFrame::SetInteractable(bool interactable)
{
    startword->Enable(interactable);
    endword->Enable(interactable);

    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        textBox->Enable(interactable);
    }

    minusButton->Enable(interactable && writableTextBoxes.size() > MIN_WRITEABLE);
    plusButton->Enable(interactable && writableTextBoxes.size() < MAX_WRITEABLE);
    submitButton->Enable(interactable);
}

void LLFrame::OnAddTextBox(wxCommandEvent& event)
{
    if (writableTextBoxes.size() >= MAX_WRITEABLE) {
        return;
    }

    wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, yMargin + (writableTextBoxes.size() + 1) * (yMargin)), wxSize(200, yMargin));
    newTextBox->SetMaxLength(5);
    writableTextBoxes.push_back(newTextBox);

    wxPanel* newColorBox = new wxPanel(this, wxID_ANY, wxPoint(260, yMargin + (writableTextBoxes.size() + 1) * (yMargin)), wxSize(30, yMargin));
    newColorBox->SetBackgroundColour(*wxWHITE);
    colorBoxes.push_back(newColorBox);

    UpdateLayout();
    UpdateButtonAccessibility();
}

void LLFrame::OnRemoveTextBox(wxCommandEvent& event)
{
    if (writableTextBoxes.size() <= MIN_WRITEABLE) {
        return;
    }

    wxTextCtrl* lastTextBox = writableTextBoxes.back();
    writableTextBoxes.pop_back();
    lastTextBox->Destroy();

    wxPanel* lastColorBox = colorBoxes.back();
    colorBoxes.pop_back();
    lastColorBox->Destroy();

    UpdateLayout();
    UpdateButtonAccessibility();
}

void LLFrame::UpdateButtonAccessibility()
{

    if (writableTextBoxes.size() > MIN_WRITEABLE) {
        minusButton->Enable();
    }
    else {
        minusButton->Disable();
    }

    if (writableTextBoxes.size() >= MAX_WRITEABLE) {
        plusButton->Disable();
    }
    else {
        plusButton->Enable();
    }
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

void LLFrame::UpdateLayout()
{
    // Updated layout code to position new buttons appropriately
    int numWritableBoxes = writableTextBoxes.size();
    int windowWidth, windowHeight;
    GetClientSize(&windowWidth, &windowHeight);

    xMargin = windowWidth / 16;
    yMargin = windowHeight / 32;

    int textBoxWidth = (windowWidth / 2) - (2 * xMargin);
    int medBoxWidth = textBoxWidth / 1.32;
    int smallBoxWidth = textBoxWidth / 4;
    int buttonHeight = windowHeight / 12;
    int bigFontSize = buttonHeight / 2;
    int smallFontSize = buttonHeight / 2.25;

    wxFont bigFont(bigFontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont smallFont(smallFontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    startword->SetSize(wxSize(textBoxWidth, buttonHeight));
    startword->SetPosition(wxPoint(xMargin + smallBoxWidth, yMargin));
    startword->SetFont(bigFont);

    endword->SetSize(wxSize(textBoxWidth, buttonHeight));
    endword->SetPosition(wxPoint(xMargin + smallBoxWidth, yMargin + (numWritableBoxes + 1) * (buttonHeight + yMargin)));
    endword->SetFont(bigFont);

    for (int i = 0; i < numWritableBoxes; ++i)
    {
        colorBoxes[i]->SetSize(wxSize(smallBoxWidth, buttonHeight));
        colorBoxes[i]->SetPosition(wxPoint(xMargin / 2, yMargin + (i + 1) * (buttonHeight + yMargin)));

        writableTextBoxes[i]->SetSize(wxSize(textBoxWidth, buttonHeight));
        writableTextBoxes[i]->SetPosition(wxPoint(xMargin + smallBoxWidth, yMargin + (i + 1) * (buttonHeight + yMargin)));
        writableTextBoxes[i]->SetFont(bigFont);
    }

    minusButton->SetSize(wxSize(textBoxWidth / 4, buttonHeight));
    minusButton->SetPosition(wxPoint((xMargin * 1.5) + smallBoxWidth + textBoxWidth, yMargin + (numWritableBoxes + 1) * (buttonHeight + yMargin)));
    minusButton->SetFont(smallFont);

    plusButton->SetSize(wxSize(textBoxWidth / 4, buttonHeight));
    plusButton->SetPosition(wxPoint((xMargin * 1.5) + smallBoxWidth + textBoxWidth, yMargin + (numWritableBoxes + 2) * (buttonHeight + yMargin)));
    plusButton->SetFont(smallFont);

    // Positioning new buttons
    newGameButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    newGameButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, yMargin));
    newGameButton->SetFont(smallFont);

    difficultyButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    difficultyButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, (2 * yMargin)+buttonHeight));
    difficultyButton->SetFont(smallFont);

    aboutButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    aboutButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, (3 * yMargin) + (2 * buttonHeight)));
    aboutButton->SetFont(smallFont);

    optionsButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    optionsButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, (4 * yMargin) + (3 * buttonHeight)));
    optionsButton->SetFont(smallFont);

    
    submitButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    submitButton->SetFont(smallFont);
    submitButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, (5 * yMargin) + (5 * buttonHeight)));


    quitButton->SetSize(wxSize(medBoxWidth, buttonHeight));
    quitButton->SetPosition(wxPoint(windowWidth - medBoxWidth - xMargin / 2, (8 * yMargin) + (7 * buttonHeight)));
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