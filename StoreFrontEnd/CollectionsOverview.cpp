#include "CollectionsOverview.h"
#include "cCollectionsOverview.h"
#include "vCollectionsOverview.h"
#include "IMenuEventSource.h"


CollectionsOverview::CollectionsOverview( IMenuEventSource* aParent )
{
   m_Controller = new cCollectionsOverview( aParent );
   m_View = new vCollectionsOverview( aParent, wxID_ANY );

   // TODO: Polymorphism with sharedptr
   aParent->BindMenuEventHandler( m_Controller );
}


CollectionsOverview::~CollectionsOverview()
{
   delete m_Controller;
}

cCollectionsOverview*
CollectionsOverview::GetController()
{
   return m_Controller;
}

vCollectionsOverview*
CollectionsOverview::GetView()
{
   return m_View;
}