#pragma once
#include <wx/wxprec.h>
#include <vector>
// vc d's help a higher panel gather data so that the higher panel can
// organize the data on its sizer.
using namespace std;
// TODO: This only gathers data by hash.
class vcdCDBIListItemData
{
public:
   enum DataStyle
   {
      LONG_NAME = 0x0,
      SOURCE_NAME = 0x1
   };
   vcdCDBIListItemData();
   vcdCDBIListItemData(const wxString& aszCardName, DataStyle aDataStyle);
   ~vcdCDBIListItemData();

   int GetNumber() const;
   wxString GetHash() const;
   wxString GetName() const;
   wxString GetManaCost() const;
   wxString GetCardType() const;
   wxString GetSet() const;
   wxString GetMetaTag(const wxString& aszKey) const;
   wxString GetAttribute(const wxString& aszKey) const;

private:
   // Number, name, mana cost, card type
   int m_iNumber;
   wxString m_szHash;
   wxString m_szName;
   wxString m_szSet;
   wxString m_szManaCost;
   wxString m_szCardType;
   vector<wxString> m_vecSetOptions;
   vector<pair<string, string>> m_vecIdentifiers;
   vector<pair<string, string>> m_vecMetaTags;

   void parseLongName(const wxString& aszName);
   void getItemData();
   void getItemHash(const wxString& aszUID);
};

