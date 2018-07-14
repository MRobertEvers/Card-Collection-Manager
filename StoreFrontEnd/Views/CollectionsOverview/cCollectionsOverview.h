#pragma once
#include "../ViewTemplates/IMenuEventHandler.h"
#include <map>
#include <wx/wxprec.h>

class VCollectionsOverview;

class CCollectionsOverview
{
public:
   CCollectionsOverview( VCollectionsOverview* apView );
   virtual ~CCollectionsOverview();

private:
   VCollectionsOverview* m_pView;
};