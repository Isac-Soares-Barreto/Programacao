#ifndef INCOMEPANEL_H
#define INCOMEPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/Income.h"

// Forward declarations
class wxChoice;
class wxSpinCtrl;

class IncomePanel : public wxPanel
{
public:
    IncomePanel(wxWindow* parent);
    virtual ~IncomePanel();

private:
    void CreateInterface();
    void LoadIncomes();
    void LoadFilterCombos();
    void ClearAllFilters();

    // Event handlers
    void OnAddIncome(wxCommandEvent& event);
    void OnEditIncome(wxCommandEvent& event);
    void OnDeleteIncome(wxCommandEvent& event);
    void OnFilter(wxCommandEvent& event);
    void OnClearFilter(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnColumnClick(wxListEvent& event);

    // Métodos de ordenação
    void SortByColumn(int column);
    void RefreshListView();

    // Comparadores estáticos para ordenação
    static int CompareIncomes(const Income& i1, const Income& i2, int column, bool ascending);

    // Controles
    wxListCtrl* m_listView;
    wxChoice* m_monthChoice;
    wxSpinCtrl* m_yearSpin;
    wxChoice* m_incomeTypeChoice;
    wxChoice* m_destinationChoice;
    wxStaticText* m_summaryText;

    // Dados
    std::vector<Income> m_incomes;

    // Controle de ordenação
    int m_sortColumn;
    bool m_sortAscending;
};

#endif // INCOMEPANEL_H
