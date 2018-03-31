#pragma once
#include "../StoreFrontEnd/CollectionInterface.h"
#include <wx/wxprec.h>
#include <memory>

class vimCMCModule : public wxPanel
{
public:
   vimCMCModule( wxWindow* aptParent,
                 wxWindowID aiID,
                 std::shared_ptr<CollectionInterface> aptInterface );
   ~vimCMCModule();

private:
   std::shared_ptr<CollectionInterface> m_ptInterface;
};