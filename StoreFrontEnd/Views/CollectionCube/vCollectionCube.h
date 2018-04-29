#pragma once
#include "../ViewTemplates/ivCollectionView.h"
#include <wx/wxprec.h>
#include <wx/object.h> 
#include <wx/grid.h>

class vcCollectionCubeDisplay;
class IMenuEventSource;

class vCollectionCube : public ivCollectionView
{
public:
   vCollectionCube( IMenuEventSource* aParent, wxWindowID aiWID, const wxString& aszColID );
   ~vCollectionCube();

   void PrepareInterface();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionCubeDisplay* m_vcItemList;

   void onNewItemSelected( wxGridEvent& awxEvt );
   void onEditorAccept( wxCommandEvent& awxEvt ) override;
   void onCardChanged( wxCommandEvent& awxEvt ) override;
   void buildGroupPanel();
};
