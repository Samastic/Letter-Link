#include "letterlink.h"

// Add this to the list of includes
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
    void OnAddTextBox(wxCommandEvent& event);
    void OnRemoveTextBox(wxCommandEvent& event);
    void OnSubmit(wxCommandEvent& event);
    void OnShowChain(wxCommandEvent& event);
    void UpdateLayout();
    void SetInteractable(bool interactable);
    void OnResize(wxSizeEvent& event);
    void UpdateButtonAccessibility();
    void UpdateColorBoxes(const vector<int>& guessResponse); // New function to update color boxes

    vector<wxTextCtrl*> writableTextBoxes;
    wxButton* minusButton; // Only one minus button
    vector<wxPanel*> colorBoxes; // New vector to store color boxes
    int topMargin = 50;
    int boxHeight = 30;
    int verticalSpacing = 20;
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
    ID_RemoveTextBox, // New ID for remove button
    ID_Submit,
    ID_ShowChain
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
    // Set the height to match the monitor's height
    int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
    SetSize(wxSize(screenWidth / 2, screenHeight));

    wxMenu* menuOptions = new wxMenu;
    menuOptions->Append(ID_NewGame, "&New Game...\tCtrl-S", "Start a new game");
    menuOptions->Append(ID_ShowChain, "&Show Chain...\tCtrl-C", "Show the auto-generated chain");
    menuOptions->AppendSeparator();
    menuOptions->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuOptions, "&Options");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to Letter Link!");

    Bind(wxEVT_MENU, &LLFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &LLFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &LLFrame::OnNewGame, this, ID_NewGame);
    Bind(wxEVT_MENU, &LLFrame::OnShowChain, this, ID_ShowChain);

    // Create all UI elements initially
    startword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin), wxSize(screenWidth / 4 - 10, boxHeight), wxTE_READONLY);
    startword->SetMaxLength(5); // Set maximum length to 5

    for (int i = 0; i < 3; ++i) {
        wxTextCtrl* textBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin + (i + 1) * (boxHeight + verticalSpacing)), wxSize(screenWidth / 4 - 10, boxHeight));
        textBox->SetMaxLength(5); // Set maximum length to 5
        writableTextBoxes.push_back(textBox);

        wxPanel* colorBox = new wxPanel(this, wxID_ANY, wxPoint(260, topMargin + (i + 1) * (boxHeight + verticalSpacing)), wxSize(30, boxHeight));
        colorBox->SetBackgroundColour(*wxWHITE);
        colorBoxes.push_back(colorBox);
    }

    minusButton = new wxButton(this, ID_RemoveTextBox, "-", wxPoint(220, topMargin + (writableTextBoxes.size()) * (boxHeight + verticalSpacing)), wxSize(30, boxHeight));
    Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, ID_RemoveTextBox);

    endword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin + 4 * (boxHeight + verticalSpacing)), wxSize(screenWidth / 4 - 10, boxHeight), wxTE_READONLY);
    endword->SetMaxLength(5); // Set maximum length to 5

    plusButton = new wxButton(this, ID_AddTextBox, "+", wxPoint(260, topMargin + (writableTextBoxes.size() + 2) * (boxHeight + verticalSpacing)));
    submitButton = new wxButton(this, ID_Submit, "Submit", wxPoint(10, topMargin + (writableTextBoxes.size() + 2) * (boxHeight + verticalSpacing)));

    // Bind buttons to event handlers
    Bind(wxEVT_BUTTON, &LLFrame::OnAddTextBox, this, ID_AddTextBox);
    Bind(wxEVT_BUTTON, &LLFrame::OnSubmit, this, ID_Submit);

    // Make all controls unclickable initially
    SetInteractable(false);

    Bind(wxEVT_SIZE, &LLFrame::OnResize, this);

    UpdateLayout();
}

void LLFrame::OnNewGame(wxCommandEvent& event)
{
    // Generate a new chain and make controls interactable
    LLGame.newChain();
    vector<string> wordset = LLGame.getWordset();

    // Set start and end words
    startword->SetValue(wordset.size() > 0 ? wordset[0] : "");
    endword->SetValue(wordset.size() > 1 ? wordset[1] : "");

    // Make all controls interactable
    SetInteractable(true);
}

void LLFrame::SetInteractable(bool interactable)
{
    startword->Enable(interactable);
    endword->Enable(interactable);

    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        textBox->Enable(interactable);
    }

    minusButton->Enable(interactable && writableTextBoxes.size() > 3);
    plusButton->Enable(interactable && writableTextBoxes.size() < 7);
    submitButton->Enable(interactable);
}

void LLFrame::OnAddTextBox(wxCommandEvent& event)
{
    if (writableTextBoxes.size() >= 7) {
        return;
    }

    wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin + (writableTextBoxes.size() + 1) * (boxHeight + verticalSpacing)), wxSize(200, boxHeight));
    newTextBox->SetMaxLength(5);
    writableTextBoxes.push_back(newTextBox);

    wxPanel* newColorBox = new wxPanel(this, wxID_ANY, wxPoint(260, topMargin + (writableTextBoxes.size() + 1) * (boxHeight + verticalSpacing)), wxSize(30, boxHeight));
    newColorBox->SetBackgroundColour(*wxWHITE);
    colorBoxes.push_back(newColorBox);

    UpdateLayout();
    UpdateButtonAccessibility();
}

void LLFrame::OnRemoveTextBox(wxCommandEvent& event)
{
    if (writableTextBoxes.size() <= 3) {
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

    if (writableTextBoxes.size() > 3) {
        minusButton->Enable();
    }
    else {
        minusButton->Disable();
    }

    if (writableTextBoxes.size() >= 7) {
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
    int numWritableBoxes = writableTextBoxes.size();
    int windowWidth, windowHeight;
    GetClientSize(&windowWidth, &windowHeight);

    int horizontalMargin = windowWidth / 32;
    int verticalMargin = windowHeight / 64;
    int textBoxWidth = (windowWidth / 2) - (2 * horizontalMargin);
    int buttonHeight = windowHeight / 12;
    int fontSize = std::max(8, buttonHeight / 2);

    wxFont font(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    std::vector<std::pair<wxTextCtrl*, int>> textCtrls = {
        {startword, 0}, {endword, numWritableBoxes + 1}
    };

    for (int i = 0; i < textCtrls.size(); ++i)
    {
        wxTextCtrl* textCtrl = textCtrls[i].first;
        int positionIndex = textCtrls[i].second;

        textCtrl->SetSize(wxSize(textBoxWidth, buttonHeight));
        textCtrl->SetPosition(wxPoint(horizontalMargin, verticalMargin + positionIndex * (buttonHeight + verticalMargin)));
        textCtrl->SetFont(font);
    }

    for (int i = 0; i < numWritableBoxes; ++i)
    {
        writableTextBoxes[i]->SetSize(wxSize(textBoxWidth, buttonHeight));
        writableTextBoxes[i]->SetPosition(wxPoint(horizontalMargin, verticalMargin + (i + 1) * (buttonHeight + verticalMargin)));
        writableTextBoxes[i]->SetFont(font);

        if (i == numWritableBoxes - 1) {
            minusButton->SetSize(wxSize(textBoxWidth / 4, buttonHeight));
            minusButton->SetPosition(wxPoint(horizontalMargin + textBoxWidth + 5, verticalMargin + (i + 1) * (buttonHeight + verticalMargin)));
        }

        colorBoxes[i]->SetSize(wxSize(textBoxWidth / 4, buttonHeight));
        colorBoxes[i]->SetPosition(wxPoint(horizontalMargin + textBoxWidth + (textBoxWidth / 4), verticalMargin + (i + 1) * (buttonHeight + verticalMargin)));
    }

    // Set button sizes to match text entry boxes
    plusButton->SetSize(wxSize(textBoxWidth, buttonHeight));
    submitButton->SetSize(wxSize(textBoxWidth, buttonHeight));

    plusButton->SetFont(font);
    submitButton->SetFont(font);

    // Center buttons at the bottom
    int centerX = (windowWidth - (2 * textBoxWidth)) / 3;
    plusButton->SetPosition(wxPoint(centerX, windowHeight - buttonHeight - verticalMargin));
    submitButton->SetPosition(wxPoint(centerX + textBoxWidth + centerX, windowHeight - buttonHeight - verticalMargin));

    Refresh();
}

void LLFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void LLFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
        "About Hello World", wxOK | wxICON_INFORMATION);
}

void LLFrame::OnResize(wxSizeEvent& event)
{
    UpdateLayout();  // Call UpdateLayout to adjust layout based on new size
    event.Skip();    // Ensure the event is processed further
}
