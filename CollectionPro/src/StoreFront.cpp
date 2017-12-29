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
#include "Support\Config.h"

StoreFront::StoreFront()
{
   //SelfTest();

   // No Server for now
   m_ColSource = new CollectionSource();
   m_ColSource->LoadLib(Config::Instance()->GetSourceFile());

   m_ColFactory = new CollectionFactory(m_ColSource);
}


StoreFront::~StoreFront()
{
}

bool
StoreFront::SelfTest()
{
   bool bTest = true;
   CollectionItemTest cit;
   bTest &= cit.AddCopy_Test();
   bTest &= cit.RemoveCopy_EntireChain_Test();
   bTest &= cit.RemoveCopy_PartialChain_Test();
   bTest &= cit.FindCopies_All_Test();
   bTest &= cit.FindCopies_Virtual_Test();
   bTest &= cit.FindCopies_Borrowed_Test();
   bTest &= cit.FindCopies_Local_Test();

   CopyTest ct;
   bTest &= ct.CreateCopy_Test();
   bTest &= ct.SetMetaTag_Test();
   bTest &= ct.Hash_Test();
   bTest &= ct.SetParent_Test();
   bTest &= ct.AddResident_InParent_AlreadyDesignated_Test();
   bTest &= ct.AddResident_InParent_ExistingChain_NotDesignatedByParentChain_Test();
   bTest &= ct.AddResident_InParent_NewChain_NotDesignatedByParentChain_Test();
   bTest &= ct.AddResident_InResidentNotParent_AlreadyDesignated_Test();
   bTest &= ct.AddResident_InResidentNotParent_ExistingChain_NotDesignated_Test();
   bTest &= ct.AddResident_InResidentNotParent_NewChain_NotDesignated_Test();
   bTest &= ct.ResidentIn_ChainOfParent_Test();
   bTest &= ct.ResidentIn_NotParent_AddedResident_Test();
   bTest &= ct.ResidentIn_NotParent_ChainOfAddedResident_Test();
   bTest &= ct.ResidentIn_Parent_ParentIsNotResident_Test();
   bTest &= ct.ResidentIn_Parent_ParentIsResident_Test();
   bTest &= ct.ResidentIn_ChainOfParent_Test();
   bTest &= ct.RemoveResident_NotParent_InChainOfResident_EntireChain_Test();
   bTest &= ct.RemoveResident_NotParent_InChainOfResident_NotEntireChain_Test();
   bTest &= ct.RemoveResident_NotParent_NotInChainOfResident_Test();
   bTest &= ct.RemoveResident_Parent_InChainOfParent_EntireChain();
   bTest &= ct.RemoveResident_Parent_InChainOfParent_NotEntireChain();
   bTest &= ct.IsParent_Test();

   AddressTest at;
   bTest &= at.InceptLocationTest();
   bTest &= at.IsResidentInTest();
   bTest &= at.ParseTestManySub();
   bTest &= at.ParseTestSingle();
   bTest &= at.PitheLocationTest();

   CollectionTest clt;
   bTest &= clt.AddItem_Test();
   bTest &= clt.RemoveItem_Test();
   bTest &= clt.AddItemFrom_Test();
   bTest &= clt.RemoveItem_OtherCollectionsRef_Test();

   return bTest;
}

bool
StoreFront::ConfigIsLoaded()
{
   return Config::Instance()->IsLoaded();
}

void StoreFront::SaveCollection(std::string aszCollectionName)
{
   m_ColFactory->SaveCollection(aszCollectionName);
}

std::string StoreFront::LoadCollection(std::string aszCollectionFile)
{
   return m_ColFactory->LoadCollectionFromFile(aszCollectionFile);
}

std::string StoreFront::CreateNewCollection(std::string aszCollectionName, std::string aszParent)
{
   return m_ColFactory->CreateNewCollection(aszCollectionName, aszParent);
}

std::vector<std::string> StoreFront::GetLoadedCollections()
{
   return m_ColFactory->GetLoadedCollections();
}

std::vector<std::string> StoreFront::GetCollectionMetaData(std::string aszCollection)
{
   if( m_ColFactory->CollectionExists(aszCollection) )
   {
      return m_ColFactory->GetCollection(aszCollection)->GetMetaData();
   }
   else
   {
      std::vector<std::string> noRetval;
      return noRetval;
   }
}

std::vector<std::string>
StoreFront::GetCollectionList(std::string aszCollection, int aiVisibility)
{
   if( m_ColFactory->CollectionExists(aszCollection) )
   {
      if( aiVisibility < 0 )
      {
         return m_ColFactory->GetCollection(aszCollection)->GetShortList();
      }
      else
      {
         return m_ColFactory->GetCollection(aszCollection)->GetCollectionList((MetaTagType)aiVisibility);
      }
   }
   else
   {
      std::vector<std::string> lstEmpty;
      return lstEmpty;
   }
}

void
StoreFront::SetAttribute(string aszCardName, string aszUID, string aszKey, string aszVal)
{
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         item->SetIdentifyingTrait(copy.Value()->get(), aszKey, aszVal);
      }
   }
}

vector<pair<string, string>>
StoreFront::GetMetaTags(string aszCardName, string aszUID)
{
   vector<pair<string, string>> vecRetval;
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         vecRetval = copy->get()->GetMetaTags(MetaTagType::Any);
      }
   }

   return vecRetval;
}

vector<pair<string, string>>
StoreFront::GetIdentifyingAttributes(string aszCardName, string aszUID)
{
   vector<pair<string, string>> vecRetval;
   auto item = m_ColSource->GetCardPrototype(aszCardName);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         vecRetval = copy->get()->GetIdentifyingAttributes();
      }
   }

   return vecRetval;
}

string
StoreFront::GetCardString(string aszCardname, string aszUID)
{
   auto item = m_ColSource->GetCardPrototype(aszCardname);
   if( item.Good() )
   {
      auto copy = item->FindCopy(aszUID);
      if( copy.Good() )
      {
         return item->CopyToString(copy->get(), Any);
      }
   }

   return "";
}

std::string
StoreFront::GetCardPrototype(std::string aszCardName)
{
   int iValidCard = m_ColSource->LoadCard(aszCardName);
   if( iValidCard != -1 )
   {
      return m_ColSource->GetCardPrototype(iValidCard)->GetProtoType();
   }
   else
   {
      return "";
   }
}

std::vector<std::string>
StoreFront::GetAllCardsStartingWith(std::string aszSearch)
{
   return m_ColSource->GetAllCardsStartingWith(aszSearch);
}

vector<pair<string, string>>
StoreFront::GetPairedAttributes()
{
   return Config::Instance()->GetPairedKeysList();
}

std::string StoreFront::GetImagesPath()
{
   return Config::Instance()->GetImagesFolder();
}

string StoreFront::GetSourceFilePath()
{
   return Config::Instance()->GetSourceFile();
}

string StoreFront::GetImportSourceFilePath()
{
   return Config::Instance()->GetImportSourceFile();
}

void StoreFront::SubmitBulkChanges(std::string aszCollection, std::vector<std::string> alstChanges)
{
   if( m_ColFactory->CollectionExists(aszCollection) )
   {
      m_ColFactory->GetCollection(aszCollection)->LoadChanges(alstChanges);
   }
}

void StoreFront::ImportCollectionSource()
{
   JSONImporter JI;
   JI.ImportJSON(Config::Instance()->GetImportSourceFile());
   //m_ColSource->HotSwapLib(Config::Instance()->GetSourceFile());
}
