#pragma once
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class TestPanel :public wxPanel
{
private:
    bool clicked;
    wxBoxSizer* sizer;
    wxStaticText* label;
    wxButton* button;

public:
    TestPanel(wxWindow* parent);

private:
    void OnClick(wxCommandEvent& event);
};