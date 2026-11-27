#pragma once
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <iostream>
#include <sstream>
#include <memory>
#include <robadaCLI/Engine.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

wxDECLARE_EVENT(EVT_CLI_COMMAND_FINISHED, wxCommandEvent);

class ConsolePanel: public wxPanel
{
private:
    wxFrame* mainWindow;
    wxTextCtrl* textCtrl;
    std::unique_ptr<cli::Engine> cliEngine;
    std::unique_ptr<std::istringstream> input;
    std::unique_ptr<std::ostringstream> output;

    std::string textCtrlContents;
    wxTextPos userStartPosition;

public:
    ConsolePanel(wxWindow* parent, wxFrame* mainWindow);
private:
    void OnTextChanged(wxCommandEvent& event);
    void OnUserInput(wxCommandEvent& event);
    void OnCommandFinished(wxCommandEvent& event);
};