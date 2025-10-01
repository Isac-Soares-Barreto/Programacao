#ifndef INVESTMENTDIALOG_H
#define INVESTMENTDIALOG_H

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include "../models/Investment.h"

// Forward declaration
class DecimalTextCtrl;

class InvestmentDialog : public wxDialog
{
public:
    // Construtor para novo investimento
    InvestmentDialog(wxWindow* parent);

    // Construtor para editar investimento existente
    InvestmentDialog(wxWindow* parent, const Investment& investment);

    virtual ~InvestmentDialog();

    // Obter investimento preenchido
    Investment GetInvestment() const;

private:
    void CreateInterface();
    void LoadData();
    void LoadCombos();

    // Event handlers
    void OnOK(wxCommandEvent& event);

    // Controles
    wxDatePickerCtrl* m_investmentDatePicker;
    wxTextCtrl* m_descriptionText;
    DecimalTextCtrl* m_amountText;
    wxComboBox* m_investmentTypeCombo;
    wxComboBox* m_sourceCombo;

    // Dados
    Investment m_investment;
    bool m_isEdit;
};

#endif // INVESTMENTDIALOG_H
