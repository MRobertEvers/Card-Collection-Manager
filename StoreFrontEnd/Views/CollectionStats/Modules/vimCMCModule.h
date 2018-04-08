#pragma once
#include "../StoreFrontEnd/CollectionInterface.h"
#include <wx/chartpanel.h>
#include <wx/wxprec.h>
#include <memory>

class ClickableShape;
class wxChartPanel;

class ClickMode : public ChartPanelMode
{
public:
   ClickMode() {}
   ~ClickMode() {}

   void Init( wxChartPanel* panel );

   void ShowToolTip( ClickableShape* dataShape );

   void ChartEnterWindow()
   {
   }

   void ChartMouseDown( wxPoint &pt, int key );

   void ChartMouseUp( wxPoint &pt, int key )
   {
   }

   void ChartMouseMove( wxPoint &pt )
   {
   }

   void ChartMouseDrag( wxPoint &pt )
   {
   }

   void ChartMouseWheel( int rotation )
   {
   }

private:
   wxChartPanel * m_Panel;
   wxPoint m_LastPoint;
};


class vimCMCModule : public wxChartPanel
{
public:
   vimCMCModule( wxWindow* aptParent,
                 wxWindowID aiID,
                 std::shared_ptr<CollectionInterface> aptInterface );
   ~vimCMCModule();

   wxString GetColorID( CardInterface& item );

private:
   wxDECLARE_EVENT_TABLE();
   std::shared_ptr<CollectionInterface> m_ptInterface;
};