#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include <vector>
#include <memory>

// Forward declarations
class DashboardPanel;
class ExpensesPanel;
class FixedExpensesPanel;
class InstallmentExpensesPanel;
class IncomePanel;
class InvestmentsPanel;
class CardsPanel;
class PaymentMethodsPanel;

// Janela principal da aplicação
class MainFrame : public wxFrame
{
public:
    MainFrame();
    virtual ~MainFrame();

private:
    // Criar barra de menus
    void CreateMenuBar();

    // Criar barra de status
    void CreateAppStatusBar();

    // Criar notebook com abas
    void CreateNotebook();

    // Carregar ícone da janela
    void LoadWindowIcon();

    // Event handlers
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnPageChanged(wxBookCtrlEvent& event);

    // Componentes principais
    wxNotebook* m_notebook;

    // Painéis
    DashboardPanel* m_dashboardPanel;
    ExpensesPanel* m_expensesPanel;
    FixedExpensesPanel* m_fixedExpensesPanel;
    InstallmentExpensesPanel* m_installmentExpensesPanel;
    IncomePanel* m_incomePanel;
    InvestmentsPanel* m_investmentsPanel;
    CardsPanel* m_cardsPanel;
    PaymentMethodsPanel* m_paymentMethodsPanel;

    // IDs dos menus
    enum
    {
        ID_Settings = wxID_HIGHEST + 1
    };

    wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H