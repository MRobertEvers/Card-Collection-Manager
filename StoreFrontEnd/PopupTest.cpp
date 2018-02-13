/////////////////////////////////////////////////////////////////////////////
// Name:        popup.cpp
// Purpose:     Popup wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// Created:     2005-02-04
// Copyright:   (c) 2005 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/popupwin.h"
#include "wx/spinctrl.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
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

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
class SimpleTransientPopup : public wxPopupTransientWindow
{
public:
   SimpleTransientPopup(wxWindow *parent, bool scrolled);
   virtual ~SimpleTransientPopup();

   // wxPopupTransientWindow virtual methods are all overridden to log them
   virtual void Popup(wxWindow *focus = NULL) wxOVERRIDE;
   virtual void OnDismiss() wxOVERRIDE;
   virtual bool ProcessLeftDown(wxMouseEvent& event) wxOVERRIDE;
   virtual bool Show(bool show = true) wxOVERRIDE;

private:
   wxScrolledWindow * m_panel;
   wxButton *m_button;
   wxSpinCtrl *m_spinCtrl;
   wxStaticText *m_mouseText;

private:
   void OnMouse(wxMouseEvent &event);
   void OnSize(wxSizeEvent &event);
   void OnSetFocus(wxFocusEvent &event);
   void OnKillFocus(wxFocusEvent &event);
   void OnButton(wxCommandEvent& event);
   void OnSpinCtrl(wxSpinEvent& event);

private:
   wxDECLARE_ABSTRACT_CLASS(SimpleTransientPopup);
   wxDECLARE_EVENT_TABLE();
};

//----------------------------------------------------------------------------
// SimpleTransientPopup
//----------------------------------------------------------------------------
wxIMPLEMENT_CLASS(SimpleTransientPopup, wxPopupTransientWindow);

wxBEGIN_EVENT_TABLE(SimpleTransientPopup, wxPopupTransientWindow)
EVT_MOUSE_EVENTS(SimpleTransientPopup::OnMouse)
EVT_SIZE(SimpleTransientPopup::OnSize)
EVT_SET_FOCUS(SimpleTransientPopup::OnSetFocus)
EVT_KILL_FOCUS(SimpleTransientPopup::OnKillFocus)
EVT_BUTTON(Minimal_PopupButton, SimpleTransientPopup::OnButton)
EVT_SPINCTRL(Minimal_PopupSpinctrl, SimpleTransientPopup::OnSpinCtrl)
wxEND_EVENT_TABLE()

SimpleTransientPopup::SimpleTransientPopup(wxWindow *parent, bool scrolled)
   :wxPopupTransientWindow(parent)
{
   m_panel = new wxScrolledWindow(this, wxID_ANY);
   m_panel->SetBackgroundColour(*wxLIGHT_GREY);

   // Keep this code to verify if mouse events work, they're required if
   // you're making a control like a combobox where the items are highlighted
   // under the cursor, the m_panel is set focus in the Popup() function
   m_panel->Connect(wxEVT_MOTION,
      wxMouseEventHandler(SimpleTransientPopup::OnMouse),
      NULL, this);

   wxStaticText *text = new wxStaticText(m_panel, wxID_ANY,
      wxT("wxPopupTransientWindow is a\n")
      wxT("wxPopupWindow which disappears\n")
      wxT("automatically when the user\n")
      wxT("clicks the mouse outside it or if it\n")
      wxT("(or its first child) loses focus in \n")
      wxT("any other way."));

   m_button = new wxButton(m_panel, Minimal_PopupButton, wxT("Press Me"));
   m_spinCtrl = new wxSpinCtrl(m_panel, Minimal_PopupSpinctrl, wxT("Hello"));
   m_mouseText = new wxStaticText(m_panel, wxID_ANY,
      wxT("<- Test Mouse ->"));

   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
   topSizer->Add(text, 0, wxALL, 5);
   topSizer->Add(m_button, 0, wxALL, 5);
   topSizer->Add(m_spinCtrl, 0, wxALL, 5);
   topSizer->Add(m_mouseText, 0, wxCENTRE | wxALL, 5);

   if( scrolled )
   {
      // Add a big window to ensure that scrollbars are shown when we set the
      // panel size to a lesser size below.
      topSizer->Add(new wxPanel(m_panel, wxID_ANY, wxDefaultPosition,
         wxSize(600, 900)));
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

void SimpleTransientPopup::Popup(wxWindow* WXUNUSED(focus))
{
   wxLogMessage("%p SimpleTransientPopup::Popup", this);
   wxPopupTransientWindow::Popup();
}

void SimpleTransientPopup::OnDismiss()
{
   wxLogMessage("%p SimpleTransientPopup::OnDismiss", this);
   wxPopupTransientWindow::OnDismiss();
}

bool SimpleTransientPopup::ProcessLeftDown(wxMouseEvent& event)
{
   wxLogMessage("%p SimpleTransientPopup::ProcessLeftDown pos(%d, %d)", this, event.GetX(), event.GetY());
   return wxPopupTransientWindow::ProcessLeftDown(event);
}
bool SimpleTransientPopup::Show(bool show)
{
   wxLogMessage("%p SimpleTransientPopup::Show %d", this, int(show));
   return wxPopupTransientWindow::Show(show);
}

void SimpleTransientPopup::OnSize(wxSizeEvent &event)
{
   wxLogMessage("%p SimpleTransientPopup::OnSize", this);
   event.Skip();
}

void SimpleTransientPopup::OnSetFocus(wxFocusEvent &event)
{
   wxLogMessage("%p SimpleTransientPopup::OnSetFocus", this);
   event.Skip();
}

void SimpleTransientPopup::OnKillFocus(wxFocusEvent &event)
{
   wxLogMessage("%p SimpleTransientPopup::OnKillFocus", this);
   event.Skip();
}

void SimpleTransientPopup::OnMouse(wxMouseEvent &event)
{
   wxRect rect(m_mouseText->GetRect());
   rect.SetX(-100000);
   rect.SetWidth(1000000);
   wxColour colour(*wxLIGHT_GREY);

   if( rect.Contains(event.GetPosition()) )
   {
      colour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
      wxLogMessage("%p SimpleTransientPopup::OnMouse pos(%d, %d)",
         event.GetEventObject(), event.GetX(), event.GetY());
   }

   if( colour != m_mouseText->GetBackgroundColour() )
   {
      m_mouseText->SetBackgroundColour(colour);
      m_mouseText->Refresh();
   }
   event.Skip();
}

void SimpleTransientPopup::OnButton(wxCommandEvent& event)
{
   wxLogMessage("%p SimpleTransientPopup::OnButton ID %d", this, event.GetId());

   wxButton *button = wxDynamicCast(event.GetEventObject(), wxButton);
   if( button->GetLabel() == wxT("Press Me") )
      button->SetLabel(wxT("Pressed"));
   else
      button->SetLabel(wxT("Press Me"));

   event.Skip();
}

void SimpleTransientPopup::OnSpinCtrl(wxSpinEvent& event)
{
   wxLogMessage("%p SimpleTransientPopup::OnSpinCtrl ID %d Value %d",
      this, event.GetId(), event.GetInt());
   event.Skip();
}

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------