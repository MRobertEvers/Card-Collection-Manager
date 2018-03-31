#include "ScryFallAPI.h"
#include "CURLAPI.h"
#include "../include/json/json.hpp"
#include <wx/wxprec.h>

ScryFallAPI::
ScryFallFunctor::ScryFallFunctor( std::string* aptBuffer )
   : m_ptBuffer( aptBuffer )
{
}

ScryFallAPI::
ScryFallFunctor::~ScryFallFunctor()
{
}

size_t
ScryFallAPI::
ScryFallFunctor::Append( void *contents, size_t size, size_t nmemb )
{
   m_ptBuffer->append( (char*)contents, size * nmemb );
   return size * nmemb;
}


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
   std::string readBuffer;
   ScryFallFunctor jsonCallBack( &readBuffer );
   CURLAPI::Easy_HTTPS( aszRequest, &jsonCallBack );
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