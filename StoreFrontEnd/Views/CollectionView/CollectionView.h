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

   wxPanel* GetView();

private:
   CCollectionView* m_pController;
   VCollectionView* m_pView;
};