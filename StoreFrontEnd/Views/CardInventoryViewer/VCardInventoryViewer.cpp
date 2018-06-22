#include "VCardInventoryViewer.h"
#include "wx/richtext/richtextctrl.h"
#include "../StoreFrontEnd/CardInterface.h"

VCardInventoryViewer::VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel( aptParent, aiWID )
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );

   this->Layout();
}

VCardInventoryViewer::~VCardInventoryViewer()
{
}

void
VCardInventoryViewer::SetController( CCardInventoryViewer* apController )
{
   m_pController = apController;
}

void
VCardInventoryViewer::ViewCard( CardInterface* apInterface )
{
   //auto szName = apInterface->GetName();

   //m_pTitle = new wxRichTextCtrl(
   //   this, wxID_ANY, wxEmptyString, wxDefaultPosition,
   //   wxDefaultSize, wxRE_READONLY | wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS );

   //wxFont boldFont = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
   //m_pTitle->SetFont( boldFont );

   //m_pTitle->BeginUnderline();
   //m_pTitle->BeginAlignment( wxTEXT_ALIGNMENT_CENTRE );
   //m_pTitle->WriteText( szName );

   //GetSizer()->Add( m_pTitle, wxSizerFlags(0).Expand().Align( wxTOP | wxCenter ) );


}
