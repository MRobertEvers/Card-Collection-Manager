#pragma once
#include <memory>
#include <vector>
#include <string>

class CollectionInterface;
class CardInterface;
class VCardAttributesEditor;
class CCardAttributesEditor
{
public:
   CCardAttributesEditor( VCardAttributesEditor* apView, int aiMaxSelection = -1 );
   ~CCardAttributesEditor();

   void SetModel( CardInterface* aptModel );

   void SelectUID( const std::string& aszUID );
   void UnselectUID( const std::string& aszUID );

   int GetSelectedNum();
   std::vector<std::string> GetSelected();
   void DeselectOne( const std::string& aszUID = "" );

   void OnSave();

private:
   VCardAttributesEditor* m_pView;
   CardInterface* m_pModel;
   int m_iMaxSelection;
   std::vector<std::string> m_vecSelectedUIDs;
};