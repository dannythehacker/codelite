#include "ColoursAndFontsManager.h"
#include "NodeJSDebugger.h"
#include "NodeJSDebuggerPane.h"
#include "NodeJSOuptutParser.h"
#include "NoteJSWorkspace.h"
#include "bookmark_manager.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "json_node.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "wxterminal.h"
#include <algorithm>
#include <editor_config.h>
#include <map>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>

class NodeJSLocalClientData : public wxTreeItemData
{
    NodeJSHandle m_handle;
    bool m_expanded;

public:
    NodeJSLocalClientData(const NodeJSHandle& h)
        : m_handle(h)
        , m_expanded(false)
    {
    }
    void SetHandle(const NodeJSHandle& handle) { this->m_handle = handle; }
    const NodeJSHandle& GetHandle() const { return m_handle; }
    void SetExpanded(bool expanded) { this->m_expanded = expanded; }
    bool IsExpanded() const { return m_expanded; }
};

NodeJSDebuggerPane::NodeJSDebuggerPane(wxWindow* parent)
    : NodeJSDebuggerPaneBase(parent)
{
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_EXPRESSION_EVALUATED, &NodeJSDebuggerPane::OnExpressionEvaluated,
                               this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeJSDebuggerPane::OnUpdateCallstack, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_INTERACT, &NodeJSDebuggerPane::OnLostControl, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_LOCALS_LOOKUP, &NodeJSDebuggerPane::OnLookup, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, &NodeJSDebuggerPane::OnConsoleLog, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeJSDebuggerPane::OnSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STOPPED, &NodeJSDebuggerPane::OnSessionStopped, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, &NodeJSDebuggerPane::OnExceptionThrown, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME, &NodeJSDebuggerPane::OnFrameSelected, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW, &NodeJSDebuggerPane::OnUpdateDebuggerView,
                               this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &NodeJSDebuggerPane::OnSettingsChanged, this);

    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        m_notebook->SetStyle((kNotebook_Default & ~kNotebook_LightTabs) | kNotebook_DarkTabs);
    } else {
        m_notebook->SetStyle(kNotebook_Default);
    }

    m_terminal = new wxTerminal(GetPanelConsoleLog());
    m_terminal->SetInteractive(true);
    GetPanelConsoleLog()->GetSizer()->Insert(0, m_terminal, 1, wxEXPAND);
    m_terminal->Bind(wxEVT_TERMINAL_EXECUTE_COMMAND, &NodeJSDebuggerPane::OnEvaluateExpression, this);

    // Build our table view for the locals
    m_locals->AddRoot(_("Hidden Root"));
    m_locals->AddHeader(_("Name"));
    m_locals->AddHeader(_("Type"));
    m_locals->AddHeader(_("Value"));
}

NodeJSDebuggerPane::~NodeJSDebuggerPane()
{
    m_terminal->Unbind(wxEVT_TERMINAL_EXECUTE_COMMAND, &NodeJSDebuggerPane::OnEvaluateExpression, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_EXPRESSION_EVALUATED, &NodeJSDebuggerPane::OnExpressionEvaluated,
                                 this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_CALLSTACK, &NodeJSDebuggerPane::OnUpdateCallstack, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_INTERACT, &NodeJSDebuggerPane::OnLostControl, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_CONSOLE_LOG, &NodeJSDebuggerPane::OnConsoleLog, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &NodeJSDebuggerPane::OnSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_EXCEPTION_THROWN, &NodeJSDebuggerPane::OnExceptionThrown, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_SELECT_FRAME, &NodeJSDebuggerPane::OnFrameSelected, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW,
                                 &NodeJSDebuggerPane::OnUpdateDebuggerView, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &NodeJSDebuggerPane::OnSettingsChanged, this);

    ClearCallstack();
}

NodeJSHandle NodeJSDebuggerPane::ParseRef(const JSONElement& ref)
{
    NodeJSOuptutParser p;
    return p.ParseRef(ref, m_handles);
}

void NodeJSDebuggerPane::ParseRefsArray(const JSONElement& refs)
{
    int refsCount = refs.arraySize();
    for(int i = 0; i < refsCount; ++i) {
        JSONElement ref = refs.arrayItem(i);
        ParseRef(ref);
    }
}

void NodeJSDebuggerPane::OnUpdateCallstack(clDebugEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_locals);
    Clear();

    JSONRoot root(event.GetString());
    JSONElement frames = root.toElement().namedObject("body").namedObject("frames");
    JSONElement refs = root.toElement().namedObject("refs");

    // Load the handlers into a map
    m_handles.clear();
    ParseRefsArray(refs);

    int count = frames.arraySize();
    for(int i = 0; i < count; ++i) {
        JSONElement frame = frames.arrayItem(i);
        int index = frame.namedObject("index").toInt();
        int funcRef = frame.namedObject("func").namedObject("ref").toInt();
        int fileRef = frame.namedObject("script").namedObject("ref").toInt();
        int line = frame.namedObject("line").toInt() + 1;

        wxVector<wxVariant> cols;
        cols.push_back(wxString() << index);
        wxString file, func;
        if(m_handles.count(funcRef)) { func = m_handles.find(funcRef)->second.value; }
        if(m_handles.count(funcRef)) { file = m_handles.find(fileRef)->second.value; }
        cols.push_back(func);
        cols.push_back(file);
        cols.push_back(wxString() << line);

        FrameData* cd = new FrameData();
        cd->file = file;
        cd->line = line;
        cd->function = func;
        cd->index = i;
        m_dvListCtrlCallstack->AppendItem(cols, (wxUIntPtr)cd);

        if(i == 0) {
            // Notify the debugger to use frame #0 for the indicator
            clDebugEvent event(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
            event.SetLineNumber(line);
            event.SetFileName(file);
            EventNotifier::Get()->AddPendingEvent(event);
            BuildLocals(frame);
            BuildArguments(frame);
        }
    }
}

void NodeJSDebuggerPane::OnLostControl(clDebugEvent& event)
{
    event.Skip();
#ifndef __WXGTK__
    m_locals->Enable(false);
    m_dvListCtrlCallstack->Enable(false);
#endif
}

void NodeJSDebuggerPane::OnConsoleLog(clDebugEvent& event)
{
    event.Skip();
    m_terminal->AddTextRaw(event.GetString());
    ::clRecalculateSTCHScrollBar(m_terminal->GetCtrl());
}

void NodeJSDebuggerPane::OnSessionStarted(clDebugEvent& event)
{
    event.Skip();
    m_terminal->Clear();
    // Clear all markers
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
}

void NodeJSDebuggerPane::OnItemActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    FrameData* cd = (FrameData*)m_dvListCtrlCallstack->GetItemData(event.GetItem());
    CHECK_PTR_RET(cd);
    NodeJSDebugger::Ptr_t debugger = NodeJSWorkspace::Get()->GetDebugger();
    if(!debugger) return;
    debugger->SelectFrame(cd->index);
}

void NodeJSDebuggerPane::ClearCallstack()
{
    for(size_t i = 0; i < m_dvListCtrlCallstack->GetItemCount(); ++i) {
        FrameData* cd = (FrameData*)m_dvListCtrlCallstack->GetItemData(m_dvListCtrlCallstack->RowToItem(i));
        wxDELETE(cd);
    }
    m_dvListCtrlCallstack->DeleteAllItems();
    m_locals->DeleteChildren(m_locals->GetRootItem());
    m_locals->Enable(true);
    m_dvListCtrlCallstack->Enable(true);
}

wxTreeItemId NodeJSDebuggerPane::AddLocal(const wxTreeItemId& parent, const wxString& name, int refId)
{
    // extract the value
    if(m_handles.count(refId)) {
        NodeJSHandle h = m_handles.find(refId)->second;
        wxTreeItemId local = m_locals->AppendItem(parent, name);
        m_locals->SetItemData(local, new NodeJSLocalClientData(h));
        m_locals->SetItemText(local, h.type, 1);
        m_locals->SetItemText(local, h.value, 2);
        if(!h.properties.empty()) { m_locals->AppendItem(local, "Loading..."); }
        return local;
    }
    return wxTreeItemId();
}

void NodeJSDebuggerPane::BuildArguments(const JSONElement& json)
{
    wxTreeItemId locals = m_locals->AppendItem(m_locals->GetRootItem(), _("Arguments"));
    JSONElement arr = json.namedObject("arguments");
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        JSONElement local = arr.arrayItem(i);
        AddLocal(locals, local.namedObject("name").toString(), local.namedObject("value").namedObject("ref").toInt());
    }
    if(m_locals->HasChildren(locals)) { m_locals->Expand(locals); }
}

void NodeJSDebuggerPane::BuildLocals(const JSONElement& json)
{
    wxTreeItemId locals = m_locals->AppendItem(m_locals->GetRootItem(), _("Locals"));
    JSONElement arr = json.namedObject("locals");
    int count = arr.arraySize();
    for(int i = 0; i < count; ++i) {
        JSONElement local = arr.arrayItem(i);
        AddLocal(locals, local.namedObject("name").toString(), local.namedObject("value").namedObject("ref").toInt());
    }
    if(m_locals->HasChildren(locals)) { m_locals->Expand(locals); }
}

void NodeJSDebuggerPane::OnExceptionThrown(clDebugEvent& event)
{
    event.Skip();
    ::wxMessageBox(_("An uncaught exception thrown!"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
    NodeJSDebugger::Ptr_t debugger = NodeJSWorkspace::Get()->GetDebugger();

    if(!debugger) return;

    wxFileName fn(event.GetFileName());
    IEditor* editor = clGetManager()->OpenFile(fn.GetFullPath());
    if(editor) {
        // Center the editor on the error line and add
        // - Error marker on the left margin
        // - Annotation text below the errornous line
        editor->CenterLine(event.GetLineNumber(), event.GetInt());
        editor->GetCtrl()->AnnotationSetText(event.GetLineNumber(), event.GetString());
        editor->GetCtrl()->AnnotationSetStyle(event.GetLineNumber(), ANNOTATION_STYLE_ERROR);
        editor->GetCtrl()->MarkerAdd(event.GetLineNumber(), smt_error);
    }
    debugger->Callstack();
}

void NodeJSDebuggerPane::OnUpdateDebuggerView(clDebugEvent& event)
{
    event.Skip();
    NodeJSDebugger::Ptr_t debugger = NodeJSWorkspace::Get()->GetDebugger();
    if(!debugger) return;
    m_dvListCtrlBreakpoints->DeleteAllItems();
    const NodeJSBreakpoint::List_t& breakpoints = debugger->GetBreakpointsMgr()->GetBreakpoints();
    std::for_each(breakpoints.begin(), breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
        wxVector<wxVariant> cols;
        cols.push_back(wxString() << bp.GetNodeBpID());
        cols.push_back(wxString() << bp.GetLine());
        cols.push_back(bp.GetFilename());
        m_dvListCtrlBreakpoints->AppendItem(cols);
    });
}

void NodeJSDebuggerPane::OnFrameSelected(clDebugEvent& event)
{
    event.Skip();
    wxWindowUpdateLocker locker(m_locals);
    m_locals->DeleteChildren(m_locals->GetRootItem());;
    m_locals->Enable(true);

    JSONRoot root(event.GetString());
    JSONElement json = root.toElement();
    JSONElement frame = json.namedObject("body");
    JSONElement refs = json.namedObject("refs");

    // Load the handlers into a map
    m_handles.clear();
    ParseRefsArray(refs);

    int index = frame.namedObject("index").toInt();
    int funcRef = frame.namedObject("func").namedObject("ref").toInt();
    int fileRef = frame.namedObject("script").namedObject("ref").toInt();
    int line = frame.namedObject("line").toInt() + 1;

    wxVector<wxVariant> cols;
    cols.push_back(wxString() << index);
    wxString file, func;
    if(m_handles.count(funcRef)) { func = m_handles.find(funcRef)->second.value; }
    if(m_handles.count(funcRef)) { file = m_handles.find(fileRef)->second.value; }
    cols.push_back(func);
    cols.push_back(file);
    cols.push_back(wxString() << line);

    // Notify the debugger to use frame #0 for the indicator
    clDebugEvent eventHighlight(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
    eventHighlight.SetLineNumber(line);
    eventHighlight.SetFileName(file);
    EventNotifier::Get()->AddPendingEvent(eventHighlight);
    BuildLocals(frame);
    BuildArguments(frame);
}

void NodeJSDebuggerPane::OnBreakpointSelected(wxDataViewEvent& event)
{
    wxVariant v;
    int line;
    wxString file;
    // sanity
    CHECK_ITEM_RET(event.GetItem());
    wxDataViewItem item = event.GetItem();
    int row = m_dvListCtrlBreakpoints->ItemToRow(item);
    // sanity
    if(row >= m_dvListCtrlBreakpoints->GetItemCount()) return;

    m_dvListCtrlBreakpoints->GetValue(v, row, 1);
    line = v.GetInteger();

    m_dvListCtrlBreakpoints->GetValue(v, row, 2);
    file = v.GetString();

    CallAfter(&NodeJSDebuggerPane::DoOpenFile, file, line);
}

void NodeJSDebuggerPane::DoOpenFile(const wxString& filename, int line)
{
    clGetManager()->OpenFile(filename, "", line - 1);
}
void NodeJSDebuggerPane::OnEvaluateExpression(clCommandEvent& event)
{
    clDebugEvent evalEvent(wxEVT_NODEJS_DEBUGGER_EVAL_EXPRESSION);
    evalEvent.SetString(event.GetString());
    EventNotifier::Get()->AddPendingEvent(evalEvent);
}

void NodeJSDebuggerPane::OnExpressionEvaluated(clDebugEvent& event)
{
    event.Skip();
    m_terminal->AddTextWithEOL(event.GetString());
}

void NodeJSDebuggerPane::OnLocalExpanding(wxTreeEvent& event)
{
    event.Skip();
    CHECK_ITEM_RET(event.GetItem());
    NodeJSLocalClientData* d = dynamic_cast<NodeJSLocalClientData*>(m_locals->GetItemData(event.GetItem()));

    CHECK_PTR_RET(d);
    if(d->IsExpanded()) {
        // nothing to be done here
        return;
    }

    if(m_locals->GetChildrenCount(event.GetItem(), false) != 1) { return; }
    d->SetExpanded(true);

    // Prepare list of refs that we don't have
    std::vector<std::pair<int, wxString> > unknownRefs;
    std::vector<std::pair<int, wxString> > knownRefs;
    const NodeJSHandle& h = d->GetHandle();
    std::for_each(h.properties.begin(), h.properties.end(), [&](const std::pair<int, wxString>& p) {
        if(m_handles.count(p.first) == 0) {
            unknownRefs.push_back(p);
        } else {
            knownRefs.push_back(p);
        }
    });
    // Delete the dummy node
    m_locals->DeleteChildren(event.GetItem());
    // Add the children
    DoAddKnownRefs(knownRefs, event.GetItem());
    DoAddUnKnownRefs(unknownRefs, event.GetItem());
}

void NodeJSDebuggerPane::DoAddKnownRefs(const std::vector<std::pair<int, wxString> >& refs, const wxTreeItemId& parent)
{
    std::for_each(refs.begin(), refs.end(),
                  [&](const std::pair<int, wxString>& p) { AddLocal(parent, p.second, p.first); });
}

void NodeJSDebuggerPane::DoAddUnKnownRefs(const std::vector<std::pair<int, wxString> >& refs,
                                          const wxTreeItemId& parent)
{
    if(!NodeJSWorkspace::Get()->GetDebugger()) return;

    std::vector<int> handles;
    std::for_each(refs.begin(), refs.end(), [&](const std::pair<int, wxString>& p) {
        PendingLookupT pl;
        pl.parent = parent;
        pl.name = p.second;
        pl.refID = p.first;
        m_pendingLookupRefs.push_back(pl);
        handles.push_back(p.first);
    });
    NodeJSWorkspace::Get()->GetDebugger()->Lookup(handles, kNodeJSContextLocals);
}

void NodeJSDebuggerPane::OnSessionStopped(clDebugEvent& event)
{
    event.Skip();
    // Clear all markers
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    std::for_each(editors.begin(), editors.end(), [&](IEditor* e) { e->DelAllCompilerMarkers(); });

    Clear();
}

void NodeJSDebuggerPane::Clear()
{
    ClearCallstack();
    m_locals->DeleteChildren(m_locals->GetRootItem());
    m_pendingLookupRefs.clear();
    m_handles.clear();
}

void NodeJSDebuggerPane::OnLookup(clDebugEvent& event)
{
    JSONRoot root(event.GetString());
    JSONElement body = root.toElement().namedObject("body");
    std::vector<PendingLookupT> unresolved;

    wxTreeItemId parent;
    for(size_t i = 0; i < m_pendingLookupRefs.size(); ++i) {
        const PendingLookupT& pl = m_pendingLookupRefs.at(i);
        if(!parent.IsOk()) { parent = pl.parent; }
        wxString nameID;
        nameID << pl.refID;

        if(!body.hasNamedObject(nameID)) {
            unresolved.push_back(pl);
            continue;
        }

        // Parse and add this ref to the global m_handles map
        JSONElement ref = body.namedObject(nameID);
        NodeJSHandle h = ParseRef(ref);
        h.name = pl.name;
        if(!h.IsOk()) continue;

        // Add the local
        AddLocal(pl.parent, pl.name, pl.refID);
    }

    if(parent.IsOk() && m_locals->ItemHasChildren(parent) && !m_locals->IsExpanded(parent)) {
        m_locals->Expand(parent);
    }
    m_pendingLookupRefs.clear();
}

void NodeJSDebuggerPane::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();
    if(EditorConfigST::Get()->GetOptions()->IsTabColourDark()) {
        m_notebook->SetStyle((m_notebook->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs);
    } else {
        m_notebook->SetStyle((m_notebook->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs);
    }
}
