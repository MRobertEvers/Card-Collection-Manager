#pragma once
#include "CollectionInterface.h"
#include <wx/wxprec.h>
#include <memory>

class vimCountModule : public wxPanel
{
public:
   vimCountModule( wxWindow* aptParent,
                   wxWindowID aiID,
                   std::shared_ptr<CollectionInterface> aptInterface );
   ~vimCountModule();

private:
   std::shared_ptr<CollectionInterface> m_ptInterface;
};