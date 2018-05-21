#pragma once
#include<vector>
#include<string>

class Addresser
{
public:
   // TODO: move this somewhere more appropriate.
   int GetRandom();

private:
   static unsigned int ms_iRandom;
};

class SubAddress
{
public:
   SubAddress() {}
   SubAddress( unsigned char aiAddress );
   SubAddress( std::vector<unsigned char> avecAdressPath );
   SubAddress( std::string aszPath );
   ~SubAddress() {};

   unsigned char GetLeastSignificantValue() { return Size() == 0 ? 0 : m_vecPath.back(); }
   bool IsSuperSetOf( const SubAddress& rhs );

   std::vector<unsigned char> GetPath( int aiStop = -1 ) const;
   std::string ToString();
   void Push( unsigned char aiNode );
   size_t Size() const { return m_vecPath.size(); }

   bool operator==( const SubAddress& rhs ) const;
   bool operator!=( const SubAddress& rhs ) const;
   bool operator<( const SubAddress& rhs ) const;

private:
   std::vector<unsigned char> m_vecPath;
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

   virtual bool IsEmpty() const;
   virtual std::vector<SubAddress_t> GetSubAddresses() const;
   virtual std::vector<Location> GetLocations() const;

   std::string GetMain() const;
   std::string GetFullAddress() const;
   Location GetBase() const;

   virtual Address ToAddress() const;
   
   bool operator==(const Identifier& rhs) const;
   bool operator!=(const Identifier& rhs) const;
   bool operator<(const Identifier& rhs) const;

protected:
   std::string m_szMain;
   std::vector<SubAddress_t> m_veciSubAddresses;

   void parseIdName(const std::string& aszID);
   int addSubAddress(std::vector<SubAddress_t>& avecSAs, SubAddress_t aiSA);
   bool isSuperSet( SubAddress_t aiSuper,
                    SubAddress_t aiSub ) const;

private:
   int compareSubAddress( SubAddress_t aiSOne,
                          SubAddress_t aiSTwo ) const;
};

class Location;
class Address : public Identifier
{
public:
   Address();
   Address(const std::string& aszId);
   ~Address();

   std::vector<SubAddress_t> GetSubAddresses() const override;

   bool ContainsLocation(const Location& aLoc) const;
   bool AddSubAddress( SubAddress_t aiSub);
   int RemoveSubAddress( SubAddress_t aiSub);
   int SetSubAddress( SubAddress_t aiAlreadySub, SubAddress_t aiSub);
   bool MergeIdentifier(const Identifier& aID);
   bool ExtractIdentifier(const Identifier& aID);
private:

};

class Location : public Identifier
{
public:
   Location();
   Location(const std::string& aszId);
   Location(const std::string& aszMain, SubAddress_t aiSA);
   ~Location();

   bool IsSpecifiedBy(const Address& aAddr) const;
   std::vector<SubAddress_t> GetSubAddresses() const override;
   std::vector<Location> GetLocationsSpecified() const;
   SubAddress_t GetSubAddress() const;

   Address ToAddress() const;

private:
   SubAddress_t m_iAddress;
};