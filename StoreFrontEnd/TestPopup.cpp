#include "TestPopup.h"
#include "viCollectionEditor.h"

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
wxIMPLEMENT_CLASS(SimpleTransientPopup, wxPopupTransientWindow);
wxBEGIN_EVENT_TABLE(SimpleTransientPopup, wxPopupTransientWindow)
wxEND_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup(wxWindow *parent)
   :wxPopupTransientWindow(parent)
{
   m_panel = new wxPanel(this, wxID_ANY);
   m_panel->SetBackgroundColour(*wxLIGHT_GREY);
   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
   if( true )
   {
      // Add a big window to ensure that scrollbars are shown when we set the
      // panel size to a lesser size below.
      //topSizer->Add(new wxPanel(m_panel, wxID_ANY, wxDefaultPosition,
      //   wxSize(600, 900)));
      m_Editor = new viCollectionEditor(m_panel, 1, "22130-1");
      topSizer->Add(m_Editor, 0, wxALL, 5);
   }

   m_panel->SetSizer(topSizer);
   if( true )
   {
      // Set the fixed size to ensure that the scrollbars are shown.
      m_panel->SetSize(300, 300);

      // And also actually enable them.
      //m_panel->SetScrollRate(10, 10);
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