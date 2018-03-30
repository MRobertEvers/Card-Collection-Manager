#pragma once
#include "IControlledView.h"
#include <wx/wxprec.h>

class cCollectionDeckBox;
class ivCollectionView;
class IMenuEventSource;

class CollectionDeckBox : public IControlledView
{
public:
   CollectionDeckBox( IMenuEventSource* aParent, const wxString& aszColName );
   ~CollectionDeckBox();

   cCollectionDeckBox* GetController();
   ivCollectionView* GetView();

private:
   cCollectionDeckBox* m_Controller;
   ivCollectionView* m_View;
};

