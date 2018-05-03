#include "CollectionsOverview.h"
#include "cCollectionsOverview.h"
#include "vCollectionsOverview.h"
#include "../ViewTemplates/IMenuEventSource.h"


CollectionsOverview::CollectionsOverview( IMenuEventSource* aParent )
{
   m_Controller = new CCollectionsOverview( aParent );
   m_View = new VCollectionsOverview( aParent, wxID_ANY );

   // TODO: Polymorphism with sharedptr
   aParent->BindMenuEventHandler( m_Controller );
}


CollectionsOverview::~CollectionsOverview()
{
   delete m_Controller;
}

CCollectionsOverview*
CollectionsOverview::GetController()
{
   return m_Controller;
}

VCollectionsOverview*
CollectionsOverview::GetView()
{
   return m_View;
}