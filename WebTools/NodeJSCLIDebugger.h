#ifndef NODEJSCLIDEBUGGER_H
#define NODEJSCLIDEBUGGER_H

#include "NodeJSDebuggerBase.h"
#include "NodeJSDebuggerBreakpoint.h"
#include "NodeJSDevToolsProtocol.h"
#include "SocketAPI/clWebSocketClient.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <wx/event.h>

class NodeJSCLIDebugger : public NodeJSDebuggerBase
{
    IProcess* m_process = nullptr;
    bool m_canInteract = false;
    wxString m_workingDirectory;
    clWebSocketClient m_socket;
    NodeJSDevToolsProtocol m_protocol;

public:
    NodeJSCLIDebugger();
    virtual ~NodeJSCLIDebugger();
    bool IsRunning() const;
    bool IsCanInteract() const;
    void StartDebugger(const wxString& command, const wxString& command_args, const wxString& workingDirectory);
    void ListBreakpoints();
    void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

protected:
    void OnDebugStart(clDebugEvent& event);
    void OnStopDebugger(clDebugEvent& event);
    void OnDebugNext(clDebugEvent& event);
    void OnDebugContinue(clDebugEvent& event);
    void OnToggleBreakpoint(clDebugEvent& event);
    void OnDebugIsRunning(clDebugEvent& event);
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnWebSocketConnected(clCommandEvent& event);
    void OnWebSocketError(clCommandEvent& event);
    void OnWebSocketOnMessage(clCommandEvent& event);
    void OnWebSocketDisconnected(clCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnInteract(clDebugEvent& event);
    // Helpers
    void DoCleanup();
    void SetBreakpoint(const wxFileName& file, int lineNumber, bool useVoidHandler = false);
    void DeleteBreakpoint(const NodeJSBreakpoint& bp);
    wxString GetBpRelativeFilePath(const NodeJSBreakpoint& bp) const;
    void ApplyAllBerakpoints();
};

#endif // NODEJSCLIDEBUGGER_H
