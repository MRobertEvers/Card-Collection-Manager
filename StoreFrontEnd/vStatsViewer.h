#pragma once
#include "CollectionInterface.h"
#include <wx/wxprec.h>
#include <memory>
#include <vector>

class vicStatsViewRow;

class vStatsViewer : public wxFrame
{
public:
   vStatsViewer( wxWindow* aptParent,
                 wxWindowID aiID,
                 std::shared_ptr<CollectionInterface> aptInterface );
   ~vStatsViewer();

private:
   std::vector<vicStatsViewRow> m_vecRows;
   std::shared_ptr<CollectionInterface> m_ptInterface;
};

