#pragma once
#include "../Support/TypeDefs.h"
#include <string>

class MetaTag
{
public:
   enum Type : unsigned short
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

public:
   MetaTag(Tag aTag, MetaTag::Type atagType);
   MetaTag(const std::string& aszKey, const std::string& aszVal, MetaTag::Type atagType);

   std::string GetKey() const;
   std::string GetVal( const Type& atagType = Any) const;
   void SetVal(std::string aszVal);

   bool CanView(const Type& atagType) const;

private:
   Type m_Type;
   Tag m_Tag;

public:
   static Type DetermineMetaTagType( const std::string& aszTag );

   static std::string GetUIDKey();
   static std::string GetSessionKey();
   static std::string GetHashKey();
   static std::string GetAddressKey();
   static std::string MakeIgnoredTag( const std::string& aszTag );
   static std::string MakeTrackingTag( const std::string& aszTag );
};

