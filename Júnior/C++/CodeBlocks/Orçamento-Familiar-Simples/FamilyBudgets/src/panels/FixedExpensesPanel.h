#ifndef FIXEDEXPENSESPANEL_H
#define FIXEDEXPENSESPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/FixedExpense.h"

// Painel de Desepsas Fixas
class FixedExpensesPanel : public wxPanel
{
public:
    FixedExpensesPanel(wxWindow* parent);
    virtual ~FixedExpensesPanel();

private:
    void CreateInterface();
    void LoadFixedExpenses();

    // Event handlers
    void OnAddFixedExpense(wxCommandEvent& event);
    void OnEditFixedExpense(wxCommandEvent& event);
    void OnDeleteFixedExpense(wxCommandEvent& event);
    void OnToggleActive(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);

    // Controles
    wxListCtrl* m_listView;
    wxStaticText* m_summaryText;

    // Dados
    std::vector<FixedExpense> m_fixedExpenses;
};

#endif // FIXEDEXPENSESPANEL_H
