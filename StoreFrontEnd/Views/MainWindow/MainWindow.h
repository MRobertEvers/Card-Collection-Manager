#pragma once

// Fixed WS_EX_LAYOUTRTL not defined by updating the preprocessor definition; WINVER=0x0500

// When a wxWindow is destroyed, it automatically deletes 
// all its children.These children are all the objects that
// received the window as the parent - argument in their constructors.

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/aui/framemanager.h>
#include <vector>
#include <memory>

class CMainWindow;
class VMainWindow;

class MainFrame
{
public:
   enum
   {
      Menu_Quit = wxID_EXIT,
      Menu_About = wxID_ABOUT,
      Menu_Main = 0x71,
      Menu_Import = 0x4,
      Menu_View_As = 0x4500,
      Menu_View_As_Deck = 0x4503,
      Menu_View_As_Cube = 0x4502
   };

   MainFrame(const wxString& title);
   ~MainFrame();

private:
   CMainWindow * m_Controller;
   VMainWindow* m_View;
   
};
