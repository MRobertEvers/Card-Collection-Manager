#pragma once
#include "CURLAPI.h"

#include <string>
#include <memory>
#include <fstream>

class ScryFallAPI
{
public:
   class ScryFallFunctor : public cURLFunctor
   {
   public:
      ScryFallFunctor( std::string* aptBuffer );
      ~ScryFallFunctor();
      size_t Append( void *contents, size_t size, size_t nmemb );

   private:
      std::string* m_ptBuffer;
   };

   ScryFallAPI();
   ~ScryFallAPI();

    std::string GetCardPrice(const std::string& aszMUD);

public:
   static const char* API_URL_BASE;

   std::string curlRequest( const std::string& aszRequest );
   std::string jsonFind( const std::string& aszJSON );

public:
   static size_t WriteCallback( void *contents, size_t size, size_t nmemb, void *userp );
};

