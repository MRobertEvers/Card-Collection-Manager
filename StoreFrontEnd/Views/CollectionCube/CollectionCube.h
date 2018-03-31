#pragma once
#include "../ViewTemplates/IControlledView.h"
#include <wx/wxprec.h>

class cCollectionCube;
class ivCollectionView;
class IMenuEventSource;

class CollectionCube : public IControlledView
{
public:
   CollectionCube( IMenuEventSource* aParent, const wxString& aszColName );
   ~CollectionCube();

   cCollectionCube* GetController();
   ivCollectionView* GetView();

private:
   cCollectionCube* m_Controller;
   ivCollectionView* m_View;
};

