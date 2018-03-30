#pragma once
#include "IMenuEventHandler.h"
#include "CollectionInterface.h"

#include <wx\wxprec.h>

class MainFrame;

class icCollectionView : public IMenuEventHandler
{
public:
	icCollectionView(MainFrame* aParent, const wxString& aszColName);
	~icCollectionView();

	// Editors
	virtual void ShowCollectionEditor();
	virtual void CloseCollectionEditor();
	virtual void ShowStats();
	virtual void ShowHistory();

	// Loading
	virtual void BulkInsert();
	virtual void RemoveItem();
	virtual void InsertItem();

	// Control
	virtual void RefreshView(); // Full refresh.

private:
	std::shared_ptr<CollectionInterface> m_ptCollectionInterface;
};

