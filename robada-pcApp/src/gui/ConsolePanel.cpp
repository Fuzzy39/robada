#include "ConsolePanel.h"   

ConsolePanel::ConsolePanel(wxWindow* parent, wxFrame* mainWindow): wxPanel(parent), mainWindow(mainWindow)
{
    auto sizer = new wxGridSizer(1);
    textCtrl = new wxTextCtrl(this, wxID_ANY, "", 
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_PROCESS_ENTER);

    sizer->Add(textCtrl, 1, wxALL | wxEXPAND);
    this->SetSizer(sizer);

    // setup CLI engine.
    input = std::make_unique<std::istringstream>();
    output = std::make_unique<std::ostream>(textCtrl);
    cliEngine = std::make_unique<cli::Engine>(*input, *output);
    
    userStartPosition = textCtrl->GetLastPosition();
    textCtrlContents = textCtrl->GetValue();

    // bind events.
    textCtrl->Bind(wxEVT_TEXT_ENTER, &ConsolePanel::OnUserInput, this);
    textCtrl->Bind(wxEVT_TEXT, &ConsolePanel::OnTextChanged, this);
}

void ConsolePanel::OnTextChanged(wxCommandEvent& event)
{
    // prevent the user from deleting text... this doesn't work very well.
    // Mainly because the user could insert characters in the middle of our output.
    // TODO: Figure out a better solution. Might involve extending wxTextCtrl.
    // Yeah, that way we could override various methods.
    if(textCtrl->GetLastPosition()<userStartPosition)
    {
        textCtrl->SetValue(textCtrlContents);
        textCtrl->SetInsertionPointEnd();
        textCtrl->DiscardEdits();
        return;
    }

}

void ConsolePanel::OnUserInput(wxCommandEvent& event)
{
    // set the input stream to get our user input.
    input->str(textCtrl->GetRange(userStartPosition, textCtrl->GetLastPosition()));
    input->seekg(0); // reset the position of the input as well.

    textCtrl->AppendText("\n"); // give a newline
    if(!cliEngine->interpret())
    {
        // How can I close the app?
        mainWindow->Close(true);
        return;
    }   

    // update the last position the user can edit. TODO: put this in a callback and call interpret in a new thread or something.
    userStartPosition = textCtrl->GetLastPosition();
    textCtrlContents = textCtrl->GetValue();
}
