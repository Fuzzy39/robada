#include "TestPanel.h"

TestPanel::TestPanel(wxWindow *parent): wxPanel(parent)
{
    clicked = false;

    sizer = new wxBoxSizer( wxVERTICAL );
    sizer->AddStretchSpacer();

    // label
    label = new wxStaticText(this, wxID_ANY, "Hello, world!");
    sizer->Add(label, 0, wxALL | wxALIGN_CENTER, 10);

    // button
    button = new wxButton(this, wxID_ANY, "Click me!");
    sizer->Add(button, 0, wxALL | wxALIGN_CENTER, 10);

    sizer->AddStretchSpacer(); // Adding stretch spacers to the top and bottom should vertically center the children in the sizer?

    this->SetSizer(sizer);

    
    // setup events.
    button->Bind(wxEVT_BUTTON, &TestPanel::OnClick, this);
}


void TestPanel::OnClick(wxCommandEvent& event)
{
    clicked= !clicked;
    if(clicked)
    {
        label->SetLabel("You clicked meee!!!111!!!!!11!!");
        button->SetLabel("Unclick Me?");
        sizer->Layout(); // since the length of the label changed we need the sizer to redo its work.
        return;
    }

    label->SetLabel("Aw, lame.");
    button->SetLabel("Click Me!");
    sizer->Layout();
}