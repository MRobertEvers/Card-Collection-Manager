#pragma once
#include "../StoreFrontEnd/CollectionInterface.h"
#include "../vStatsViewer.h"
#include <wx/chartpanel.h>
#include <wx/wxprec.h>
#include <memory>

class BarClickMode : public ClickMode
{
public:
   virtual void ShowToolTip( ClickableShape* dataShape );
};


class vimCMCModule : public wxChartPanel
{
public:
   vimCMCModule( wxWindow* aptParent,
                 wxWindowID aiID,
                 std::shared_ptr<CollectionInterface> aptInterface );
   ~vimCMCModule();

   wxString GetColorID( CardInterface& item );
   wxColour GetColorFromColorID( const wxString& aszColor );

private:
   wxDECLARE_EVENT_TABLE();
   std::shared_ptr<CollectionInterface> m_ptInterface;
};