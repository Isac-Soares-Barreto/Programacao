#ifndef EXPENSESPANEL_H
#define EXPENSESPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/Expense.h"

// Forward declarations
class wxListCtrl;
class wxChoice;
class wxSpinCtrl;

// Painel de Despesas
class ExpensesPanel : public wxPanel
{
public:
    ExpensesPanel(wxWindow* parent);
    virtual ~ExpensesPanel();

private:
    void CreateInterface();
    void LoadExpenses();
    void LoadFilterCombos();
    void RefreshListView();
    void ClearAllFilters();

    // Event handlers
    void OnAddExpense(wxCommandEvent& event);
    void OnEditExpense(wxCommandEvent& event);
    void OnDeleteExpense(wxCommandEvent& event);
    void OnImportFixed(wxCommandEvent& event);
    void OnImportInstallments(wxCommandEvent& event);
    void OnFilter(wxCommandEvent& event);
    void OnClearFilter(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnColumnClick(wxListEvent& event);

    // Métodos de ordenação
    void SortByColumn(int column);

    // Comparadores estáticos para ordenação
    static int CompareExpenses(const Expense& e1, const Expense& e2, int column, bool ascending);

    // Controles
    wxListCtrl* m_listView;
    wxChoice* m_monthChoice;
    wxSpinCtrl* m_yearSpin;
    wxChoice* m_expenseTypeChoice;
    wxChoice* m_paymentMethodChoice;
    wxChoice* m_statusChoice;
    wxStaticText* m_summaryText;

    // Dados
    std::vector<Expense> m_expenses;

    // Controle de ordenação
    int m_sortColumn;
    bool m_sortAscending;
};

#endif // EXPENSESPANEL_H
