#ifndef DASHBOARDPANEL_H
#define DASHBOARDPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>

// Painel principal do dashboard
class DashboardPanel : public wxPanel
{
public:
    DashboardPanel(wxWindow* parent);
    virtual ~DashboardPanel();

private:
    void CreateInterface();
    wxNotebook* m_notebook;
};

#endif // DASHBOARDPANEL_H
