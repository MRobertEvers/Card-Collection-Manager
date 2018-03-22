#pragma once
#include "CollectionInterface.h"
#include <wx/wxprec.h>
#include <memory>

class viHistoryViewer : public wxFrame
{
public:
   viHistoryViewer( wxWindow* aptParent,
                    wxWindowID aiID,
                    std::shared_ptr<CollectionInterface> aptInterface);
   ~viHistoryViewer();

private:
   DECLARE_EVENT_TABLE()

   std::shared_ptr<CollectionInterface> m_ptInterface;

   void onResize( wxSizeEvent &ev );
};

