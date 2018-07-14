#pragma once
#include <memory>
#include <vector>
#include <string>

class CollectionInterface;

class CardEditorModel
{
public:
   CardEditorModel( CollectionInterface* aptModel, std::vector<std::string> avecHashes );
   ~CardEditorModel();

   CollectionInterface* GetCollectionInterface();
   std::vector<std::string>& GetHashOptions();

private:
   CollectionInterface* m_pModel;
   std::vector<std::string> m_vecHashes;
};

class CardView;
class VCardEditor;
class CardAttributesEditor;
class CardInterface;
class CCardEditor
{
public:
   CCardEditor( VCardEditor* apView, int aiMaxSelection = -1 );
   ~CCardEditor();

   void SetModel( CollectionInterface* aptModel, std::vector<std::string> avecHashes );

   void Draw( CardInterface* apCard );

   void ShowWindow();

   std::vector<std::string> GetSelectedUIDs();

private:
   VCardEditor* m_pView;
   int m_iMaxSelection;
   std::shared_ptr<CardAttributesEditor> m_pAttributes;
   std::shared_ptr<CardView> m_ptCardViewer;
   std::shared_ptr<CardEditorModel> m_pModel;
   std::shared_ptr<CardInterface> m_pLastDrawnVirtual;

   void uiBuildView();
};