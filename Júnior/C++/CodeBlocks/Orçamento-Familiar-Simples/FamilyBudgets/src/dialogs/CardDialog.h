#ifndef CARDDIALOG_H
#define CARDDIALOG_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "../models/Card.h"

// Forward declaration
class DecimalTextCtrl;

class CardDialog : public wxDialog
{
public:
    // Construtor para novo cartão
    CardDialog(wxWindow* parent);
    
    // Construtor para editar cartão existente
    CardDialog(wxWindow* parent, const Card& card);
    
    virtual ~CardDialog();
    
    // Obter cartão preenchido
    Card GetCard() const;
    
private:
    void CreateInterface();
    void LoadData();
    
    // Event handlers
    void OnOK(wxCommandEvent& event);
    void OnClosingDayChanged(wxSpinEvent& event);
    void OnDueDayChanged(wxSpinEvent& event);
    
    // Validar ciclo do cartão
    bool ValidateCardCycle();
    
    // Controles
    wxTextCtrl* m_nameText;
    wxSpinCtrl* m_closingDaySpin;
    wxSpinCtrl* m_dueDaySpin;
    DecimalTextCtrl* m_limitText;
    wxTextCtrl* m_descriptionText;
    wxStaticText* m_cycleInfoText;
    
    // Dados
    Card m_card;
    bool m_isEdit;
};

#endif // CARDDIALOG_H