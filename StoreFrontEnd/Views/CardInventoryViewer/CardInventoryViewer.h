#pragma once
#include "../ViewTemplates/IViewFactory.h"
#include <wx/wxprec.h>

class CCardInventoryViewer;
class VCardInventoryViewer;

class CardInventoryViewer : public IViewFactory
{
public:
   CardInventoryViewer(  );
   ~CardInventoryViewer();

   wxPanel* GetView( wxPanel* aptParent );
   CCardInventoryViewer* GetController();

private:
   VCardInventoryViewer* m_pView;
   CCardInventoryViewer* m_pController;
};
