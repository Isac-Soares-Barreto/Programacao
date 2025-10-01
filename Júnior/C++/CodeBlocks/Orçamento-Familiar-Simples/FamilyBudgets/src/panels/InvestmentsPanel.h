#ifndef INVESTMENTSPANEL_H
#define INVESTMENTSPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/Investment.h"

// Forward declarations
class wxChoice;
class wxSpinCtrl;

class InvestmentsPanel : public wxPanel
{
public:
    InvestmentsPanel(wxWindow* parent);
    virtual ~InvestmentsPanel();

private:
    void CreateInterface();
    void LoadInvestments();
    void LoadFilterCombos();
    void ClearAllFilters();

    // Event handlers
    void OnAddInvestment(wxCommandEvent& event);
    void OnEditInvestment(wxCommandEvent& event);
    void OnDeleteInvestment(wxCommandEvent& event);
    void OnFilter(wxCommandEvent& event);
    void OnClearFilter(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    void OnColumnClick(wxListEvent& event);

    // Métodos de ordenação
    void SortByColumn(int column);
    void RefreshListView();

    // Comparadores estáticos para ordenação
    static int CompareInvestments(const Investment& i1, const Investment& i2, int column, bool ascending);

    // Controles
    wxListCtrl* m_listView;
    wxChoice* m_monthChoice;
    wxSpinCtrl* m_yearSpin;
    wxChoice* m_investmentTypeChoice;
    wxChoice* m_sourceChoice;
    wxStaticText* m_summaryText;

    // Dados
    std::vector<Investment> m_investments;

    // Controle de ordenação
    int m_sortColumn;
    bool m_sortAscending;
};

#endif // INVESTMENTSPANEL_H
