#include "ScryFallAPI.h"
#include "CURLAPI.h"
#include "../include/json/json.hpp"
#include <wx/wxprec.h>

// https://api.scryfall.com/cards/search?order=set&q=!“Foe-Razer%20Regent”+e%3Dptkdf&unique=prints&pretty=true
// q= indicates the fulltext search that scryfall's main sight uses.
// Each field has a code that can be set equal to something e.g.
// If we want to filter on set, q=e%3Dmm2 will include the mm2 set.
// %3D is url encoded "=". All the field keys are documented on the scryfall site.
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

   return jsonFind( { "usd" }, szResult );
}

std::string 
ScryFallAPI::GetCardImageURL( const std::string & aszCardName, const std::string & aszSet )
{
   const char* API_CARD_SEARCH = "cards/search";
   const char SET_KEY = 'e';

   std::string szQuery = API_URL_BASE;
   szQuery += API_CARD_SEARCH;
   szQuery += "?q=!" + PercentEncode( "\"" + aszCardName + "\"") + // Encode card name
              "+" + SET_KEY + PercentEncode("=") + aszSet;   // Encode the set

   std::string szResult = curlRequest( szQuery );

   return jsonFind( {"data","0","image_uris","normal"} ,szResult );
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
ScryFallAPI::jsonFind( const std::vector<std::string>& avecKeys, const std::string& aszJSON )
{
   if( aszJSON == "" )
   {
      return "";
   }
   nlohmann::json jParser = nlohmann::json::parse(aszJSON);

   nlohmann::json* jCurrent = &jParser;
   nlohmann::json::value_type iter_USD;
   for( int i = 0; i < avecKeys.size(); i++ )
   {
      int iNextInd = -1;
      auto szKey = avecKeys[i];
      if( jCurrent->is_array() )
      {
         size_t iCount;
         iNextInd = std::stoi( avecKeys[i], &iCount );
         if( iCount <= 0 )
         {
            return "";
         }
      }
      else
      {
         iNextInd = -1;
      }


      
      if( jCurrent->is_array() )
      {
         iter_USD = jCurrent->at( iNextInd );
      }
      else
      {
         auto iter = jCurrent->find( szKey );
         if( iter == jCurrent->end() )
         {
            return "";
         }
         else
         {
            iter_USD = iter.value();
         }
      }

      if( i < avecKeys.size() - 1 && ( iter_USD.is_object() || iter_USD.is_array() ) )
      {
         jCurrent = &iter_USD;
      }
      else if( i == avecKeys.size() - 1 )
      {
         if( iter_USD.is_number() )
         {
            return std::to_string( iter_USD.get<float>() );
         }
         else
         {
            return iter_USD.get<std::string>();
         }
      }
   }

   return "";
}

size_t 
ScryFallAPI::WriteCallback( void *contents, size_t size, size_t nmemb, void *userp )
{
   ((std::string*)userp)->append( (char*)contents, size * nmemb );
   return size * nmemb;
}

static std::string getEncoded( const char c )
{
   if( c == ' ' )
   {
      return "%20";
   }
   else if( c == '!' )
   {
      return "%21";
   }
   else if( c == '\"' )
   {
      return "%22";
   }
   else if( c == '#' )
   {
      return "%23";
   }
   else if( c == '$' )
   {
      return "%24";
   }
   else if( c == '&' )
   {
      return "%26";
   }
   else if( c == '\'' )
   {
      return "%27";
   }
   else if( c == '(' )
   {
      return "%28";
   }
   else if( c == ')' )
   {
      return "%29";
   }
   else if( c == '*' )
   {
      return "%2A";
   }
   else if( c == '+' )
   {
      return "%2B";
   }
   else if( c == ',' )
   {
      return "%2C";
   }
   else if( c == '/' )
   {
      return "%2F";
   }
   else if( c == ':' )
   {
      return "%3A";
   }
   else if( c == ';' )
   {
      return "%3B";
   }
   else if( c == '=' )
   {
      return "%3D";
   }
   else if( c == '?' )
   {
      return "%3F";
   }
   else if( c == '@' )
   {
      return "%40";
   }
   else if( c == '[' )
   {
      return "%5B";
   }
   else if( c == ']' )
   {
      return "%5D";
   }
   else
   {
      return std::string(1, c);
   }
}

std::string 
ScryFallAPI::PercentEncode( const std::string & aszStr )
{
   std::string szRetval;
   for( auto& c : aszStr )
   {
      szRetval += getEncoded( c );
   }
   return szRetval;
}
