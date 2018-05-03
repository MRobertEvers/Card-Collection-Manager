#pragma once
#include "../ViewTemplates/IMenuEventHandler.h"
#include <map>
#include <wx/wxprec.h>

class VCollectionsOverview;
class MainFrame;

class CCollectionsOverview : public IMenuEventHandler
{
public:

   CCollectionsOverview( IMenuEventSource* aParent );
   virtual ~CCollectionsOverview();

   wxPanel* GetView() override;

   void BindEventHandler();

protected:
   void handleEvent(unsigned int aiEvent);

private:
   VCollectionsOverview * m_wxOverview;
   std::map<wxString, unsigned int> m_mapTitleToEventID;
};