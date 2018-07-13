#include "VCardEditor.h"
#include "../Views/CardView/VCardView.h"
#include "../Views/CardAttributesEditor/VCardAttributesEditor.h"


wxBEGIN_EVENT_TABLE( VCardEditor, wxPanel )
EVT_GRID_CELL_CHANGING( VCardEditor::onItemSelected )
wxEND_EVENT_TABLE()


VCardEditor::VCardEditor( wxWindow* aptParent, wxWindowID aiWID )
   : wxFrame(aptParent, aiWID, "Card Editor")
{
   m_mgr.SetManagedWindow( this );
   this->SetSizeHints( wxSize( 740, 440 ) );
   this->SetMinSize( wxSize( 740, 440 ) );
}

VCardEditor::~VCardEditor()
{
   m_mgr.UnInit();
}

void
VCardEditor::SetController( CCardEditor* apController )
{
   m_pController = apController;
}

void 
VCardEditor::SetCardTitle( const wxString & aszTitle )
{
   if( m_ptxtCtrl != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_ptxtCtrl ) );
   }
   m_ptxtCtrl = new wxTextCtrl( this, wxID_ANY, aszTitle, wxDefaultPosition,
                                wxDefaultSize, wxTE_CENTER | wxTE_READONLY );
   m_mgr.AddPane( m_ptxtCtrl,
      wxAuiPaneInfo().Left().CaptionVisible( false ).CloseButton( false ).Floatable( false )
                     .Fixed().BestSize( wxSize( 223, m_ptxtCtrl->GetBestHeight( 50 ) ) ) );
   m_mgr.Update();
}

void
VCardEditor::ShowCardViewer( IViewFactory * aptViewer )
{
   auto view = aptViewer->GetView( this );
   m_mgr.AddPane( view,
      wxAuiPaneInfo().Left().CaptionVisible( false ).CloseButton( false ).Floatable( false ).Fixed().BestSize( view->GetBestSize() ) );
   m_mgr.Update();
}

void 
VCardEditor::ShowAttributesTable( IViewFactory * aptViewer )
{
   auto view = aptViewer->GetView( this );
   m_mgr.AddPane( view,
      wxAuiPaneInfo().Center().CaptionVisible( false ).CloseButton( false ).Floatable( false ) );
   m_mgr.Update();
}

void 
VCardEditor::onItemSelected( wxGridEvent & awxEvt )
{
   if( awxEvt.GetId() == VCardAttributesEditor::ITEM_GRID )
   {
      // Make sure that others arent selected over the max.
   }
}

CardEditorControlPanel::CardEditorControlPanel()
{
   m_mgr.SetManagedWindow( this );

   wxPanel* m_pButtonPanel = new wxPanel( this, wxID_ANY );
   m_pButtonPanel->SetSizer( new wxGridSizer( 2, 0, 0 ) );
   m_pButtonPanel->GetSizer()->Add( new wxButton( m_pButtonPanel, VCardEditor::OK_BUTTON, "Submit" ), wxSizerFlags( 1 ).Left().Expand() );
   m_pButtonPanel->GetSizer()->Add( new wxButton( m_pButtonPanel, VCardEditor::CANCEL_BUTTON, "Cancel" ), wxSizerFlags( 1 ).Right().Expand() );
   m_mgr.AddPane( m_pButtonPanel, wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false )
                                                 .Floatable( false ).Bottom().BestSize( wxSize( 223, 44 ) ) );
   
   m_mgr.Update();
}

CardEditorControlPanel::~CardEditorControlPanel()
{
   m_mgr.UnInit();
}
