#pragma once
#include <wx/wxprec.h>

class MainFrame;
class cCollectionCube;
class ivCollectionView;
class IMenuEventSource;

class CollectionCube
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

