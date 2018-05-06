#include "CollectionsOverview.h"
#include "cCollectionsOverview.h"
#include "vCollectionsOverview.h"
#include <wx/wxprec.h>


CollectionsOverview::CollectionsOverview( wxFrame* aParent )
{
   m_pView = new VCollectionsOverview( aParent, wxID_ANY );
   m_pController = new CCollectionsOverview( m_pView );

   m_pView->SetController( m_pController );
}


CollectionsOverview::~CollectionsOverview()
{
   delete m_pController;
}

std::shared_ptr<IMenuEventHandler> 
CollectionsOverview::GetEventHandler()
{
   // Overview has no menu.
   return nullptr;
}

wxPanel*
CollectionsOverview::GetView()
{
   return m_pView;
}