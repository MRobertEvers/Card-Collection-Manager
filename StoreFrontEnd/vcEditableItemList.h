#pragma once
#include <map>
#include <vector>
#include "wx/wxprec.h"

class vcItemNameCheckBox;

class vcEditableItemList : public wxScrolledWindow
{
public:
   vcEditableItemList( wxWindow* aptParent,
                       wxWindowID aiID,
                       std::vector<wxString> avecOptions );
   ~vcEditableItemList();

   void RefreshNew(std::vector<wxString> avecOptions);

   std::map<wxString, bool> GetUIDModifiedMap();

private:
   std::map<wxString, vcItemNameCheckBox*> m_mapItemToVC;


};
