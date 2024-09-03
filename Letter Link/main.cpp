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
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnAddTextBox(wxCommandEvent& event);
    void OnRemoveTextBox(wxCommandEvent& event);
    void OnSubmit(wxCommandEvent& event);
    void OnShowChain(wxCommandEvent& event);
    void UpdateLayout();
    void SetInteractable(bool interactable);

    vector<wxTextCtrl*> writableTextBoxes;
    int topMargin = 50;
    int boxHeight = 30;
    int verticalSpacing = 20;
    wxTextCtrl* startword;
    wxTextCtrl* endword;
    wxButton* minusButton;
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
    startword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin), wxSize(200, boxHeight), wxTE_READONLY);
    wxTextCtrl* initialTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin + boxHeight + verticalSpacing), wxSize(200, boxHeight));
    writableTextBoxes.push_back(initialTextBox);
    endword = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin + 2 * (boxHeight + verticalSpacing)), wxSize(200, boxHeight), wxTE_READONLY);

    minusButton = new wxButton(this, ID_RemoveTextBox, "-", wxPoint(220, topMargin));
    plusButton = new wxButton(this, ID_AddTextBox, "+", wxPoint(220, topMargin + (writableTextBoxes.size() + 1) * (boxHeight + verticalSpacing)));
    submitButton = new wxButton(this, ID_Submit, "Submit", wxPoint(10, topMargin + (writableTextBoxes.size() + 2) * (boxHeight + verticalSpacing)));

    // Bind buttons to event handlers
    Bind(wxEVT_BUTTON, &LLFrame::OnAddTextBox, this, ID_AddTextBox);
    Bind(wxEVT_BUTTON, &LLFrame::OnRemoveTextBox, this, ID_RemoveTextBox);
    Bind(wxEVT_BUTTON, &LLFrame::OnSubmit, this, ID_Submit);

    // Make all controls unclickable initially
    SetInteractable(false);

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
    // Enable or disable all text boxes and buttons
    startword->Enable(interactable);
    endword->Enable(interactable);
    for (wxTextCtrl* textBox : writableTextBoxes)
    {
        textBox->Enable(interactable);
    }
    minusButton->Enable(interactable);
    plusButton->Enable(interactable);
    submitButton->Enable(interactable);
}

void LLFrame::OnAddTextBox(wxCommandEvent& event)
{
    wxTextCtrl* newTextBox = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10, topMargin + (writableTextBoxes.size() + 1) * (boxHeight + verticalSpacing)), wxSize(200, boxHeight));
    writableTextBoxes.push_back(newTextBox);
    UpdateLayout();
}

void LLFrame::OnRemoveTextBox(wxCommandEvent& event)
{
    if (!writableTextBoxes.empty())
    {
        wxTextCtrl* lastTextBox = writableTextBoxes.back();
        writableTextBoxes.pop_back();
        lastTextBox->Destroy();
        UpdateLayout();
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

    bool isValidChain = LLGame.evalChain(LLGame.guess);

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

void LLFrame::UpdateLayout()
{
    int numWritableBoxes = writableTextBoxes.size();
    int totalHeight = topMargin + (2 + numWritableBoxes) * (boxHeight + verticalSpacing) + topMargin;

    startword->SetPosition(wxPoint(10, topMargin));

    for (int i = 0; i < numWritableBoxes; ++i)
    {
        writableTextBoxes[i]->SetPosition(wxPoint(10, topMargin + (i + 1) * (boxHeight + verticalSpacing)));
    }

    endword->SetPosition(wxPoint(10, topMargin + (numWritableBoxes + 1) * (boxHeight + verticalSpacing)));

    wxWindow* plusButton = FindWindow(ID_AddTextBox);
    wxWindow* minusButton = FindWindow(ID_RemoveTextBox);
    wxWindow* submitButton = FindWindow(ID_Submit);

    minusButton->SetPosition(wxPoint(220, topMargin));
    plusButton->SetPosition(wxPoint(220, topMargin + (numWritableBoxes + 1) * (boxHeight + verticalSpacing)));
    submitButton->SetPosition(wxPoint(10, topMargin + (numWritableBoxes + 2) * (boxHeight + verticalSpacing)));

    SetSize(wxSize(320, totalHeight + 100));
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
