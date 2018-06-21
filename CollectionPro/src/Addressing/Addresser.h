#pragma once
#include<vector>
#include<string>
#include<set>

class Addresser
{
public:
   // TODO: move this somewhere more appropriate.
   static int GetRandom();

private:
   static unsigned int ms_iRandom;
};

class ISubAddress
{
public:
   // -2 not related, -1 this is a subset, 0 same, 1 this is a superset.
   virtual int Compare( const ISubAddress& other ) const = 0;
   virtual std::string ToString() const = 0;
   virtual void Push( const std::string& aszNextStep ) = 0;
   virtual std::string Pop( ) = 0;
   // Required by std::set
   virtual bool operator<( const ISubAddress& rhs ) const = 0;
};

// Maintains a list of numbers indicating which child of the parent it refers to
// E.g. Name-0101 is the first child of the first child of name. 0101 = [01,01]
class SubAddress : public ISubAddress
{
public:
   SubAddress() {}
   SubAddress( const std::string& aszBranch );
   ~SubAddress() {};

   // -2 not related, -1 this is a subset, 0 same, 1 this is a superset.
   int Compare( const ISubAddress& other ) const;
   std::string ToString() const;
   virtual void Push( const std::string& aszNextStep );
   virtual std::string Pop( );

   // Required by std::set
   bool operator<( const ISubAddress& rhs ) const;
private:
   std::string m_szBranch;

public:
   static std::vector<std::string> ParseLeafString( const std::string& aszTree );
   static std::string ToLeafString( const std::vector<std::string>& aszSubAddresses );
   static std::string CleanLeafString( const std::string& aszTree );
};
typedef SubAddress SubAddress_t;

class Address;
class Location;
class Identifier
{
public:
   Identifier();
   Identifier(const std::string& aszId);
   ~Identifier();

   virtual std::string GetLeafString() const = 0;
   virtual std::set<SubAddress_t> GetLeaves() const = 0;

   // TODO: Get rid of this function
   std::set<Location> GetLocations() const;
   
   // TODO: Id like to get rid of this function too!
   bool IsEmpty() const;
   // -2 not related, -1 this is a subset, 0 same, 1 this is a superset.
   int Compare( const Identifier& other ) const;
   std::string GetMain() const;
   void SetMain( const std::string& aszMain );
   std::string ToString() const;

   // Returns true if the input set is a superset of ANY
   // of the sets held by this class.
   bool IsSuperset( const Identifier& aSet ) const;
   // See above.
   bool IsPartOfRootPath( const Identifier& aSet ) const;

   // Required by std::set
   bool operator<( const Identifier& rhs ) const;

protected:
   std::string m_szMain;
};

class Location;
class Address : public Identifier
{
public:
   Address();
   Address(const std::string& aszId);
   ~Address();

   std::string GetLeafString() const override;
   std::set<SubAddress_t> GetLeaves() const override;

   bool AddLeaf( SubAddress_t aiSub);
   int RemoveLeaf( SubAddress_t aiSub);

   int SetSubAddress( SubAddress_t aiAlreadySub, SubAddress_t aiSub);
   bool MergeIdentifier(const Identifier& aID);
   bool ExtractIdentifier(const Identifier& aID);
private:
   std::set<SubAddress_t> m_setLeaves;
};

class Location : public Identifier
{
public:
   Location();
   Location(const std::string& aszId);
   Location( const std::string& aszId, SubAddress_t aSub );
   ~Location();

   std::set<Location> GetRootPath() const;
   std::string GetLeafString() const override;
   std::set<SubAddress_t> GetLeaves() const override;

   SubAddress_t GetLeaf() const;

private:
   SubAddress_t m_Leaf;
};

class AddresserTest
{
public:
   static bool Test();
};