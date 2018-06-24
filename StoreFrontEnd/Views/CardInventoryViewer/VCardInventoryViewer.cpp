#include "VCardInventoryViewer.h"
#include "CCardInventoryViewer.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../StoreFrontEnd/CollectionInterface.h"
#include <algorithm>
#include <wx/richtext/richtextctrl.h>

wxBEGIN_EVENT_TABLE( VCardInventoryViewer, wxPanel )
//EVT_BUTTON( viCardEditor::Changes_Submit, vCollectionCube::onCardChanged )
//EVT_BUTTON( viCardEditor::Image_Changed, vCollectionCube::onNewItemSelectView )
EVT_BUTTON( VCardInventoryViewer::INV_VIEWER_OPEN_EDITOR, VCardInventoryViewer::onOpenEditor )
EVT_BUTTON( VCardInventoryViewer::INV_VIEWER_RESET_COPY, VCardInventoryViewer::onReset )
EVT_BUTTON( VCardInventoryViewer::INV_VIEWER_SAVE_COPY, VCardInventoryViewer::onSave )
wxEND_EVENT_TABLE()

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
   this->Freeze();
   if( m_pTitle != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pTitle ) );
   }
   if( m_pOptions != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pOptions ) );
   }
   m_mapShownInterfaces.clear();

   m_pOptions = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
   m_pOptions->SetSizer( new wxBoxSizer( wxVERTICAL ) );
   m_pOptions->SetAutoLayout( true );
   m_pOptions->SetScrollRate( 0, 10 );

   auto ptSFE = StoreFrontEnd::Client();
   auto lChangeTime = ptSFE->StartStopWatch();
   auto szName = apInterface->GetName();

   auto ptSF = StoreFrontEnd::Server();
   Query query;
   query.UIDs();
   query.SearchFor( szName );

   auto vecItems = ptSF->GetAllCardsStartingWith( apInterface->GetCollection()->GetColId(), query );
   int layer = 0;
   // TODO: Use a pool for these, this is slow.
   for( auto& item : vecItems )
   {
      // We don't need permanent copies
      CardInterface tmpIFace = CardInterface( item, apInterface->GetCollection() );

      VCardInventoryViewer::SetDisplay* newDisp = new VCardInventoryViewer::SetDisplay( m_pOptions, wxID_ANY, &tmpIFace );
      m_pOptions->GetSizer()->Add( newDisp, wxSizerFlags( 0 ).Top() );
      m_mapShownInterfaces.insert( std::make_pair( tmpIFace.GetSet(), tmpIFace ) );
   }
   m_mgr.AddPane( m_pOptions, GetPlainPane().CenterPane() );

   m_pTitle = new wxTextCtrl( this, wxID_ANY, szName, wxDefaultPosition, wxDefaultSize, wxTE_CENTER );
   m_pTitle->SetEditable( false );
   m_mgr.AddPane( m_pTitle, GetPlainPane().Top().BestSize( wxSize( 300, -1 ) ) );
   m_mgr.Update();

   lChangeTime = ptSFE->EndStopWatch();
   this->Thaw();
}

void
VCardInventoryViewer::onSave( wxCommandEvent & awxEvt )
{
   auto newe = dynamic_cast<SetDisplay*>((SetDisplay*)awxEvt.GetClientData());
   auto szCal = newe->GetCurrentValue();
   m_pController->OnSave( m_mapShownInterfaces[awxEvt.GetString()], szCal );
}

void 
VCardInventoryViewer::onReset( wxCommandEvent & awxEvt )
{
   m_pController->OnReset( m_mapShownInterfaces[awxEvt.GetString()] );
}

void 
VCardInventoryViewer::onOpenEditor( wxCommandEvent & awxEvt )
{
  // m_pController->OnSave( m_mapShownInterfaces[awxEvt.GetString()] );
}

wxAuiPaneInfo
VCardInventoryViewer::GetPlainPane()
{
   return wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Floatable( false ).Fixed();
}

wxBEGIN_EVENT_TABLE( VCardInventoryViewer::SetDisplay, wxPanel )
EVT_COMBOBOX( VCardInventoryViewer::INV_VIEWER_PREVIEWED_COPY, VCardInventoryViewer::SetDisplay::onPreview )
wxEND_EVENT_TABLE()

VCardInventoryViewer::SetDisplay::SetDisplay( wxWindow* aptParent, wxWindowID aiWID, CardInterface* apInterface )
   : wxPanel(aptParent, aiWID)
{
   m_mgr.SetManagedWindow( this );
   this->SetMaxSize( wxSize( 223, -1 ) );
   auto ptSF = StoreFrontEnd::Server();

   auto iCount = apInterface->GetNumber();
   auto szSet = apInterface->GetSet();
   m_szDefault = szSet;

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
         wxComboBox* cmbBox = new wxComboBox( this, Buttons::INV_VIEWER_PREVIEWED_COPY, wxEmptyString, wxDefaultPosition,
                                              wxDefaultSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY );
         m_pCombo = cmbBox;
         for( auto& opt : iter_set->second )
         {
            cmbBox->Append( opt );
            m_vecOpts.push_back( opt );
         }

         auto iter_findPick = std::find( iter_set->second.begin(), iter_set->second.end(), szSet );
         if( iter_findPick != iter_set->second.end() )
         {
            cmbBox->Select( std::distance(iter_set->second.begin(), iter_findPick ));
         }
         else
         {
            cmbBox->Select( 0 );
         }
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
   wxButton* viewButton = new wxButton( this, Buttons::INV_VIEWER_OPEN_EDITOR, "..." );
   m_mgr.AddPane( viewButton, GetPlainPane().Center().Layer(2).BestSize( wxSize( 223-125, txtCount->GetBestHeight( 50 ) ) ) );

   // Save
   // Reset
   wxPanel* tpanel = new wxPanel( this, wxID_ANY );
   tpanel->SetSizer( new wxGridSizer( 2, 0, 0 ) );
   tpanel->GetSizer()->Add( new wxButton( tpanel, Buttons::INV_VIEWER_SAVE_COPY, "Save" ), wxSizerFlags(1).Left().Expand() );
   tpanel->GetSizer()->Add( new wxButton( tpanel, Buttons::INV_VIEWER_RESET_COPY, "Reset" ), wxSizerFlags( 1 ).Right().Expand() );
   m_mgr.AddPane( tpanel, GetPlainPane().Bottom().BestSize( wxSize(223, txtCount->GetBestHeight( 50 ) ) ) );
   
   m_mgr.Update();
}

VCardInventoryViewer::SetDisplay::~SetDisplay()
{
   m_mgr.UnInit();
}

wxString 
VCardInventoryViewer::SetDisplay::GetCurrentValue()
{
   auto szVal = m_pCombo->GetValue();
   if( szVal != m_szDefault )
   {
      return szVal.substr( 1 );
   }
   else
   {
      return szVal;
   }
}

void 
VCardInventoryViewer::SetDisplay::onPreview( wxCommandEvent& awxEvt )
{
   auto newVal = awxEvt.GetString();
   if( newVal.size() == 0 )
   {
      return;
   }
   else if( newVal[0] == '*' )
   {
      newVal = newVal.substr( 1 );
   }

   if( m_pCombo != nullptr )
   {
      for( int i = 0; i < m_vecOpts.size(); i++ )
      {
         m_pCombo->Delete( 0 );
      }

      int iTarg = 0;
      if( newVal != m_szDefault )
      {
         for( int i = 0; i < m_vecOpts.size(); i++ )
         {
            auto& opt = m_vecOpts[i];
            if( opt == newVal )
            {
               iTarg = i;
               m_pCombo->Append( "*" + newVal );
            }
            else
            {
               m_pCombo->Append( opt );
            }
         }
         m_pCombo->Select( iTarg );
      }
      else
      {
         for( int i = 0; i < m_vecOpts.size(); i++ )
         {
            auto& opt = m_vecOpts[i];
            if( opt == newVal )
            {
               iTarg = i;
            }
            m_pCombo->Append( opt );
         }
         m_pCombo->Select( iTarg );
      }
   }
}

void 
VCardInventoryViewer::SetDisplay::onSave( wxCommandEvent & awxEvt )
{
   awxEvt.SetString( m_szDefault );
   awxEvt.SetClientData( this );
   awxEvt.Skip();
}

void 
VCardInventoryViewer::SetDisplay::onReset( wxCommandEvent & awxEvt )
{
   awxEvt.SetString( m_szDefault );
   awxEvt.SetClientData( this );
   awxEvt.Skip();
}

void 
VCardInventoryViewer::SetDisplay::onOpenEditor( wxCommandEvent & awxEvt )
{
   awxEvt.SetString( m_szDefault );
   awxEvt.SetClientData( this );
   awxEvt.Skip();
}

