#ifndef INSTALLMENTEXPENSESPANEL_H
#define INSTALLMENTEXPENSESPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/InstallmentExpense.h"

// Painel de Despesas Parceladas
class InstallmentExpensesPanel : public wxPanel
{
public:
    InstallmentExpensesPanel(wxWindow* parent);
    virtual ~InstallmentExpensesPanel();

private:
    void CreateInterface();
    void LoadInstallmentExpenses();

    // Event handlers
    void OnAddInstallmentExpense(wxCommandEvent& event);
    void OnEditInstallmentExpense(wxCommandEvent& event);
    void OnDeleteInstallmentExpense(wxCommandEvent& event);
    void OnToggleActive(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);

    // Controles
    wxListCtrl* m_listView;
    wxStaticText* m_summaryText;

    // Dados
    std::vector<InstallmentExpense> m_installmentExpenses;
};

#endif // INSTALLMENTEXPENSESPANEL_H
