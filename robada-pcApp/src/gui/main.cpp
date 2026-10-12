#include "test.h"

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
        : wxFrame(nullptr, wxID_ANY, "Robada pcApp")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);

    // let's try for some actual UI elements
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxPanel* panel = new wxPanel(this);
    label = new wxStaticText(panel, wxID_ANY, "Hello, world!");
    topsizer->Add(label, 0, wxALL, 10);
    auto button = new wxButton(panel, wxID_ANY, "Click me!");
    topsizer->Add(button, 0, wxALL, 10);
    panel->SetSizer(topsizer);

    // bind events
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    button->Bind(wxEVT_BUTTON, &MyFrame::OnClick, this);
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

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}  

void MyFrame::OnClick(wxCommandEvent& event)
{
    label->SetLabel("Wow You clicked me how dare you, pervert.");
}  