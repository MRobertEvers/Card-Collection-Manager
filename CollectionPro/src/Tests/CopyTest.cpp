#include "CopyTest.h"
#include "../Collection/CollectionObject.h"
#include "../Support/Config.h"

using namespace std;

CopyTest::CopyTest()
{
}


CopyTest::~CopyTest()
{
}

bool 
CopyTest::CreateCopy_Test()
{
   bool bResult = true;
   CollectionObject testItem = getTestCollectionItem();

   /****************************************************/
   /*Test 1: Parent Not Overridden Until after creation*/
   /****************************************************/
   Location testAddr("IDTest-6");
   std::vector<Tag> vecAttrs;
   vecAttrs.push_back(make_pair("Collar", "red"));
   std::vector<Tag> vecMeta;
   auto ptTestCopy = testItem.AddCopy(testAddr, vecAttrs, vecMeta);
   
   bResult &= ptTestCopy->GetIdentifyingAttribute("Collar")=="red";
   bResult &= ptTestCopy->GetParent()==testAddr.GetFullAddress();
   bResult &= ptTestCopy->GetUID() != ""; // Should have create a UID.
   bResult &= ptTestCopy->GetResidentIn().size() == 1 &&
              ptTestCopy->GetResidentIn()[0].GetFullAddress() == testAddr.GetFullAddress();

   /***************************************/
   /*Test 2: Parent Overridden at creation*/
   /***************************************/
   // Perform create with "Parent" overriden in the metatags.
   Location testAddr2("IDTest2-1");
   vecMeta.push_back(make_pair(CopyItem::GetAddressKey(),testAddr2.GetFullAddress()));
   vecMeta.push_back(make_pair("test","val"));
   vecMeta.push_back(make_pair("test2","val2"));
   ptTestCopy = testItem.AddCopy(testAddr, vecAttrs, vecMeta);

   // Overriding the parent in construction does NOT add to resident.
   bResult &= ptTestCopy->GetParent()==testAddr2.GetFullAddress();
   bResult &= ptTestCopy->GetResidentIn().size() == 1 &&
              ptTestCopy->GetResidentIn()[0].GetFullAddress() == testAddr.GetFullAddress();
   bResult &= ptTestCopy->GetMetaTag("test", Public) == "val";
   bResult &= ptTestCopy->GetMetaTag("tests", MetaTagType::Any) == Config::NotFoundString;
   bResult &= ptTestCopy->GetMetaTag("test", Hidden) == ""; // "" indicates that we are not allowed to view the tag.
   bResult &= ptTestCopy->GetHash() != ""; // Should've created a hash.

   /***************************************/
   /*Test 3: Default IDs added at creation*/
   /***************************************/
   // Perform create with "Parent" overriden in the metatags.
   vecAttrs.clear();
   ptTestCopy = testItem.AddCopy(testAddr, vecAttrs, vecMeta);

   bResult &= ptTestCopy->GetIdentifyingAttribute("Collar")=="blue";

   return bResult;
}

bool 
CopyTest::SetMetaTag_Test()
{
   bool bResult = true;
   CollectionObject testItem = getTestCollectionItem();

   Location testAddr("IDTest-6");
   std::vector<Tag> vecAttrs;
   vecAttrs.push_back(make_pair("Collar", "red"));
   std::vector<Tag> vecMeta;
   vecMeta.push_back(make_pair(CopyItem::GetAddressKey(),testAddr.GetFullAddress()));
   vecMeta.push_back(make_pair("test","val"));
   vecMeta.push_back(make_pair("test2","val2"));
   vecMeta.push_back(make_pair(CopyItem::MakeIgnoredTag("beat"),"val"));
   vecMeta.push_back(make_pair(CopyItem::MakeTrackingTag("beats"),"val"));
   auto ptTestCopy = testItem.AddCopy(testAddr, vecAttrs, vecMeta);

   // Meta Tags added in construction.
   bResult &= ptTestCopy->GetMetaTag("test", Public) == "val";
   bResult &= ptTestCopy->GetMetaTag("test2", Public) == "val2";
   bResult &= ptTestCopy->GetMetaTag(CopyItem::MakeIgnoredTag("beat"), Ignored) == "val";
   bResult &= ptTestCopy->GetMetaTag(CopyItem::MakeTrackingTag("beats"), Tracking) == "val";
   bResult &= ptTestCopy->GetMetaTag(CopyItem::MakeIgnoredTag("beat"), Hidden) == "";
   bResult &= ptTestCopy->GetMetaTag(CopyItem::MakeTrackingTag("beats"), Visible) == "";

   // Meta Tag Addition
   ptTestCopy->SetMetaTag("newTest", "firstVal", Public);
   bResult &= ptTestCopy->GetMetaTag("newTest", Public) == "firstVal";

   // Meta Tag Changing
   ptTestCopy->SetMetaTag("newTest", "newVal", Public);
   bResult &= ptTestCopy->GetMetaTag("newTest", Public) == "newVal";

   // Meta Tag Removal
   ptTestCopy->SetMetaTag("newTest","",Public);
   bResult &= ptTestCopy->GetMetaTag("newTest", Public) == Config::NotFoundString;

   // Wrong Accessor 
   bResult &= ptTestCopy->GetMetaTag("test", MetaTagType::Hidden) == "";
   bResult &= ptTestCopy->GetMetaTag("test", MetaTagType::Tracking) == "";
   bResult &= ptTestCopy->GetMetaTag("test", MetaTagType::Ignored) == "";

   // Wrong Type for setting (should do nothing)
   ptTestCopy->SetMetaTag("test", "notVal", Hidden);
   bResult &= ptTestCopy->GetMetaTag("test", Public) == "val";

   return bResult;
}

bool 
CopyTest::Hash_Test()
{
   bool bResult = true;
   CollectionObject testItem = getTestCollectionItem();

   Location testAddr("IDTest-6");
   std::vector<Tag> vecAttrs;
   vecAttrs.push_back(make_pair("Collar", "red"));
   std::vector<Tag> vecMeta;
   vecMeta.push_back(make_pair("test","val"));
   vecMeta.push_back(make_pair("test2","val2"));
   auto ptTestCopy = testItem.AddCopy(testAddr, vecAttrs, vecMeta);
   
   vecMeta.clear();
   vecMeta.push_back(make_pair("test2","val2"));
   // This line tests whether overriding a parent with itself does anything.
   vecMeta.push_back(make_pair(CopyItem::GetAddressKey(),testAddr.GetFullAddress()));
   vecMeta.push_back(make_pair("test","val"));
   auto ptTestCopy2 = testItem.AddCopy(testAddr, vecAttrs, vecMeta);

   string szTargetHash = testItem.GenerateHash(ptTestCopy->GetAddress(), vecAttrs, vecMeta);

   // Ensure that order in which meta tags are added does not change the hash.
   bResult &= ptTestCopy->GetHash() == szTargetHash; 
   bResult &= ptTestCopy2->GetHash() == szTargetHash;

   return bResult;
}

bool 
CopyTest::SetParent_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();
   Address testAddr = ptTestCopy->GetAddress();

   bResult &= ptTestCopy->GetParent()==testAddr.GetFullAddress();
   bResult &= ptTestCopy->GetResidentIn().size() == 1 &&
              ptTestCopy->GetResidentIn()[0].GetFullAddress() == testAddr.GetFullAddress();

   Location testAddr2("IDTest2-9");
   ptTestCopy->SetParent(testAddr2);
   bResult &= ptTestCopy->GetParent()==testAddr2.GetFullAddress();
   bResult &= ptTestCopy->GetResidentIn().size() == 2 &&
              ptTestCopy->GetResidentIn()[0].GetFullAddress() == testAddr.GetFullAddress() &&
              ptTestCopy->GetResidentIn()[1].GetFullAddress() == testAddr2.GetFullAddress();

   return bResult;
}

bool 
CopyTest::IsParent_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();
   Location testAddr(ptTestCopy->GetAddress().GetFullAddress());

   // The parent should be the parent...
   bResult &= ptTestCopy->IsParent(testAddr);

   // Check if a superset of the parent is a parent
   Location testAddr2("IDTest-2");
   bResult &= ptTestCopy->IsParent(testAddr2);

   // Check if a subset of the parent is a parent (false)
   Location testAddr3("IDTest-18");
   bResult &= !ptTestCopy->IsParent(testAddr3);

   // Check if a subset of the parent is a parent (false)
   Location testAddr4("TTTT-6");
   bResult &= !ptTestCopy->IsParent(testAddr4);

   return bResult;
}

bool 
CopyTest::ResidentIn_Parent_ParentIsResident_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();
   Location parentAddress(ptTestCopy->GetAddress().GetFullAddress());

   // Check we are both referenced by and resident in the parent.
   bResult &= ptTestCopy->IsResidentIn(parentAddress);
   bResult &= ptTestCopy->IsReferencedBy(parentAddress);
   return bResult;
}

bool 
CopyTest::ResidentIn_Parent_ParentIsNotResident_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();
   Location parentAddress(ptTestCopy->GetAddress().GetFullAddress());

   // Check we are both referenced by and resident in the parent.
   bResult &= ptTestCopy->IsResidentIn(parentAddress);
   bResult &= ptTestCopy->IsReferencedBy(parentAddress);
   return bResult;
}

bool 
CopyTest::ResidentIn_ChainOfParent_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location chainOfParentAddress("IDTest-2");
   // Check we are both referenced by and resident in the parent.
   bResult &= ptTestCopy->IsResidentIn(chainOfParentAddress);
   bResult &= ptTestCopy->IsReferencedBy(chainOfParentAddress);
   return bResult;
}

bool 
CopyTest::ResidentIn_NotParent_AddedResident_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location newResident("ResTest-18");
   ptTestCopy->AddResident(newResident);

   bResult &= ptTestCopy->IsResidentIn(newResident);
   bResult &= ptTestCopy->IsReferencedBy(newResident);
   return bResult;
}

bool
CopyTest::ResidentIn_NotParent_ChainOfAddedResident_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location newResident("ResTest-18");
   ptTestCopy->AddResident(newResident);

   Location chainOfNewResident("ResTest-6");

   bResult &= ptTestCopy->IsResidentIn(chainOfNewResident);
   bResult &= ptTestCopy->IsReferencedBy(chainOfNewResident);
   return bResult;
}

bool 
CopyTest::AddResident_InParent_AlreadyDesignated_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location alreadyDesignatedByParent("IDTest-2");

   ptTestCopy->AddResident(alreadyDesignatedByParent);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetParent() == "IDTest-6";
   return bResult;
}

bool 
CopyTest::AddResident_InParent_ExistingChain_NotDesignatedByParentChain_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location addressInParentChainNotDesignated("IDTest-18");

   ptTestCopy->AddResident(addressInParentChainNotDesignated);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetParent() == "IDTest-18";
   return bResult;
}

bool 
CopyTest::AddResident_InParent_NewChain_NotDesignatedByParentChain_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inParentNewChain("IDTest-3");

   ptTestCopy->AddResident(inParentNewChain);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetParent() == "IDTest-6,3";
   return bResult;
}

bool 
CopyTest::AddResident_InResidentNotParent_AlreadyDesignated_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location residentAddress("ResTest-9");
   ptTestCopy->AddResident(residentAddress);

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location alreadyDesignatedByResident("ResTest-3");

   ptTestCopy->AddResident(alreadyDesignatedByResident);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetResidentIn().size() == 2 &&
              ptTestCopy->GetResidentIn()[1].GetFullAddress() == "ResTest-9";
   return bResult;
}

bool 
CopyTest::AddResident_InResidentNotParent_ExistingChain_NotDesignated_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location residentAddress("ResTest-9");
   ptTestCopy->AddResident(residentAddress);

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inExistingResidentChain("ResTest-27");

   ptTestCopy->AddResident(inExistingResidentChain);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetResidentIn().size() == 2 &&
              ptTestCopy->GetResidentIn()[1].GetFullAddress() == "ResTest-27";
   return bResult;
}

bool 
CopyTest::AddResident_InResidentNotParent_NewChain_NotDesignated_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location residentAddress("ResTest-9");
   ptTestCopy->AddResident(residentAddress);

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inResidentNewChain("ResTest-15");

   ptTestCopy->AddResident(inResidentNewChain);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetResidentIn().size() == 2 &&
              ptTestCopy->GetResidentIn()[1].GetFullAddress() == "ResTest-9,15";
   return bResult;
}

bool 
CopyTest::RemoveResident_NotParent_InChainOfResident_NotEntireChain_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location residentAddress("ResTest-9");
   ptTestCopy->AddResident(residentAddress);

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inResidentNotEntireCahin("ResTest-9");

   ptTestCopy->RemoveResident(inResidentNotEntireCahin);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetResidentIn().size() == 2 &&
              ptTestCopy->GetResidentIn()[1].GetFullAddress() == "ResTest-3";
   return bResult;
}

bool 
CopyTest::RemoveResident_NotParent_InChainOfResident_EntireChain_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location residentAddress("ResTest-9");
   ptTestCopy->AddResident(residentAddress);

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inResidentEntireCahin("ResTest-1");

   ptTestCopy->RemoveResident(inResidentEntireCahin);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount-1;
   bResult &= ptTestCopy->GetResidentIn().size() == 1;
   return bResult;
}

bool 
CopyTest::RemoveResident_NotParent_NotInChainOfResident_Test()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   Location residentAddress("ResTest-9");
   ptTestCopy->AddResident(residentAddress);

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location notInResident("ResTest-6");

   ptTestCopy->RemoveResident(notInResident);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetResidentIn().size() == 2 &&
              ptTestCopy->GetResidentIn()[1].GetFullAddress() == "ResTest-9";
   return bResult;
}

bool 
CopyTest::RemoveResident_Parent_InChainOfParent_NotEntireChain()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inParent("IDTest-6");

   ptTestCopy->RemoveResident(inParent);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount;
   bResult &= ptTestCopy->GetResidentIn().size() == 1 &&
              ptTestCopy->GetResidentIn()[0].GetFullAddress() == "IDTest-2";
   return bResult;
}

bool 
CopyTest::RemoveResident_Parent_InChainOfParent_EntireChain()
{
   bool bResult = true;
   auto ptTestCopy = getTestCopy();

   int iResidentCount, iNewResidentCount;
   iResidentCount = ptTestCopy->GetResidentIn().size();
   Location inParent("IDTest-1");

   ptTestCopy->RemoveResident(inParent);
   iNewResidentCount = ptTestCopy->GetResidentIn().size();

   bResult &= iNewResidentCount == iResidentCount-1;
   bResult &= ptTestCopy->GetResidentIn().size() == 0;
   bResult &= ptTestCopy->IsVirtual();
   return bResult;
}

CollectionObject 
CopyTest::getTestCollectionItem()
{
   // For this, I don't want to be dependent on outside files
   // Create a custom item.
   std::vector<Tag> vecCommonTags;
   vecCommonTags.push_back(make_pair("set", "BFZ"));
   vecCommonTags.push_back(make_pair("height", "5"));
   vecCommonTags.push_back(make_pair("degree", "Good"));

   std::vector<TraitItem> vecRestrictedTraits;
   TraitItem traitOne( "Collar", { "blue", "red" } );
   vecRestrictedTraits.push_back(traitOne);

   return CollectionObject( "TestItem", vecCommonTags, vecRestrictedTraits);
}

// Returns a copy item of the testCollectionObject, with parent and resident IDTest-6.
shared_ptr<CopyItem> 
CopyTest::getTestCopy()
{
   std::vector<Tag> vecAttrs, vecMeta;

   CollectionObject testItem = getTestCollectionItem();

   Location parentAddress("IDTest-6");
   return testItem.AddCopy(parentAddress, vecAttrs, vecMeta);
}