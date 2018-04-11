#pragma once
#include "../StoreFrontEnd/CollectionInterface.h"
#include "../vStatsViewer.h"
#include <wx/chartpanel.h>
#include <wx/wxprec.h>
#include <memory>

class PieClickMode : public ClickMode
{
public:
   virtual void ShowToolTip( ClickableShape* dataShape );
};

class vimTypeBreakDown : public wxChartPanel
{
public:
   vimTypeBreakDown( wxWindow* aptParent,
                     wxWindowID aiID,
                     std::shared_ptr<CollectionInterface> aptInterface );
   ~vimTypeBreakDown();

private:
   std::shared_ptr<CollectionInterface> m_ptInterface;

   wxColour GetColorForGroup( const wxString& aszGroup );
};

