#include "VCardInventoryViewer.h"
#include "wx/richtext/richtextctrl.h"

VCardInventoryViewer::VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel( aptParent, aiWID )
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );

   wxRichTextCtrl* richTextCtrl = new wxRichTextCtrl(
      this, wxID_ANY, wxEmptyString, wxDefaultPosition,
      wxSize( 200, 200 ), wxRE_READONLY | wxVSCROLL | wxHSCROLL | wxBORDER_NONE | wxWANTS_CHARS );

   wxFont boldFont = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
   richTextCtrl->SetFont( boldFont );

   richTextCtrl->BeginUnderline();
   richTextCtrl->BeginAlignment( wxTEXT_ALIGNMENT_CENTRE );
   richTextCtrl->BeginBold();

   richTextCtrl->WriteText( wxT( "by Julian Smart" ) );

   boxSizer->Add( richTextCtrl, wxSizerFlags().Align( wxTOP ) );
   this->Layout();
}

VCardInventoryViewer::~VCardInventoryViewer()
{
}

void
VCardInventoryViewer::SetController( CCardInventoryViewer * apController )
{
}

void
VCardInventoryViewer::ViewCard( CardInterface * apInterface )
{
}
