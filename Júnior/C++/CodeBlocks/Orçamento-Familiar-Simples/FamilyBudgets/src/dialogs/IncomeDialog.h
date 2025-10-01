#ifndef INCOMEDIALOG_H
#define INCOMEDIALOG_H

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include "../models/Income.h"

// Forward declaration
class DecimalTextCtrl;

class IncomeDialog : public wxDialog
{
public:
    // Construtor para nova receita
    IncomeDialog(wxWindow* parent);

    // Construtor para editar receita existente
    IncomeDialog(wxWindow* parent, const Income& income);

    virtual ~IncomeDialog();

    // Obter receita preenchida
    Income GetIncome() const;

private:
    void CreateInterface();
    void LoadData();
    void LoadCombos();

    // Event handlers
    void OnOK(wxCommandEvent& event);

    // Controles
    wxDatePickerCtrl* m_receiptDatePicker;
    wxTextCtrl* m_descriptionText;
    DecimalTextCtrl* m_amountText;
    wxComboBox* m_incomeTypeCombo;
    wxComboBox* m_destinationCombo;

    // Dados
    Income m_income;
    bool m_isEdit;
};

#endif // INCOMEDIALOG_H
