#ifndef EXPENSEDIALOG_H
#define EXPENSEDIALOG_H

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include "../models/Expense.h"
#include "../utils/StringUtils.h"

// Forward declaration
class DecimalTextCtrl;

class ExpenseDialog : public wxDialog
{
public:
    // Construtor para nova despesa
    ExpenseDialog(wxWindow* parent);

    // Construtor para editar despesa existente
    ExpenseDialog(wxWindow* parent, const Expense& expense);

    virtual ~ExpenseDialog();

    // Obter despesa preenchida
    Expense GetExpense() const;

private:
    void CreateInterface();
    void LoadData();
    void LoadCombos();
    void UpdateStatus();
    void UpdateAdjustedAmountState();

    // Event handlers
    void OnPaymentMethodChanged(wxCommandEvent& event);
    void OnPaidCheckChanged(wxCommandEvent& event);
    void OnPaymentDateChanged(wxDateEvent& event);
    void OnPaymentDateCheckChanged(wxCommandEvent& event);
    void OnDueDateChanged(wxDateEvent& event);
    void OnOK(wxCommandEvent& event);

    // Controles
    wxDatePickerCtrl* m_dueDatePicker;
    wxTextCtrl* m_descriptionText;
    DecimalTextCtrl* m_amountText;
    wxComboBox* m_expenseTypeCombo;
    wxComboBox* m_paymentMethodCombo;
    wxComboBox* m_paymentCategoryCombo;
    wxCheckBox* m_paidCheck;
    wxCheckBox* m_paymentDateCheck;
    wxDatePickerCtrl* m_paymentDatePicker;
    wxTextCtrl* m_statusText;
    DecimalTextCtrl* m_adjustedAmountText;

    // Dados
    Expense m_expense;
    bool m_isEdit;

    // IDs dos controles
    enum
    {
        ID_PaymentMethod = wxID_HIGHEST + 1,
        ID_PaidCheck,
        ID_PaymentDateCheck,
        ID_PaymentDate,
        ID_DueDate
    };
};

#endif // EXPENSEDIALOG_H
