#ifndef CARDSPANEL_H
#define CARDSPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/Card.h"

class CardsPanel : public wxPanel
{
public:
    CardsPanel(wxWindow* parent);
    virtual ~CardsPanel();
    
private:
    void CreateInterface();
    void LoadCards();
    
    // Event handlers
    void OnAddCard(wxCommandEvent& event);
    void OnEditCard(wxCommandEvent& event);
    void OnDeleteCard(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    
    // Controles
    wxListCtrl* m_listView;
    wxStaticText* m_summaryText;
    
    // Dados
    std::vector<Card> m_cards;
};

#endif // CARDSPANEL_H