#include "vimCountModule.h"

vimCountModule::vimCountModule( wxWindow* aptParent,
                                wxWindowID aiID,
                                std::shared_ptr<CollectionInterface> aptInterface )
   : wxPanel(aptParent, aiID, wxDefaultPosition, wxDefaultSize, wxBORDER), m_ptInterface(aptInterface)
{
   wxGridSizer* sizer = new wxGridSizer( 1, 1, 0, 0 );
   this->SetSizer( sizer );

   wxStaticText* txt = new wxStaticText( this, 1, "Count: " + std::to_string(m_ptInterface->GetItemCount()) );
   sizer->Add( txt, wxSizerFlags( 1 ) );

   this->SetSizeHints( 120, 120 );
}

vimCountModule::~vimCountModule()
{

}