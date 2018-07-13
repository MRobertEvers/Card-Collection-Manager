#pragma once
#include "VCollectionView.h"
#include "..\Views\wxExtras\wxInfiniteGrid.h"
#include "..\StoreFrontEnd\Group.h"
#include <wx/wxprec.h>
#include <wx/object.h> 
#include <wx/grid.h>
#include <memory>
#include <map>
#include <set>
#include <string>

class ColoredGroupColumnRenderer;

class RendererItem : public IRendererItem
{
public:
   RendererItem( CardInterface* apIFace );
   ~RendererItem();

   CardInterface* GetBase();

   // Inherited via IRendererItem
   virtual std::string GetHash() const override;

   virtual bool RepresentsUID( const std::string& aszKey ) const override;

   virtual std::string GetName() const override;

   virtual std::string GetMetaTag( const std::string & aszKey ) const override;

   virtual std::string GetAttribute( const std::string & aszKey ) const override;

private:
   std::shared_ptr<CardInterface> m_pCard;

   std::map<wxString, wxString> m_mapMetaMemo;
   std::map<wxString, wxString> m_mapAttrMemo;
   std::map<wxString, bool> m_mapUIDMemo;
   wxString m_hashMemo;
};

class CubeRenderer : public wxScrolledWindow, public FlexibleGroupRenderer
{
public:
   CubeRenderer( wxPanel* aParent, wxWindowID aiWID );
   virtual ~CubeRenderer();

   void Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData );

   // Returns true if the item was successfully removed.
   virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem );
   virtual void AddItem( std::shared_ptr<IRendererItem> aptItem );

   Group GetGrouping();

protected:
   virtual ColoredGroupColumnRenderer* uiGetColumnRenderer( const wxString& aszColumnName, const Group& aGroup );

private:
   void uiBuildColumns( std::vector<std::shared_ptr<IRendererItem>> avecItemData );
   void uiAddColumn( const wxString& aszColumnName,
                     std::vector<std::shared_ptr<IRendererItem>> avecItemData );
   void uiBuildGrouping();
   wxColour uiGetColumnColor( const wxString& aszColumnName );

   Group m_Grouping;
   std::map<wxString, ColoredGroupColumnRenderer*> m_mapColumns;
};

class ColumnRenderer
{
public:
   virtual void Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData ) = 0;

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

   virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem ) = 0;
   virtual void AddItem( std::shared_ptr<IRendererItem> aptItem ) = 0;

   void SetTitle( const wxString& aszTitle );
   void SetBackgroundColor( const wxColour& aColour );
   void SetFontColor( const wxColour& aColour );

   /* ColumnRenderer */
   void Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData ) = 0;
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

private:
   wxDECLARE_EVENT_TABLE();

   void onResize( wxSizeEvent& awxEvt );
   virtual void onItemClicked( wxGridEvent& awxEvt );
};

class DisplayGroup;
class DisplayNodeSource
{
public:
   virtual DisplayGroup* GetDisplayGroup( int aiType, wxString aszGroupName, Group aGroup,
                                          std::vector<std::shared_ptr<IRendererItem>> avecItems,
                                          DisplayGroup* aParent ) = 0;
};

class DisplayGroup
{
public:
   DisplayGroup( ColumnRenderer* apRenderer,
                 DisplayNodeSource* apSource,
                 wxString aszGroupName,
                 Group aGroup,
                 std::vector<std::shared_ptr<IRendererItem>> avecItems,
                 DisplayGroup* aParent = nullptr );
   virtual ~DisplayGroup();

   virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem );
   virtual bool AddItem( std::shared_ptr<IRendererItem> aptItem );
   std::shared_ptr<IRendererItem> GetItem( unsigned int auiItemRow );

   // Returns all overhead plus the size of children.
   virtual int GetTotalOverhead() = 0;
   virtual void Draw() = 0;
   virtual void UnDraw() = 0;

   // Returns the number of items drawn by this group.
   virtual int GetSize();
   // Returns the number of rows drawn by this group.
   virtual int GetDrawSize();
   // Gets the first row that belongs to this group... not necessarily drawn.
   virtual int GetFirstRow();
   virtual int GetFirstItemRow();
   virtual bool IsEmpty();
   virtual bool IsLastChild();
   virtual bool IsFirstChild();
   virtual int GetLevel();

   wxString m_szGroupName;
   Group m_Group;
   ColumnRenderer* m_pRenderer;
   DisplayGroup* m_Parent;
   DisplayNodeSource* m_pSource;
   std::map<std::shared_ptr<IRendererItem>, bool> m_mapDrawnItems;
   std::multiset<std::shared_ptr<IRendererItem>, Group::ItemSorting> m_setItems;

   // Delete the groups when done.
   std::map<wxString, DisplayGroup*, Group::Sorting> m_mapChildren;
};

class RootGroup : public DisplayGroup
{
public:
   RootGroup( ColumnRenderer* apRenderer,
              DisplayNodeSource* apSource,
              Group aGroup,
              std::vector<std::shared_ptr<IRendererItem>> avecItems,
              wxString aszGroupName = "",
              DisplayGroup* aParent = nullptr )
      : DisplayGroup(apRenderer, apSource, aszGroupName, aGroup, avecItems, aParent) {}
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

   // ColoredGroupColumnRenderer
   void Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData ) override;
   virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem ) override;
   virtual void AddItem( std::shared_ptr<IRendererItem> aptItem ) override;

   // DisplayNodeSource
   DisplayGroup* GetDisplayGroup( int aiType, wxString aszGroupName, Group aGroup,
                                  std::vector<std::shared_ptr<IRendererItem>> avecItems,
                                  DisplayGroup* aParent ) override;
   
protected:
   class TypeGroup : public DisplayGroup
   {
   public:
      TypeGroup( ColumnRenderer* apRenderer, 
                 DisplayNodeSource* apSource,
                 wxString aszGroupName,
                 Group aGroup,
                 std::vector<std::shared_ptr<IRendererItem>> avecItems,
                 DisplayGroup* aParent = nullptr );

      virtual ~TypeGroup();

      virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem );
      virtual bool AddItem( std::shared_ptr<IRendererItem> aptItem );

      virtual int GetDrawSize();
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
                wxString aszGroupName,
                Group aGroup,
                std::vector<std::shared_ptr<IRendererItem>> avecItems,
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

   // ColoredGroupColumnRenderer
   void onItemClicked( wxGridEvent& awxEvt ) override;

   wxString uiGetDisplayTitle();
   DisplayGroup* m_Root;
};

class MultiDistinctGroupColumnRenderer : public ColoredGroupColumnRenderer, public DisplayNodeSource
{
public:
   MultiDistinctGroupColumnRenderer( wxPanel* aParent, wxWindowID aiWID, const Group& aGroup );
   ~MultiDistinctGroupColumnRenderer();

   // ColoredGroupColumnRenderer
   void Draw( std::vector<std::shared_ptr<IRendererItem>> avecItemData ) override;
   virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem ) override;
   virtual void AddItem( std::shared_ptr<IRendererItem> aptItem ) override;

   // DisplayNodeSource
   DisplayGroup* GetDisplayGroup( int aiType, wxString aszGroupName, Group aGroup,
                                  std::vector<std::shared_ptr<IRendererItem>> avecItems,
                                  DisplayGroup* aParent ) override;

protected:
   class GuildGroup : public DisplayGroup
   {
   public:
      GuildGroup( ColumnRenderer* apRenderer,
                  DisplayNodeSource* apSource,
                  wxString aszGroupName,
                  Group aGroup,
                  std::vector<std::shared_ptr<IRendererItem>> avecItems,
                  DisplayGroup* aParent = nullptr );

      virtual ~GuildGroup();

      virtual bool RemoveItem( std::shared_ptr<IRendererItem> aptItem );
      virtual bool AddItem( std::shared_ptr<IRendererItem> aptItem );
      virtual void Draw();
      virtual void UnDraw();
      virtual int GetFirstItemRow();
      virtual int GetTotalOverhead();

   private:
      // This group draws an empty row after if it is not the last group.
      bool m_bHasDrawnLastSpacer;

      bool m_bHasDrawnHeader;
   };

private:
   // ColoredGroupColumnRenderer
   void onItemClicked( wxGridEvent& awxEvt ) override;

   wxString uiGetDisplayTitle();
   DisplayGroup* m_Root;
};