#ifndef PAYMENTMETHODSPANEL_H
#define PAYMENTMETHODSPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/PaymentMethod.h"

class PaymentMethodsPanel : public wxPanel
{
public:
    PaymentMethodsPanel(wxWindow* parent);
    virtual ~PaymentMethodsPanel();
    
private:
    void CreateInterface();
    void LoadPaymentMethods();
    
    // Event handlers
    void OnAddPaymentMethod(wxCommandEvent& event);
    void OnEditPaymentMethod(wxCommandEvent& event);
    void OnDeletePaymentMethod(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);
    
    // Controles
    wxListCtrl* m_listView;
    wxStaticText* m_summaryText;
    
    // Dados
    std::vector<PaymentMethod> m_methods;
};

#endif // PAYMENTMETHODSPANEL_H