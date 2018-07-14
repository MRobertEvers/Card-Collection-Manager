#pragma once
#include <wx/wxprec.h>
#include <map>
#include <memory>
#include <string>

class IGroupItem
{
public:
   virtual std::string GetName() const = 0;
   virtual std::string GetMetaTag( const std::string& aszKey ) const = 0;
   virtual std::string GetAttribute( const std::string& aszKey ) const = 0;
};

class Group
{
public:
   class SortingOperator
   {
   public:
      virtual bool operator()( const wxString& agrpLeft, const wxString& agrpRight ) const;
   };

   class Sorting
   {
   public:
      Sorting();
      Sorting( SortingOperator* aptSorting );
      virtual bool operator()( const wxString& agrpLeft, const wxString& agrpRight ) const;
   private:
      std::shared_ptr<SortingOperator> m_ptSorter;
   };

   class ItemSorting
   {
   public:
      ItemSorting();
      ItemSorting( SortingOperator* aptSorting );

      template<typename T>
      bool operator()( const T agrpLeft, const T agrpRight ) const
      {
         return (*m_ptSorter)(agrpLeft->GetName(), agrpRight->GetName());
      }

   private:
      std::shared_ptr<SortingOperator> m_ptSorter;
   };


   Group(bool abIsEmpty = false);
   // Not used in grouping.
   wxString GetParentGroupName();
   Group& SetSubGroupOf( const wxString& aszName );
   Group& GroupOn( const wxString& aszKey, bool abIsMetaKey = true );

   // Applied last. Any group matching aszGroup exactly will appear as aszAlias.
   Group& AliasGroup( const wxString& aszGroup, const wxString& aszAlias );

   // Any group containing this value will appear AS this value.
   Group& BroadenGroup( const wxString& aszValue );
   Group& OverrideGrouping( const Group& aGrouping );
   Group& SetDefaultSubGroup( const Group& aGrouping );
   Group& AddSubGroup( const wxString& aszMajorGroup, Group& aGrouping );
   Group& SetGroupSortingFunctor( SortingOperator* aptFunctor );
   Group& SetItemSortingFunctor( SortingOperator* aptFunctor );

   wxString GetGroup( const IGroupItem& aData ) const;
   Group GetSubGroup( const wxString& aszGroup ) const;
   Group GetSubGroup( const IGroupItem& aData ) const;
 
   std::shared_ptr<Sorting> GetSortingFunctor() const;
   std::shared_ptr<ItemSorting> GetItemSortingFunctor() const;
   bool IsEmpty() const;

private:
   wxString ParentGroupName;

   wxString Key;
   bool MetaKey;
   bool BIsEmpty;
   std::vector<wxString> BroadenedValues;
   std::map<wxString, wxString> Aliases;
   std::map<wxString, Group> SubGroups;
   std::shared_ptr<Group> DefaultSubGroup;
   std::vector<Group> Overrides;
   std::shared_ptr<Sorting> SortingFunctor;
   std::shared_ptr<ItemSorting> ItemSortingFunctor;
};