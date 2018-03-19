#include "viHistoryViewer.h"



viHistoryViewer::viHistoryViewer( wxWindow* aptParent,
                                  wxWindowID aiID,
                                  std::shared_ptr<CollectionInterface> aptInterface)
   : wxFrame( aptParent, aiID, "History" ), m_ptInterface( aptInterface )
{

}


viHistoryViewer::~viHistoryViewer()
{
}
