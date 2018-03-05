#pragma once
#include <string>

class ScryFallAPI
{
public:
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

