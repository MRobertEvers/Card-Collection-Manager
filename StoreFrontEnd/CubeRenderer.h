#pragma once
#include "Views\wxExtras\wxInfiniteGrid.h"
#include "StoreFrontEnd\Group.h"
#include "VCollectionView.h"
#include <wx/wxprec.h>
#include <wx/object.h> 
#include <wx/grid.h>
#include <memory>
#include <map>
#include <set>

class CardInterface;
class ColoredGroupColumnRenderer;

class CubeRenderer : public wxScrolledWindow, public GroupRenderer
{
public:
   CubeRenderer( wxPanel* aParent, wxWindowID aiWID );
   virtual ~CubeRenderer();

   void Draw( std::vector<CardInterface*> avecItemData );

   // Returns true if the item was successfully removed.
   virtual bool RemoveItem( CardInterface* aptItem );
   virtual void AddItem( CardInterface* aptItem );

   Group GetGrouping();

protected:
   virtual ColoredGroupColumnRenderer* uiGetColumnRenderer( const wxString& aszColumnName, const Group& aGroup );

private:
   void uiBuildColumns( std::vector<CardInterface*> avecItemData );
   void uiAddColumn( const wxString& aszColumnName,
                     std::vector<CardInterface*> avecItemData );
   void uiBuildGrouping();


   Group m_Grouping;
   std::map<wxString, ColoredGroupColumnRenderer*> m_mapColumns;
};

class ColumnRenderer
{
public:
   virtual void Draw( std::vector<CardInterface*> avecItemData ) = 0;

   virtual int DisplayItem( const wxString& buf, const wxString& aszHash,
                            const wxFont& awxFont,
                            bool abAlignCenter = false,
                            const wxColour& awxFontColor = wxColour( 78, 111, 47 ),
                            const wxColour& awxBackgroundColor = wxColour( 255, 255, 255 ),
                            int aiRow = -1,
                            unsigned int aiRowHeight = 0  ) = 0;

   virtual void UndisplayItem( int aiRow ) = 0;

   virtual wxColour GetBackgroundColor() = 0;
};

class ColoredGroupColumnRenderer : public wxInfiniteGrid, public GroupRenderer, public ColumnRenderer
{
public:
   ColoredGroupColumnRenderer( wxPanel* aParent, wxWindowID aiWID,
                               const Group& aGroup );
   virtual ~ColoredGroupColumnRenderer();

   virtual bool RemoveItem( CardInterface* aptItem ) = 0;
   virtual void AddItem( CardInterface* aptItem ) = 0;

   void SetTitle( const wxString& aszTitle );
   void SetBackgroundColor( const wxColour& aColour );
   void SetFontColor( const wxColour& aColour );

   void Draw( std::vector<CardInterface*> avecItemData ) = 0;

   virtual int DisplayItem( const wxString& buf, const wxString& aszHash,
                            const wxFont& awxFont, 
                            bool abAlignCenter = false, 
                            const wxColour& awxFontColor = wxColour( 78, 111, 47 ),
                            const wxColour& awxBackgroundColor = wxColour( 255, 255, 255 ),
                            int aiRow = -1,
                            unsigned int aiRowHeight = 0 );

   virtual void UndisplayItem( int aiRow );

   virtual wxColour GetBackgroundColor();
protected:
   wxColour m_GeneralFontColor;
   wxColour m_ColumnColour;
   wxString m_szGroupTitle;
   Group m_Group;
};

class DisplayGroup;
class DisplayNodeSource
{
public:
   virtual DisplayGroup* GetDisplayGroup( int aiType, Group aGroup,
                                          std::vector<CardInterface*> avecItems,
                                          DisplayGroup* aParent ) = 0;
};

class DisplayGroup
{
public:
   DisplayGroup( ColumnRenderer* apRenderer,
                 DisplayNodeSource* apSource,
                 Group aGroup,
                 std::vector<CardInterface*> avecItems,
                 DisplayGroup* aParent = nullptr );
   virtual ~DisplayGroup();

   bool RemoveItem( CardInterface* aptItem );
   void AddItem( CardInterface* aptItem );

   // Returns all overhead plus the size of children.
   virtual int GetTotalOverhead() = 0;
   virtual void Draw() = 0;
   virtual void UnDraw() = 0;

   virtual int GetSize();
   // Gets the first row that belongs to this group... not necessarily drawn.
   virtual int GetFirstRow();
   virtual int GetFirstItemRow();
   virtual bool IsEmpty();
   virtual bool IsLastChild();
   virtual bool IsFirstChild();
   virtual int GetLevel();

   Group m_Group;
   ColumnRenderer* m_pRenderer;
   DisplayGroup* m_Parent;
   DisplayNodeSource* m_pSource;
   std::map<CardInterface*, bool> m_mapDrawnItems;
   std::multiset<CardInterface*, Group::ItemSorting> m_setItems;

   // Delete the groups when done.
   std::map<wxString, DisplayGroup*, Group::Sorting> m_mapChildren;
};

class RootGroup : public DisplayGroup
{
public:
   RootGroup( ColumnRenderer* apRenderer,
              DisplayNodeSource* apSource,
              Group aGroup,
              std::vector<CardInterface*> avecItems,
              DisplayGroup* aParent = nullptr )
      : DisplayGroup(apRenderer, apSource, aGroup, avecItems, aParent){}
   virtual ~RootGroup() {}

   virtual int GetTotalOverhead() { return 0; }
   virtual void Draw();
   virtual void UnDraw() {};
};

class OrderedSubgroupColumnRenderer : public ColoredGroupColumnRenderer, public DisplayNodeSource
{
public:
   OrderedSubgroupColumnRenderer( wxPanel* aParent, wxWindowID aiWID, const Group& aGroup );
   virtual ~OrderedSubgroupColumnRenderer();

   void Draw( std::vector<CardInterface*> avecItemData );
   virtual bool RemoveItem( CardInterface* aptItem );
   virtual void AddItem( CardInterface* aptItem );

   DisplayGroup* GetDisplayGroup( int aiType, Group aGroup,
                                  std::vector<CardInterface*> avecItems,
                                  DisplayGroup* aParent );
   
protected:
   class TypeGroup : public DisplayGroup
   {
   public:
      TypeGroup( ColumnRenderer* apRenderer, 
                 DisplayNodeSource* apSource,
                 Group aGroup,
                 std::vector<CardInterface*> avecItems,
                 DisplayGroup* aParent = nullptr );

      virtual ~TypeGroup();

      virtual int GetSize();
      virtual void Draw();
      virtual void UnDraw();
      virtual int GetFirstItemRow();
      virtual int GetTotalOverhead();

   private:
      // This group draws an empty row after if it is not the last group.
      bool m_bHasDrawnLast;

      bool m_bHasDrawnHeader;
   };

   class CMCGroup : public DisplayGroup
   {
   public:
      CMCGroup( ColumnRenderer* apRenderer, 
                DisplayNodeSource* apSource,
                Group aGroup,
                std::vector<CardInterface*> avecItems,
                DisplayGroup* aParent = nullptr );

      virtual ~CMCGroup();

      virtual void Draw();
      virtual void UnDraw();
      virtual int GetFirstItemRow();
      virtual int GetTotalOverhead();

   private:
      // This group draws an empty row after if it is not the last group.
      bool m_bHasDrawnFirst;
   };
private:
   
   //std::vector<OrderedDisplayNode*> m_vecDisplayNodes;
   DisplayGroup* m_Root;
};

//class MultiDistinctGroupColumnRenderer : public ColoredGroupColumnRenderer
//{
//public:
//   MultiDistinctGroupColumnRenderer( wxPanel* aParent, wxWindowID aiWID, const Group& aGroup )
//      : ColoredGroupColumnRenderer( aParent, aiWID, aGroup )
//   {
//   }
//
//protected:
//   virtual void uiBuildColumn( std::vector<CardInterface*> avecItemData );
//   void uiAddItem( CardInterface* aptItem );
//};