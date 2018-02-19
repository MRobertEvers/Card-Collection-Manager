#pragma once
#include <map>
#include <vector>
#include "wx\wxprec.h"

typedef std::pair<std::string, std::string> Tag;

class vcEditableTraitListItem;

class vcEditableTraitList : public wxScrolledWindow
{
public:
   enum
   {
      Selection_Changed = 0x1
   };

   vcEditableTraitList( wxWindow* aptParent,
                        wxWindowID aiID,
                        const wxString& aszName,
                        const wxString& aszUID );
   ~vcEditableTraitList();

   void RefreshNew( const wxString& aszName,
                    const wxString& aszUID );

   void storeTraitListItems(std::vector<Tag> &vecSelections, std::map<std::string, std::vector<std::string>> &mapOptions);

   void storePairedAttrs(std::vector<Tag> &vecPairAttrs);

private:
   wxDECLARE_EVENT_TABLE();
   std::map<int, wxString> m_mapAttrToID;
   std::map<wxString, std::vector<wxString>> m_mapPairedAttrs;
   std::map<wxString, vcEditableTraitListItem*> m_mapTraitItems;

   void onItemSelectionChanged(wxCommandEvent& awxEvt);
   void clearList();
};