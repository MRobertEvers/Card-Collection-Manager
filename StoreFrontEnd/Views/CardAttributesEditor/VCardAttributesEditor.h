#pragma once
#include "../ViewTemplates/IViewFactory.h"
#include <vector>
#include <string>
#include <map>
#include <wx/wxprec.h>
#include <wx/aui/framemanager.h>
#include <wx/grid.h>

class CardInterface;
class CCardAttributesEditor;
class VCardAttributesEditor : public wxPanel
{
public:
   enum
   {
      SAVE_BUTTON = 0x55,
      RESET_BUTTON = 0x56
   };

   enum
   {
      ITEM_GRID = 47
   };

   enum
   {
      NEED_CLEAR_ROW = 88
   };

public:
   VCardAttributesEditor( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardAttributesEditor();

   void SetController( CCardAttributesEditor* apController );

   void ViewCard( CardInterface* apCard );

   void SelectUID( const wxString& aszUID );
   void UnselectUID( const wxString& aszUID );

   std::map<std::string, std::string> GetNonDefaultAttributes();
   std::map<std::string, std::string> GetNonDefaultMetaTags();

   void ReassignDefaults();

private:
   wxDECLARE_EVENT_TABLE();

   CCardAttributesEditor* m_pController;
   CardInterface* m_pCardInterface;
   wxAuiManager m_mgr;
   
   std::map<wxString, wxString> m_mapAttributeDefaults;
   std::map<wxString, std::vector<std::string>> m_mapAttributeOptions;
   std::map<wxString, wxString> m_mapMetaDefaults;

   wxGrid* m_pAttributesTable;
   wxGrid* m_pMetaTable;
   wxGrid* m_pUIDTable;
   wxPanel* m_pButtonPanel;

   void onAttributeChanged( wxGridEvent& awxEvt );
   void onClearRow( wxCommandEvent& awxEvt );
   void onSaveButton( wxCommandEvent& awxEvt );
   void onResetButton( wxCommandEvent& awxEvt );

   wxArrayString toArrayString( const std::vector<std::string>& avecString );
};