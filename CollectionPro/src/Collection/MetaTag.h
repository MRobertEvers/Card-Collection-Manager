#pragma once
#include <string>
#include "../Support/TypeDefs.h"

enum MetaTagType : unsigned short
{
   None = 0x0, // Used in filtering out meta info.
   Public = 0x1, // 001
   Ignored = 0x2, // 010
   Visible = 0x3, // 011
   Tracking = 0x4, // 100    // Tracking is recorded in the meta tag file.
   Persistent = 0x7, // 0111
   Hidden = 0x8, // 1000     // Hidden is not recorded in meta tag file.
   Any = 0xF // 1111
};

class MetaTag
{
public:
   MetaTag(Tag aTag, MetaTagType atagType);
   MetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType);

   std::string GetKey() const;
   std::string GetVal(MetaTagType atagType = Any) const;
   void SetVal(std::string aszVal);

   bool CanView(const MetaTagType& atagType) const;

private:
   MetaTagType m_Type;

   Tag m_Tag;
};

