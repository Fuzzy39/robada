#include "app.h"
#include "frame.h"
#include "TestPanel.h"
#include "ConsolePanel.h"
#include <wx/notebook.h>

enum
{
    ID_Hello = 1
};

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show();
    return true;
}

MyFrame::MyFrame()
        : wxFrame(nullptr, wxID_ANY, "Robada pcApp", wxDefaultPosition, wxSize(800, 600))
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);

    // let's try for some actual UI elements
    // parent has to be this?
    wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    // setup our current pages.
    notebook->AddPage(new ConsolePanel(notebook, this), "Console");
    notebook->AddPage( new TestPanel(notebook), "Test");
    notebook->SetSelection(0);

    // bind events
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
   
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
