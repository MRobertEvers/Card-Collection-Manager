#pragma once
#include "../ViewTemplates/IControlledView.h"
#include "../StoreFrontEnd/CollectionInterface.h"
#include <wx/wxprec.h>
#include <vector>
#include <memory>

class CCollectionView;
class VCollectionView;

class CollectionView : public IControlledView
{
public:
   CollectionView( wxFrame* aptParent, std::shared_ptr<CollectionInterface> aptModel );
   ~CollectionView();

   // IControlledView
   std::shared_ptr<IMenuEventHandler> GetEventHandler() override;
   wxPanel* GetView() override;

private:
   CCollectionView* m_pController;
   VCollectionView* m_pView;

   std::shared_ptr<IMenuEventHandler> m_pHandler;
};

