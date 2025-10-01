#ifndef FIXEDEXPENSEDIALOG_H
#define FIXEDEXPENSEDIALOG_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "../models/FixedExpense.h"
#include "../utils/StringUtils.h"

// Forward declaration
class DecimalTextCtrl;

class FixedExpenseDialog : public wxDialog
{
public:
    // Construtor para nova despesa fixa
    FixedExpenseDialog(wxWindow* parent);

    // Construtor para editar despesa fixa existente
    FixedExpenseDialog(wxWindow* parent, const FixedExpense& expense);

    virtual ~FixedExpenseDialog();

    // Obter despesa fixa preenchida
    FixedExpense GetFixedExpense() const;

private:
    void CreateInterface();
    void LoadData();
    void LoadCombos();

    // Event handlers
    void OnPaymentMethodChanged(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);

    // Controles
    wxSpinCtrl* m_dueDaySpin;
    wxTextCtrl* m_descriptionText;
    DecimalTextCtrl* m_amountText;
    wxComboBox* m_expenseTypeCombo;
    wxComboBox* m_paymentMethodCombo;
    wxComboBox* m_paymentCategoryCombo;
    wxCheckBox* m_activeCheck;

    // Dados
    FixedExpense m_expense;
    bool m_isEdit;

    // IDs dos controles
    enum
    {
        ID_PaymentMethod = wxID_HIGHEST + 1
    };
};

#endif // FIXEDEXPENSEDIALOG_H
