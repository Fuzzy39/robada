#include "ConsolePanel.h" 
#include <future>  

wxDEFINE_EVENT(EVT_CLI_COMMAND_FINISHED, wxCommandEvent);


ConsolePanel::ConsolePanel(wxWindow* parent, wxFrame* mainWindow): wxPanel(parent), mainWindow(mainWindow)
{
    auto sizer = new wxGridSizer(1);
    textCtrl = new wxTextCtrl(this, wxID_ANY, "", 
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_PROCESS_ENTER);

    sizer->Add(textCtrl, 1, wxALL | wxEXPAND);
    this->SetSizer(sizer);

    // setup CLI engine.
    input = std::make_unique<std::istringstream>();
    output = std::make_unique<std::ostringstream>();
    try
    {   
        cliEngine = std::make_unique<robadaCLI::Engine>(*input, *output);
        cliEngine->addCommand(nullptr, "clear")
            ->setDescription("Clears the CLI window.")
            ->setImplementation(
            [this](cli::UserArgs args, std::ostream& out)->bool 
            { 
                textCtrl->Clear(); 
                return true; 
            });

        cliEngine->initialize();
          
    }
    catch(const std::exception& e) 
    {   
        textCtrl->Clear();
        textCtrl->AppendText("Robada CLI failed to initialize. Reason below.\n");
        textCtrl->AppendText(e.what());
        textCtrl->SetEditable(false);
    }
   
    textCtrl->AppendText(output->str());
    output->str("");
    
    userStartPosition = textCtrl->GetLastPosition();
    textCtrlContents = textCtrl->GetValue();

    // bind events.
    textCtrl->Bind(wxEVT_TEXT_ENTER, &ConsolePanel::OnUserInput, this);
    textCtrl->Bind(wxEVT_TEXT, &ConsolePanel::OnTextChanged, this);
    Bind(EVT_CLI_COMMAND_FINISHED, &ConsolePanel::OnCommandFinished, this);
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
    input->str(textCtrl->GetRange(userStartPosition, textCtrl->GetLastPosition()).ToStdString());
    input->seekg(0); // reset the position of the input as well.

    textCtrl->AppendText("\n"); // give a newline

    // Run the console command asyncronously.
    std::thread thread([&]()
    {
        bool toReturn = cliEngine->interpret();

        // send an event...
        wxCommandEvent* event = new wxCommandEvent(EVT_CLI_COMMAND_FINISHED);
        //event->SetEventObject(this);
 
        event->SetInt(toReturn); // set the int as the return value. Good enough, I suppose.
        QueueEvent(event);
    });
    thread.detach();

    // disable the console in the meantime, while we wait for the command to complete.
    // TODO: Also disable the notebook.
    textCtrl->Disable();
   

}


void ConsolePanel::OnCommandFinished(wxCommandEvent& event)
{
    // append output.
    textCtrl->AppendText(output->str());
    output->str("");

    if(event.GetInt()==0)
    {
        // if cliEngine.interpret returns false, close the app.
        mainWindow->Close(true);
        return;
    }
  
    // update the last position the user can edit before re-enabling the cli text control.
    userStartPosition = textCtrl->GetLastPosition();
    textCtrlContents = textCtrl->GetValue();
    textCtrl->Enable();
    textCtrl->SetFocus(); // focus gets lost when the window is disabled. Put it back.
}

