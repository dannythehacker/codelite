//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: UI.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "UI.h"


// Declare the bitmap loading function
extern void wxCrafternz79PnInitBitmapResources();

static bool bBitmapLoaded = false;


LLDBCallStackBase::LLDBCallStackBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafternz79PnInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer2);
    
    m_dvListCtrlBacktrace = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxDV_ROW_LINES|wxDV_SINGLE);
    
    boxSizer2->Add(m_dvListCtrlBacktrace, 1, wxALL|wxEXPAND, 2);
    
    m_dvListCtrlBacktrace->AppendTextColumn(_("#"), wxDATAVIEW_CELL_INERT, 40, wxALIGN_LEFT);
    m_dvListCtrlBacktrace->AppendTextColumn(_("Function"), wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT);
    m_dvListCtrlBacktrace->AppendTextColumn(_("File"), wxDATAVIEW_CELL_INERT, 300, wxALIGN_LEFT);
    m_dvListCtrlBacktrace->AppendTextColumn(_("Line"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT);
    
    SetSizeHints(500,300);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
}

LLDBCallStackBase::~LLDBCallStackBase()
{
}

LLDBBreakpointsPaneBase::LLDBBreakpointsPaneBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafternz79PnInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer10 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer10);
    
    m_auibar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxAUI_TB_PLAIN_BACKGROUND|wxAUI_TB_DEFAULT_STYLE);
    m_auibar->SetToolBitmapSize(wxSize(16,16));
    
    boxSizer10->Add(m_auibar, 0, wxEXPAND, 5);
    
    m_auibar->AddTool(wxID_NEW, _("New breakpoint"), wxArtProvider::GetBitmap(wxART_PLUS, wxART_TOOLBAR, wxSize(16, 16)), wxNullBitmap, wxITEM_NORMAL, _("New breakpoint"), _("New breakpoint"), NULL);
    
    m_auibar->AddTool(wxID_DELETE, _("Delete Selected Breakpoints"), wxArtProvider::GetBitmap(wxART_MINUS, wxART_TOOLBAR, wxSize(16, 16)), wxNullBitmap, wxITEM_NORMAL, _("Delete Selected Breakpoints"), _("Delete Selected Breakpoints"), NULL);
    
    m_auibar->AddTool(wxID_CLEAR, _("Delete All Breakpoints"), wxArtProvider::GetBitmap(wxART_DELETE, wxART_TOOLBAR, wxSize(16, 16)), wxNullBitmap, wxITEM_NORMAL, _("Delete All Breakpoints"), _("Delete All Breakpoints"), NULL);
    m_auibar->Realize();
    
    m_dataview = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxDV_ROW_LINES|wxDV_SINGLE);
    
    m_dataviewModel = new LLDBBreakpointModel;
    m_dataviewModel->SetColCount( 4 );
    m_dataview->AssociateModel(m_dataviewModel.get() );
    
    boxSizer10->Add(m_dataview, 1, wxALL|wxEXPAND, 2);
    
    m_dataview->AppendTextColumn(_("#"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 40, wxALIGN_LEFT);
    m_dataview->AppendTextColumn(_("File"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT);
    m_dataview->AppendTextColumn(_("Line"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 40, wxALIGN_LEFT);
    m_dataview->AppendTextColumn(_("Function"), m_dataview->GetColumnCount(), wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT);
    
    SetSizeHints(500,300);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    this->Connect(wxID_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LLDBBreakpointsPaneBase::OnNewBreakpoint), NULL, this);
    this->Connect(wxID_NEW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBBreakpointsPaneBase::OnNewBreakpointUI), NULL, this);
    this->Connect(wxID_DELETE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBBreakpointsPaneBase::OnDeleteBreakpointUI), NULL, this);
    this->Connect(wxID_DELETE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LLDBBreakpointsPaneBase::OnDeleteBreakpoint), NULL, this);
    this->Connect(wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LLDBBreakpointsPaneBase::OnDeleteAll), NULL, this);
    this->Connect(wxID_CLEAR, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBBreakpointsPaneBase::OnDeleteAllUI), NULL, this);
    m_dataview->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(LLDBBreakpointsPaneBase::OnBreakpointActivated), NULL, this);
    
}

LLDBBreakpointsPaneBase::~LLDBBreakpointsPaneBase()
{
    this->Disconnect(wxID_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LLDBBreakpointsPaneBase::OnNewBreakpoint), NULL, this);
    this->Disconnect(wxID_NEW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBBreakpointsPaneBase::OnNewBreakpointUI), NULL, this);
    this->Disconnect(wxID_DELETE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBBreakpointsPaneBase::OnDeleteBreakpointUI), NULL, this);
    this->Disconnect(wxID_DELETE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LLDBBreakpointsPaneBase::OnDeleteBreakpoint), NULL, this);
    this->Disconnect(wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(LLDBBreakpointsPaneBase::OnDeleteAll), NULL, this);
    this->Disconnect(wxID_CLEAR, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBBreakpointsPaneBase::OnDeleteAllUI), NULL, this);
    m_dataview->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler(LLDBBreakpointsPaneBase::OnBreakpointActivated), NULL, this);
    
}

LLDBNewBreakpointDlgBase::LLDBNewBreakpointDlgBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafternz79PnInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer32 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer32);
    
    wxFlexGridSizer* flexGridSizer41 = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer41->SetFlexibleDirection( wxBOTH );
    flexGridSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer41->AddGrowableCol(1);
    
    boxSizer32->Add(flexGridSizer41, 1, wxALL|wxEXPAND, 5);
    
    m_checkBoxFileLine = new wxCheckBox(this, wxID_ANY, _("File and Line:"), wxDefaultPosition, wxSize(-1,-1), wxALIGN_RIGHT);
    m_checkBoxFileLine->SetValue(false);
    
    flexGridSizer41->Add(m_checkBoxFileLine, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlFile = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    m_textCtrlFile->SetToolTip(_("File name"));
    
    flexGridSizer41->Add(m_textCtrlFile, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlLine = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    m_textCtrlLine->SetToolTip(_("Line number"));
    
    flexGridSizer41->Add(m_textCtrlLine, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_checkBoxFuncName = new wxCheckBox(this, wxID_ANY, _("Function name:"), wxDefaultPosition, wxSize(-1,-1), wxALIGN_RIGHT);
    m_checkBoxFuncName->SetValue(false);
    
    flexGridSizer41->Add(m_checkBoxFuncName, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    
    m_textCtrlFunctionName = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer41->Add(m_textCtrlFunctionName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    
    flexGridSizer41->Add(0, 0, 1, wxALL, 5);
    
    m_stdBtnSizer34 = new wxStdDialogButtonSizer();
    
    boxSizer32->Add(m_stdBtnSizer34, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_button36 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_button36->SetDefault();
    m_stdBtnSizer34->AddButton(m_button36);
    
    m_button38 = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer34->AddButton(m_button38);
    m_stdBtnSizer34->Realize();
    
    SetSizeHints(-1,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
    // Connect events
    m_checkBoxFileLine->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(LLDBNewBreakpointDlgBase::OnCheckFileAndLine), NULL, this);
    m_textCtrlFile->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBNewBreakpointDlgBase::OnFileLineEnabledUI), NULL, this);
    m_textCtrlLine->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBNewBreakpointDlgBase::OnFileLineEnabledUI), NULL, this);
    m_checkBoxFuncName->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(LLDBNewBreakpointDlgBase::OnCheckFuncName), NULL, this);
    m_textCtrlFunctionName->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBNewBreakpointDlgBase::OnFuncNameUI), NULL, this);
    
}

LLDBNewBreakpointDlgBase::~LLDBNewBreakpointDlgBase()
{
    m_checkBoxFileLine->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(LLDBNewBreakpointDlgBase::OnCheckFileAndLine), NULL, this);
    m_textCtrlFile->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBNewBreakpointDlgBase::OnFileLineEnabledUI), NULL, this);
    m_textCtrlLine->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBNewBreakpointDlgBase::OnFileLineEnabledUI), NULL, this);
    m_checkBoxFuncName->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(LLDBNewBreakpointDlgBase::OnCheckFuncName), NULL, this);
    m_textCtrlFunctionName->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(LLDBNewBreakpointDlgBase::OnFuncNameUI), NULL, this);
    
}

LLDBLocalsViewBase::LLDBLocalsViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafternz79PnInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer67 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer67);
    
    SetSizeHints(500,300);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    Centre(wxBOTH);
}

LLDBLocalsViewBase::~LLDBLocalsViewBase()
{
}
