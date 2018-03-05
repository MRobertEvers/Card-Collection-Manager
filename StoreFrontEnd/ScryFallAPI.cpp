#include "ScryFallAPI.h"
#include "wx/wxprec.h"
#include "curl/curl.h"
#include "json\json.hpp"

const char* ScryFallAPI::API_URL_BASE = "https://api.scryfall.com/";

ScryFallAPI::ScryFallAPI()
{

}


ScryFallAPI::~ScryFallAPI()
{
}

std::string 
ScryFallAPI::GetCardPrice( const std::string& aszMUD )
{
   const char* API_MULTIVERSE = "cards/multiverse/";

   std::string szQuery = API_URL_BASE;
   szQuery += API_MULTIVERSE;
   szQuery += aszMUD;

   std::string szResult = curlRequest( szQuery );

   return jsonFind( szResult );
}

std::string 
ScryFallAPI::curlRequest( const std::string& aszRequest )
{
   CURL *curl;
   CURLcode res;
   std::string readBuffer;

   curl = curl_easy_init();
   if( curl )
   {

      curl_easy_setopt( curl, CURLOPT_URL, aszRequest.c_str() );

      //curl_easy_setopt( curl, CURLOPT_URL, "https://api.scryfall.com/cards/named?exact=lightning+bolt" );

      curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L );
      curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0L );

      curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
      curl_easy_setopt( curl, CURLOPT_WRITEDATA, &readBuffer );
      res = curl_easy_perform( curl );
      curl_easy_cleanup( curl );
   }
   return readBuffer;
}

std::string 
ScryFallAPI::jsonFind( const std::string& aszJSON )
{
   nlohmann::json jParser = nlohmann::json::parse(aszJSON);

   auto iter_USD = jParser.find( "usd" );
   if( iter_USD->is_number() )
   {
      return std::to_string( iter_USD->get<float>() );
   }
   else
   {
      return iter_USD->get<std::string>();
   }

   return "";
}

size_t 
ScryFallAPI::WriteCallback( void *contents, size_t size, size_t nmemb, void *userp )
{
   ((std::string*)userp)->append( (char*)contents, size * nmemb );
   return size * nmemb;
}