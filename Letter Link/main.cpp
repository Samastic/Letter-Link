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
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
 
enum
{
    ID_Hello = 1
};
 
bool LLApp::OnInit()
{
    LLFrame *frame = new LLFrame();
    frame->Show(true);
    return true;
}
 
LLFrame::LLFrame()
    : wxFrame(nullptr, wxID_ANY, "Letter Link")
{
    LetterLink LLGame;

    wxMenu *menuOptions = new wxMenu;
    menuOptions->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuOptions->AppendSeparator();
    menuOptions->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuOptions, "&Options");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );
 
    CreateStatusBar();
    SetStatusText("Welcome to Letter Link!");
 
    Bind(wxEVT_MENU, &LLFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &LLFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &LLFrame::OnExit, this, wxID_EXIT);
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
 
void LLFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

