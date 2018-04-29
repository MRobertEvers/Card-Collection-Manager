#pragma once
#include "StoreFrontEnd\CollectionInterface.h"

#include <memory>

class VCollectionView;

class CCollectionView
{
public:
   CCollectionView( VCollectionView* aptView, std::shared_ptr<CollectionInterface> aptModel);
   ~CCollectionView();

   void SetCubeRenderer();

private:
   VCollectionView* m_ptView;
   std::shared_ptr<CollectionInterface> m_ptModel;
};