#pragma once
#include "wx/wxprec.h"
#include "wx/popupwin.h"

class viCollectionEditor;

enum
{
   Minimal_Quit = wxID_EXIT,
   Minimal_About = wxID_ABOUT,
   Minimal_TestDialog,
   Minimal_StartSimplePopup,
   Minimal_StartScrolledPopup,
   Minimal_LogWindow,
   Minimal_PopupButton,
   Minimal_PopupSpinctrl
};

class SimpleTransientPopup : public wxPopupTransientWindow
{
public:
   SimpleTransientPopup(wxWindow *parent);
   virtual ~SimpleTransientPopup();

private:
   wxPanel * m_panel;
   wxButton *m_button;
   wxStaticText *m_mouseText;
   viCollectionEditor* m_Editor;

private:
   wxDECLARE_ABSTRACT_CLASS(SimpleTransientPopup);
   wxDECLARE_EVENT_TABLE();
};
