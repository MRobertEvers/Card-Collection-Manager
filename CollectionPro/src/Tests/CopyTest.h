#pragma once

#include<memory>

// Tests the copy object class
class CollectionObject;
class CopyItem;


class CopyTest
{
public:
   CopyTest();
   ~CopyTest();

   bool CreateCopy_Test();
   bool SetMetaTag_Test();
   bool Hash_Test();
   bool SetParent_Test();
   bool IsParent_Test();
   // Set attribute test

   bool ResidentIn_Parent_ParentIsResident_Test();
   bool ResidentIn_Parent_ParentIsNotResident_Test();
   bool ResidentIn_ChainOfParent_Test();
   bool ResidentIn_NotParent_AddedResident_Test();
   bool ResidentIn_NotParent_ChainOfAddedResident_Test();
   bool AddResident_InParent_AlreadyDesignated_Test();
   bool AddResident_InParent_ExistingChain_NotDesignatedByParentChain_Test();
   bool AddResident_InParent_NewChain_NotDesignatedByParentChain_Test();
   bool AddResident_InResidentNotParent_AlreadyDesignated_Test();
   bool AddResident_InResidentNotParent_ExistingChain_NotDesignated_Test();
   bool AddResident_InResidentNotParent_NewChain_NotDesignated_Test();
   bool RemoveResident_NotParent_InChainOfResident_NotEntireChain_Test();
   bool RemoveResident_NotParent_InChainOfResident_EntireChain_Test();
   bool RemoveResident_NotParent_NotInChainOfResident_Test();
   bool RemoveResident_Parent_InChainOfParent_NotEntireChain();
   bool RemoveResident_Parent_InChainOfParent_EntireChain();

private:
   CollectionObject getTestCollectionItem();
   std::shared_ptr<CopyItem> getTestCopy();
};

