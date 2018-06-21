#include "StoreFront.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>

#include "../src/Tests/AddressTest.h"
#include "../src/Tests/CollectionTest.h"
#include "../src/Tests/CollectionItemTest.h"
#include "../src/Tests/CopyTest.h"
#include "Support/JSONImporter.h"
#include "Config.h"

StoreFront::StoreFront()
{
   AddresserTest::Test();

   //SelfTest();
   // No Server for now
   m_ColSource = new CollectionSource();
   m_ColSource->LoadLib(Config::Instance()->GetSourceFilePath());

   m_ColFactory = new CollectionFactory(m_ColSource);
}


StoreFront::~StoreFront()
{
}

bool
StoreFront::SelfTest()
{
   bool bTest = true;
   //CollectionItemTest cit;
   //bTest &= cit.AddCopy_Test();
   //bTest &= cit.RemoveCopy_EntireChain_Test();
   //bTest &= cit.RemoveCopy_PartialChain_Test();
   //bTest &= cit.FindCopies_All_Test();
   //bTest &= cit.FindCopies_Virtual_Test();
   //bTest &= cit.FindCopies_Borrowed_Test();
   //bTest &= cit.FindCopies_Local_Test();

   //CopyTest ct;
   //bTest &= ct.CreateCopy_Test();
   //bTest &= ct.SetMetaTag_Test();
   //bTest &= ct.Hash_Test();
   //bTest &= ct.SetParent_Test();
   //bTest &= ct.AddResident_InParent_AlreadyDesignated_Test();
   //bTest &= ct.AddResident_InParent_ExistingChain_NotDesignatedByParentChain_Test();
   //bTest &= ct.AddResident_InParent_NewChain_NotDesignatedByParentChain_Test();
   //bTest &= ct.AddResident_InResidentNotParent_AlreadyDesignated_Test();
   //bTest &= ct.AddResident_InResidentNotParent_ExistingChain_NotDesignated_Test();
   //bTest &= ct.AddResident_InResidentNotParent_NewChain_NotDesignated_Test();
   //bTest &= ct.ResidentIn_ChainOfParent_Test();
   //bTest &= ct.ResidentIn_NotParent_AddedResident_Test();
   //bTest &= ct.ResidentIn_NotParent_ChainOfAddedResident_Test();
   //bTest &= ct.ResidentIn_Parent_ParentIsNotResident_Test();
   //bTest &= ct.ResidentIn_Parent_ParentIsResident_Test();
   //bTest &= ct.ResidentIn_ChainOfParent_Test();
   //bTest &= ct.RemoveResident_NotParent_InChainOfResident_EntireChain_Test();
   //bTest &= ct.RemoveResident_NotParent_InChainOfResident_NotEntireChain_Test();
   //bTest &= ct.RemoveResident_NotParent_NotInChainOfResident_Test();
   //bTest &= ct.RemoveResident_Parent_InChainOfParent_EntireChain();
   //bTest &= ct.RemoveResident_Parent_InChainOfParent_NotEntireChain();
   //bTest &= ct.IsParent_Test();

   //AddressTest at;
   //bTest &= at.InceptLocationTest();
   //bTest &= at.IsResidentInTest();
   //bTest &= at.ParseTestManySub();
   //bTest &= at.ParseTestSingle();
   //bTest &= at.PitheLocationTest();

   //CollectionTest clt;
   //bTest &= clt.AddItem_Test();
   //bTest &= clt.RemoveItem_Test();
   //bTest &= clt.AddItemFrom_Test();
   //bTest &= clt.RemoveItem_OtherCollectionsRef_Test();

   return bTest;
}

bool
StoreFront::ConfigIsLoaded()
{
   return Config::Instance()->IsLoaded();
}

void 
StoreFront::SaveCollection(const string& aszCollectionID)
{
   m_ColFactory->SaveCollection(aszCollectionID);
}

void 
StoreFront::ExportCollection( const string& aszCollectionName, Query aQuery )
{
   m_ColFactory->ExportCollection( aszCollectionName, aQuery );
}

string 
StoreFront::LoadCollection(const string& aszCollectionFile)
{
   return m_ColFactory->LoadCollectionFromFile(aszCollectionFile);
}

string 
StoreFront::CreateNewCollection( const string& aszCollectionName,
                                 const string& aszParent )
{
   return m_ColFactory->CreateNewCollection(aszCollectionName, aszParent);
}

vector<string> 
StoreFront::GetLoadedCollections()
{
   return m_ColFactory->GetLoadedCollections();
}

// Requires Collection ID
string 
StoreFront::GetCollectionName(const string& aszCollectionID)
{
   if( m_ColFactory->CollectionExists(aszCollectionID) )
   {
      return m_ColFactory->GetCollection(aszCollectionID)->GetName();
   }
   else
   {
      return Config::NotFoundString;
   }
}

string 
StoreFront::GetCollectionID(const string& aszCollectionName)
{
   return m_ColFactory->GetCollectionID(aszCollectionName);
}

vector<string> 
StoreFront::GetAllCardsStartingWith( const string& aszColID, 
                                     const Query& aszSearch )
{
   if( m_ColFactory->CollectionExists(aszColID) )
   {
      return m_ColFactory->GetCollection(aszColID)->
         QueryCollection(aszSearch);
   }
   return vector<string>();
}

map<unsigned long, vector<string>>
StoreFront::GetHistoryLines( const string& aszColID, unsigned int aiStart )
{
   if( m_ColFactory->CollectionExists( aszColID ) )
   {
      return m_ColFactory->GetCollection( aszColID )->GetHistoryLines(aiStart, -1);
   }
   else
   {
      return map<unsigned long, vector<string>>();
   }
}

map<unsigned long, vector<string>>
StoreFront::GetHistoryLines( const string& aszColID, unsigned int aiStart, unsigned int aiEnd )
{
   if( m_ColFactory->CollectionExists( aszColID ) )
   {
      return m_ColFactory->GetCollection( aszColID )->GetHistoryLines( aiStart, aiEnd );
   }
   else
   {
      return map<unsigned long, vector<string>>();
   }
}

vector<pair<string, string>> 
StoreFront::GetPeekValues( const string& aszColID )
{
   return m_ColFactory->PeekCollection( aszColID );
}

void
StoreFront::SetAttribute( const string& aszCardName, const string& aszUID,
                          const string& aszKey, const string& aszVal )
{
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         copy.Value()->SetAttribute( aszKey, aszVal );
      }
   }
}

vector<pair<string, string>>
StoreFront::GetMetaTags(const string& aszCardName, const string& aszUID)
{
   vector<pair<string, string>> vecRetval;
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         vecRetval = copy.Value()->GetMetaTags(MetaTag::Type::Any);
      }
   }

   return vecRetval;
}

string 
StoreFront::GetMetaTag( const string& aszCardName, const string& aszUID, 
                        const string& aszTagKey )
{
   auto item = m_ColSource->GetCardPrototype( aszCardName );
   if( item.Good() )
   {
      auto copy = item->FindCopy( aszUID );
      if( copy.Good() )
      {
         return copy->GetMetaTag( aszTagKey, MetaTag::Type::Public );
      }
   }

   return "";
}

string 
StoreFront::GetCommonAttribute(const string& aszCardName, const string& aszAttribute)
{
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      return item->GetCommonTrait(aszAttribute);
   }

   return "";
}

string 
StoreFront::GetIdentifyingAttribute( const string& aszCardName, 
                                     const string& aszUID, 
                                     const string& aszTrait )
{
   auto item = m_ColSource->GetCardPrototype( aszCardName );
   if( item.Good() )
   {
      auto copy = item->FindCopy( aszUID );
      if( copy.Good() )
      {
         return copy->GetAttribute(aszTrait);
      }
   }

   return "";
}

vector<pair<string, string>>
StoreFront::GetIdentifyingAttributes( const string& aszCardName,
                                      const string& aszUID )
{
   vector<pair<string, string>> vecRetval;
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         vecRetval = copy->GetIdentifyingAttributes();
      }
   }

   return vecRetval;
}

map<string, vector<string>> 
StoreFront::GetIdentifyingAttributeOptions(const string& aszCardName)
{
   map<string, vector<string>> mapRetVal;
   auto card = m_ColSource->GetCardPrototype(aszCardName);
   if( card.Good() )
   {
      auto vecAttrID = card->GetIdentifyingTraits();
      for( auto& trait : vecAttrID )
      {
         vector<string> vecOptions = vector<string>( trait.second.GetAllowedValues().begin(), 
                                                     trait.second.GetAllowedValues().end() );
         auto pairItem = make_pair(trait.first, vecOptions);
         mapRetVal.insert(pairItem);
      }
   }
   return mapRetVal;
}

string 
StoreFront::GetDefaultIdentifyingAttributeValue( const string& aszCardName, const string aszKey )
{
   auto card = m_ColSource->GetCardPrototype( aszCardName );
   if( card.Good() )
   {
      auto vecAttrID = card->GetIdentifyingTraits();
      auto iter_keyTrait = vecAttrID.find( aszKey );
      if( iter_keyTrait != vecAttrID.end() )
      {
         return iter_keyTrait->second.GetDefaultValue();
      }
   }
   return "";
}

string
StoreFront::GetCardString(const string& aszCardname, const string& aszUID)
{
   auto item = m_ColSource->GetCardPrototype(aszCardname);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         // TODO:
         return copy->ToString();
      }
   }

   return "";
}

string 
StoreFront::GetMetaTagHash( const string& aszCardName, const string& aszUID )
{
   string szRetVal;
   auto item = m_ColSource->GetCardPrototype( aszCardName );
   if( item.Good() )
   {
      auto copy = item->FindCopy( aszUID );
      if( copy.Good() )
      {
         szRetVal = copy->GetMetaTag( MetaTag::GetHashKey(), MetaTag::Type::Any );
      }
   }

   return szRetVal;
}

vector<string>
StoreFront::GetAllCardsStartingWith(const Query& aszSearch)
{
   return m_ColSource->GetAllCardsStartingWith(aszSearch);
}

vector<pair<string, string>>
StoreFront::GetPairedAttributes()
{
   return Config::Instance()->GetPairedKeysList();
}

string 
StoreFront::GetImagesDirectory()
{
   return Config::Instance()->GetImagesDirectory();
}

string 
StoreFront::GetCollectionsDirectory()
{
   return Config::Instance()->GetCollectionsDirectory();
}

string 
StoreFront::GetCollectionsMetaDirectory()
{
   return Config::Instance()->GetCollectionsMetaDirectory();
}

string 
StoreFront::GetCollectionsHistoryDirectory()
{
   return Config::Instance()->GetCollectionsHistoryDirectory();
}

string 
StoreFront::GetCollectionsOverheadDirectory()
{
   return Config::Instance()->GetCollectionsOverheadDirectory();
}

string 
StoreFront::GetImportSourceFileName()
{
   return Config::Instance()->GetImportSourceFileName();
}

string 
StoreFront::GetSourceDirectory()
{
   return Config::Instance()->GetSourceDirectory();
}

string 
StoreFront::GetSourceFileName()
{
   return Config::Instance()->GetSourceFileName();
}

string 
StoreFront::GetSourceFilePath()
{
   return Config::Instance()->GetSourceFilePath();
}

string 
StoreFront::GetImportSourceFilePath()
{
   return Config::Instance()->GetImportSourceFilePath();
}

string 
StoreFront::GetConfigDirectory()
{
   return Config::Instance()->GetConfigDirectory();
}

string 
StoreFront::GetImageFilePath( const string& aszCardName, const string& aszSet )
{
   return Config::Instance()->GetImageFilePath( aszCardName, aszSet );
}

string 
StoreFront::CollapseCardLine(const string& aszCard, bool abIncludeCount)
{
   string szCard = aszCard;
   m_ColSource->CollapseCardLine(szCard, abIncludeCount);
   return szCard;
}

void 
StoreFront::SubmitBulkChanges( const string& aszCollection,
                               vector<string> alstChanges )
{
   if( m_ColFactory->CollectionExists(aszCollection) )
   {
      m_ColFactory->GetCollection(aszCollection)->LoadChanges(alstChanges);
   }
}

void 
StoreFront::ImportCollectionSource()
{
   JSONImporter JI;
   JI.ImportJSON(Config::Instance()->GetImportSourceFilePath());
   m_ColSource->HotSwapLib(Config::Instance()->GetSourceFilePath());
}
