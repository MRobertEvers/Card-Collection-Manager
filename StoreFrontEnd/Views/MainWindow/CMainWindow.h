#pragma once
#include <wx/wxprec.h>
#include <memory>

class VMainWindow;
class IMenuEventHandler;
class IControlledView;

class CMainWindow
{
public:
   enum CollectionViewType
   {
      Cube_View,
      Deck_View
   };

   CMainWindow( VMainWindow* aptView );
   ~CMainWindow();

   void ShowCollectionsOverview();
   void ShowCollection( const wxString& aszColID, CollectionViewType aType = Cube_View );

   void ImportSource();

private:
   void uiSetMainPanel( std::shared_ptr<IControlledView> apView );

   VMainWindow* m_View;
   std::shared_ptr<IControlledView> m_ptrControlledView;
};

