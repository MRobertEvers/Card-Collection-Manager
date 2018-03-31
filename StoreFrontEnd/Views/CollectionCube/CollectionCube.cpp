#include "CollectionCube.h"
#include "cCollectionCube.h"
#include "vCollectionCube.h"
#include "../ViewTemplates/IMenuEventSource.h"

CollectionCube::CollectionCube( IMenuEventSource* aParent, const wxString& aszColName )
{
   auto view = new vCollectionCube( aParent, wxID_ANY, aszColName );
   m_Controller = new cCollectionCube( aParent, view, aszColName );

   m_View = view;

   // TODO: Polymorphism with sharedptr
   aParent->BindMenuEventHandler( m_Controller );
}


CollectionCube::~CollectionCube()
{
   delete m_Controller;
}

cCollectionCube*
CollectionCube::GetController()
{
   return m_Controller;
}

ivCollectionView*
CollectionCube::GetView()
{
   return m_View;
}