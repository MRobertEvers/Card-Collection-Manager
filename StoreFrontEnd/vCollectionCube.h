#pragma once
#include "wx/wxprec.h"
#include "ivCollectionView.h"
#include <wx/object.h> 
#include <wx/grid.h>

class vcCollectionCubeDisplay;
class MainFrame;

class vCollectionCube : public ivCollectionView
{
public:
   vCollectionCube( MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID );
   ~vCollectionCube();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionCubeDisplay* m_vcItemList;

   void onNewItemSelected( wxGridEvent& awxEvt );
   void onCardChanged( wxCommandEvent& awxEvt ) override;
   void buildGroupPanel();
};

