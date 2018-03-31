#pragma once
#include "IMenuEventHandler.h"
#include "../StoreFrontEnd/CollectionInterface.h"

#include <wx\wxprec.h>

class IMenuEventSource;

class icCollectionView : public IMenuEventHandler
{
public:
	icCollectionView( IMenuEventSource* aParent, const wxString& aszColName);
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

