#include "TestPopup.h"
#include "viCollectionEditor.h"

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
wxIMPLEMENT_CLASS(SimpleTransientPopup, wxPopupTransientWindow);
wxBEGIN_EVENT_TABLE(SimpleTransientPopup, wxPopupTransientWindow)
wxEND_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup(wxWindow *parent, bool scrolled)
   :wxPopupTransientWindow(parent)
{
   m_panel = new wxScrolledWindow(this, wxID_ANY);
   m_panel->SetBackgroundColour(*wxLIGHT_GREY);

   wxStaticText *text = new wxStaticText(m_panel, wxID_ANY,
      wxT("wxPopupTransientWindow is a\n")
      wxT("wxPopupWindow which disappears\n")
      wxT("automatically when the user\n")
      wxT("clicks the mouse outside it or if it\n")
      wxT("(or its first child) loses focus in \n")
      wxT("any other way."));

   m_button = new wxButton(m_panel, Minimal_PopupButton, wxT("Press Me"));
   m_mouseText = new wxStaticText(m_panel, wxID_ANY,
      wxT("<- Test Mouse ->"));

   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
   //topSizer->Add(text, 0, wxALL, 5);
   //topSizer->Add(m_button, 0, wxALL, 5);
   //topSizer->Add(m_mouseText, 0, wxCENTRE | wxALL, 5);

   if( scrolled )
   {
      // Add a big window to ensure that scrollbars are shown when we set the
      // panel size to a lesser size below.
      //topSizer->Add(new wxPanel(m_panel, wxID_ANY, wxDefaultPosition,
      //   wxSize(600, 900)));
      topSizer->Add(new viCollectionEditor(this, 1, ""), wxSizerFlags(1).Expand());
   }

   m_panel->SetSizer(topSizer);
   if( scrolled )
   {
      // Set the fixed size to ensure that the scrollbars are shown.
      m_panel->SetSize(300, 300);

      // And also actually enable them.
      m_panel->SetScrollRate(10, 10);
   }
   else
   {
      // Use the fitting size for the panel if we don't need scrollbars.
      topSizer->Fit(m_panel);
   }

   SetClientSize(m_panel->GetSize());
}

SimpleTransientPopup::~SimpleTransientPopup()
{
}

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------