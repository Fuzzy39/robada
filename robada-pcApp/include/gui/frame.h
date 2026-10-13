#pragma once
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif



class MyFrame : public wxFrame
{
public:
    MyFrame();
 
private:
    wxStaticText* label;
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
};

