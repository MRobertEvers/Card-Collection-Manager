#include "CollectionDeckBox.h"
#include "vCollectionDeckBox.h"
#include "cCollectionDeckBox.h"
#include "IMenuEventSource.h"


CollectionDeckBox::CollectionDeckBox( IMenuEventSource* aParent, const wxString& aszColName )
{
   auto view = new vCollectionDeckBox( aParent, wxID_ANY, aszColName );
   m_Controller = new cCollectionDeckBox( aParent, view, aszColName );

   m_View = view;

   // TODO: Polymorphism with sharedptr
   aParent->BindMenuEventHandler( m_Controller );
}


CollectionDeckBox::~CollectionDeckBox()
{
   delete m_Controller;
}

cCollectionDeckBox*
CollectionDeckBox::GetController()
{
   return m_Controller;
}

ivCollectionView*
CollectionDeckBox::GetView()
{
   return m_View;
}