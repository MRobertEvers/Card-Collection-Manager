#include "VCardInventoryViewer.h"
#include "wx/richtext/richtextctrl.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../StoreFrontEnd/CollectionInterface.h"


VCardInventoryViewer::VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel( aptParent, aiWID )
{
   m_mgr.SetManagedWindow( this );

}

VCardInventoryViewer::~VCardInventoryViewer()
{
   m_mgr.UnInit();
}

void
VCardInventoryViewer::SetController( CCardInventoryViewer* apController )
{
   m_pController = apController;
}

void
VCardInventoryViewer::ViewCard( CardInterface* apInterface )
{
   if( m_pTitle != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pTitle ) );
   }
   if( m_vecItems.size() > 0 )
   {
      for( auto& existPane : m_vecItems )
      {
         m_mgr.ClosePane( m_mgr.GetPane( existPane ) );
      }
   }

   auto szName = apInterface->GetName();
   m_pTitle = new wxTextCtrl( this, wxID_ANY, szName, wxDefaultPosition, wxDefaultSize, wxTE_CENTER );
   m_pTitle->SetEditable( false );
   m_mgr.AddPane( m_pTitle, GetPlainPane().Top().Layer( 1 ).BestSize(wxSize(300,-1 )) );

   VCardInventoryViewer::SetDisplay* newDisp = new VCardInventoryViewer::SetDisplay( this, wxID_ANY, apInterface );
   m_vecItems.push_back( newDisp );

   m_mgr.AddPane( newDisp, GetPlainPane().Top().BestSize( newDisp->GetBestSize() ) );

   m_mgr.Update();

}

wxAuiPaneInfo 
VCardInventoryViewer::GetPlainPane()
{
   return wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Floatable( false ).Fixed();
}

VCardInventoryViewer::SetDisplay::SetDisplay( wxWindow* aptParent, wxWindowID aiWID, CardInterface* apInterface )
   : wxPanel(aptParent, aiWID)
{
   m_mgr.SetManagedWindow( this );

   auto ptSF = StoreFrontEnd::Server();

   auto iCount = apInterface->GetNumber();
   auto szSet = apInterface->GetSet();

   wxTextCtrl* txtCount = new wxTextCtrl( this, wxID_ANY, "x" + std::to_string(iCount), wxDefaultPosition,
                                          wxDefaultSize, wxTE_CENTER | wxTE_READONLY );
   m_mgr.AddPane( txtCount, GetPlainPane().Center().BestSize( wxSize( 50, txtCount->GetBestHeight( 50 ) )) );

   auto auiMidSettings = GetPlainPane().Center().Layer( 1 );
   auto mapSetOpts = ptSF->GetIdentifyingAttributeOptions( apInterface->GetName() );
   // TODO: Should not be hardcoded
   auto iter_set = mapSetOpts.find( "set" );
   if( iter_set != mapSetOpts.end() )
   {
      if( iter_set->second.size() > 1 )
      {
         wxComboBox* cmbBox = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                              wxDefaultSize, 0, NULL, wxCB_DROPDOWN | wxTE_READONLY );
         for( auto& opt : iter_set->second )
         {
            cmbBox->Append( opt );
         }

         cmbBox->Select( 0 );
         m_mgr.AddPane( cmbBox, auiMidSettings.BestSize( wxSize( 75, cmbBox->GetBestHeight( 75 ) )) );
      }
      else
      {
         wxTextCtrl* txtSet = new wxTextCtrl( this, wxID_ANY, szSet, wxDefaultPosition,
                                              wxDefaultSize, wxTE_CENTER | wxTE_READONLY );
         m_mgr.AddPane( txtSet, auiMidSettings.BestSize( wxSize( 75, txtSet->GetBestHeight( 75 ) ) ) );
      }
   }


   // Eye button
   wxButton* viewButton = new wxButton( this, wxID_ANY, "View" );
   m_mgr.AddPane( viewButton, GetPlainPane().Center().Layer(2).BestSize( wxSize( -1, txtCount->GetBestHeight( 50 ) ) ) );

   // Save

   // Reset
   wxPanel* tpanel = new wxPanel( this, wxID_ANY );
   tpanel->SetSizer( new wxGridSizer( 2, 0, 0 ) );
   tpanel->GetSizer()->Add( new wxButton( tpanel, wxID_ANY, "Save" ), wxSizerFlags(1).Left().Expand() );
   tpanel->GetSizer()->Add( new wxButton( tpanel, wxID_ANY, "Reset" ), wxSizerFlags( 1 ).Right().Expand() );
   m_mgr.AddPane( tpanel, GetPlainPane().Bottom().BestSize( wxSize(300, txtCount->GetBestHeight( 50 ) ) ) );

   m_mgr.Update();
}

VCardInventoryViewer::SetDisplay::~SetDisplay()
{
   m_mgr.UnInit();
}
