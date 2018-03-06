#pragma once
#include <wx/wxprec.h>
#include <vector>
// vc d's help a higher panel gather data so that the higher panel can
// organize the data on its sizer.
using namespace std;
// TODO: This only gathers data by hash.
class GroupItemData
{
public:
   enum DataStyle
   {
      LONG_NAME = 0x0,
      SOURCE_NAME = 0x1
   };
   GroupItemData();
   GroupItemData(const wxString& aszCardName, DataStyle aDataStyle);
   ~GroupItemData();

   int GetNumber() const;
   wxString GetHash() const;
   wxString GetFirstUID() const;
   wxString GetName() const;
   wxString GetManaCost() const;
   wxString GetCardType() const;
   wxString GetSet() const;
   wxString GetMetaTag(const wxString& aszKey, const wxString& aszUID = "" ) const;
   wxString GetAttribute(const wxString& aszKey) const;

private:
   // Number, name, mana cost, card type
   int m_iNumber;
   wxString m_szName;
   vector<wxString> m_vecUIDs;

   void parseLongName(const wxString& aszName);
};

