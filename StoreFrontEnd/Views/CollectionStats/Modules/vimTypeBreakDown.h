#pragma once
#include "../StoreFrontEnd/CollectionInterface.h"
#include <wx/wxprec.h>
#include <memory>

class vimTypeBreakDown : public wxPanel
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

