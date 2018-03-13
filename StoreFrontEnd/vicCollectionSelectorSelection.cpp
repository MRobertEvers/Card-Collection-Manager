#include "vicCollectionSelectorSelection.h"
#include "ImageViewer.h"
#include "StoreFrontEnd.h"


vicCollectionSelectorSelection::
vicCollectionSelectorSelection( wxWindow* aptParent,
                                const wxString& aszName,
                                const wxString& aszIconName,
                                const wxString& aszSet,
                                const wxString& aszMud )
   : wxPanel(aptParent, 5, wxDefaultPosition, wxDefaultSize, wxBORDER )
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
   boxSizer->AddGrowableCol( 1 );
   this->SetSizer( boxSizer );

   this->SetSizeHints( wxSize( 400, 160 ) );
   this->SetSize( wxSize( 400, 160 ) );

   m_ptImage = new ImageViewer( this, 3 );
   this->GetSizer()->Add( m_ptImage, wxSizerFlags(2).Shaped() );

   if( aszIconName != "" )
   {
      m_ptImage->DisplayImage( aszIconName, aszMud, aszSet );
   }

   wxStaticText * wxText = new wxStaticText( this, wxID_ANY, aszName,
                                             wxDefaultPosition, wxDefaultSize );
   this->GetSizer()->Add( wxText, wxSizerFlags( 1 ).CenterVertical().Border( wxALL, 2 ) );
}

vicCollectionSelectorSelection::~vicCollectionSelectorSelection()
{

}