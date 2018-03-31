#pragma once
#include "IMenuEventHandler.h"
#include <wx\wxprec.h>
#include <map>

class vCollectionCube;
class IMenuEventSource;

class cCollectionCube : public IMenuEventHandler
{
public:
   enum
   {
      Menu_DeckEditor = 0x4001,
      Menu_Save = 0x4501,
      Menu_Stats = 0x5321,
      Menu_Export_XMage = 0x5322,
      Menu_View_As_Deck = 0x4502,
      Menu_ViewHist = 0x5353
   };

   cCollectionCube( IMenuEventSource* aParent, vCollectionCube* aView, const wxString& aszColName );
   virtual ~cCollectionCube();

   wxPanel* GetView() override;

   void BindEventHandler() override;

protected:
   void handleEvent( unsigned int aiEvent ) override;

private:
   vCollectionCube* m_wxCube;
   wxString m_ColID;
   std::map<wxString, unsigned int> m_mapTitleToEventID;
};