#include "MetaTag.h"
#include "../Config.h"

MetaTag::MetaTag(Tag aTag, MetaTag::Type atagType)
{
   m_Type = atagType;
   m_Tag = aTag;
}

MetaTag::MetaTag( const std::string& aszKey, const std::string& aszVal, MetaTag::Type atagType)
   : MetaTag(std::make_pair(aszKey, aszVal), atagType)
{
}

std::string 
MetaTag::GetKey() const
{
   return m_Tag.first;
}

std::string
MetaTag::GetVal(const MetaTag::Type& atagType) const
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

void 
MetaTag::SetVal(std::string aszVal)
{
   m_Tag.second = aszVal;
}

bool 
MetaTag::CanView(const MetaTag::Type& atagType) const
{
   return (m_Type & atagType) != 0;
}


MetaTag::Type
MetaTag::DetermineMetaTagType( const std::string& aszTagKey )
{
   MetaTag::Type mTagType = Public;
   if( ( aszTagKey.size() > 0 ) &&
       ( aszTagKey[0] == *Config::IgnoredTagId ) )
   {
      mTagType = Ignored;
   }

   if( ( mTagType == Ignored ) &&
       ( aszTagKey.size() > 1 ) &&
       ( aszTagKey[1] == *Config::TrackingTagId ) )
   {
      mTagType = Tracking;
   }

   if( mTagType == Public )
   {
      if( ( aszTagKey == GetAddressKey() ) ||
          ( aszTagKey == GetSessionKey() ) ||
          ( aszTagKey == GetUIDKey()     ) ||
          ( aszTagKey == GetHashKey()    ) )
      {
         mTagType = Tracking;
      }
   }

   return mTagType;
}

std::string
MetaTag::GetUIDKey()
{
   return Config::TrackingTagId + std::string( "UID" );
}

std::string
MetaTag::GetSessionKey()
{
   return Config::TrackingTagId + std::string( "Session" );
}

std::string
MetaTag::GetHashKey()
{
   return Config::TrackingTagId + std::string( "hash" );
}

std::string
MetaTag::GetAddressKey()
{
   return "Address";
}

std::string
MetaTag::MakeIgnoredTag( const std::string& aszTag )
{
   return Config::IgnoredTagId + aszTag;
}

std::string
MetaTag::MakeTrackingTag( const std::string& aszTag )
{
   return Config::TrackingTagId + aszTag;
}
