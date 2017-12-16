#include "MetaTag.h"

MetaTag::MetaTag(Tag aTag, MetaTagType atagType)
{
   m_Type = atagType;
   m_Tag = aTag;
}

MetaTag::MetaTag(std::string aszKey, std::string aszVal, MetaTagType atagType) :
   MetaTag(std::make_pair(aszKey, aszVal), atagType)
{
}

std::string MetaTag::GetKey() const
{
   return m_Tag.first;
}

std::string MetaTag::GetVal(MetaTagType atagType) const
{
   if (CanView(atagType))
   {
      return m_Tag.second;
   }
   else
   {
      return "";
   }
}

void MetaTag::SetVal(std::string aszVal)
{
   m_Tag.second = aszVal;
}

bool MetaTag::CanView(const MetaTagType& atagType) const
{
   return (m_Type & atagType) != 0;
}