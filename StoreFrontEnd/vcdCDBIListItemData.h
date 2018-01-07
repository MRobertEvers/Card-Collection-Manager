#pragma once
#include <wx/wxprec.h>
#include <vector>
// vc d's help a higher panel gather data so that the higher panel can
// organize the data on its sizer.

// TODO: This only gathers data by hash.
class vcdCDBIListItemData
{
public:
   enum DataStyle
   {
      LONG_NAME = 0x0,
      SOURCE_NAME = 0x1
   };
   vcdCDBIListItemData(const wxString& aszCardName, DataStyle aDataStyle);
   ~vcdCDBIListItemData();

   int GetNumber();
   wxString GetName();
   wxString GetManaCost();
   wxString GetCardType();
   wxString GetSet();

private:
   // Number, name, mana cost, card type
   int m_iNumber;
   wxString m_szName;
   wxString m_szSet;
   wxString m_szManaCost;
   wxString m_szCardType;
   std::vector<wxString> m_vecSetOptions;

   void parseLongName(const wxString& aszName);
   void getItemData();
};

