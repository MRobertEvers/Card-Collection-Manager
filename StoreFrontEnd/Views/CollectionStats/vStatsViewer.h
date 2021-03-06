#pragma once
#include "../StoreFrontEnd/CollectionInterface.h"
#include <wx/wxprec.h>
#include <wx/event.h>
#include <memory>
#include <vector>
#include <wx/chartpanel.h>
#include <wx/aui/framemanager.h>

class ClickableShape;
class wxChartPanel;

class ClickMode : public ChartPanelMode
{
public:
   ClickMode() {}
   virtual ~ClickMode() {}

   void Init( wxChartPanel* panel );

   virtual void ShowToolTip( ClickableShape* dataShape );

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

protected:
   wxChartPanel * m_Panel;
   wxPoint m_LastPoint;
};


class vStatsViewer : public wxFrame
{
public:
   vStatsViewer( wxWindow* aptParent,
                 wxWindowID aiID,
                 std::shared_ptr<CollectionInterface> aptInterface );
   ~vStatsViewer();

   void OnMouseEvents( wxMouseEvent &ev );
   void LoseFocus( wxMouseCaptureLostEvent& awxevt );

private:
   wxDECLARE_EVENT_TABLE();
   std::shared_ptr<CollectionInterface> m_ptInterface;
   wxAuiManager m_mgr;
};

