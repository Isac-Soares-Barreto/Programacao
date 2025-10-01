#ifndef INSTALLMENTEXPENSEDIALOG_H
#define INSTALLMENTEXPENSEDIALOG_H

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/spinctrl.h>
#include "../models/InstallmentExpense.h"

// Forward declaration
class DecimalTextCtrl;

class InstallmentExpenseDialog : public wxDialog
{
public:
    // Construtor para nova despesa parcelada
    InstallmentExpenseDialog(wxWindow* parent);

    // Construtor para editar despesa parcelada existente
    InstallmentExpenseDialog(wxWindow* parent, const InstallmentExpense& expense);

    virtual ~InstallmentExpenseDialog();

    // Obter despesa parcelada preenchida
    InstallmentExpense GetInstallmentExpense() const;

private:
    void CreateInterface();
    void LoadData();
    void LoadCombos();
    void UpdateCalculatedFields();

    // Event handlers
    void OnCardChanged(wxCommandEvent& event);
    void OnPurchaseDateChanged(wxDateEvent& event);
    void OnTotalAmountChanged(wxCommandEvent& event);
    void OnInstallmentsChanged(wxSpinEvent& event);
    void OnOK(wxCommandEvent& event);

    // Controles
    wxDatePickerCtrl* m_purchaseDatePicker;
    wxTextCtrl* m_descriptionText;
    DecimalTextCtrl* m_totalAmountText;
    wxSpinCtrl* m_installmentsSpin;
    wxComboBox* m_expenseTypeCombo;
    wxTextCtrl* m_paymentMethodText;
    wxComboBox* m_cardCombo;
    wxTextCtrl* m_dueDateText;
    wxTextCtrl* m_installmentAmountText;
    wxCheckBox* m_activeCheck;

    // Dados
    InstallmentExpense m_expense;
    bool m_isEdit;

    // IDs dos controles
    enum
    {
        ID_Card = wxID_HIGHEST + 800,
        ID_PurchaseDate,
        ID_TotalAmount,
        ID_Installments
    };
};

#endif // INSTALLMENTEXPENSEDIALOG_H
