#pragma once
#include "../ViewTemplates/IControlledView.h"
#include "../StoreFrontEnd/CollectionInterface.h"
#include <wx/wxprec.h>
#include <vector>
#include <memory>

class CCollectionView;
class VCollectionView;
class IMenuEventSource;

class CollectionView : public IControlledView
{
public:
   CollectionView( wxFrame* aptParent, IMenuEventSource* apSource, std::shared_ptr<CollectionInterface> aptModel );
   ~CollectionView();

   wxPanel* GetView();
   CCollectionView* GetController();

private:
   CCollectionView* m_pController;
   VCollectionView* m_pView;
};