#pragma once
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <iostream>
#include <sstream>
#include <memory>
#include <cli/Engine.h>

class ConsolePanel: public wxPanel
{
private:
    wxFrame* mainWindow;
    wxTextCtrl* textCtrl;
    std::unique_ptr<cli::Engine> cliEngine;
    std::unique_ptr<std::istringstream> input;
    std::unique_ptr<std::ostream> output;

    std::string textCtrlContents;
    wxTextPos userStartPosition;

public:
    ConsolePanel(wxWindow* parent, wxFrame* mainWindow);
private:
    void OnTextChanged(wxCommandEvent& event);
    void OnUserInput(wxCommandEvent& event);
};