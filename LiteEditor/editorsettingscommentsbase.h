///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingscommentsbase__
#define __editorsettingscommentsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsCommentsBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsCommentsBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxSmartAddFiles;
		wxStaticLine* m_staticline1;
		wxCheckBox* m_checkBoxContCComment;
		wxCheckBox* m_checkBoxContinueCppComment;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText1;
		wxChoice* m_choiceNavKey;
	
	public:
		
		EditorSettingsCommentsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsCommentsBase();
	
};

#endif //__editorsettingscommentsbase__
