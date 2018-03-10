#include "CELIOption.h"
#include "StoreFrontEnd.h"

using namespace std;

vector<CELIOption> 
CELIOption::ParseCollectionItemsList( const vector<string>& avecItems )
{
   StoreFront* ptSF = StoreFrontEnd::Server();

   vector<CELIOption> vecRetVal;
   for( auto& id : avecItems )
   {
      unsigned int Count;
      string Name;
      vector<pair<string, string>> Identifiers;
      vector<pair<string, string>> MetaTags;

      StringInterface::ParseCardLine( id, Count, Name, Identifiers, MetaTags );

      CELIOption option;
      option.Display = ptSF->CollapseCardLine( id, false );

      option.IDs = MetaTags;
      vecRetVal.push_back( option );
   }

   return vecRetVal;
}