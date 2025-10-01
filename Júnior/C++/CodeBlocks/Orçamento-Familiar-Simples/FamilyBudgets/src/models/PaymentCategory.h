#ifndef PAYMENTCATEGORY_H
#define PAYMENTCATEGORY_H

#include <wx/wx.h>

// Estrutura para representar uma categoria de forma de pagamento
struct PaymentCategory
{
    wxString paymentMethodName;  // Nome da forma de pagamento
    wxString categoryName;       // Nome da categoria
    wxString description;        // Descrição/observações
    
    // Construtor padrão
    PaymentCategory() 
    {
    }
    
    // Construtor com parâmetros
    PaymentCategory(const wxString& methodName, const wxString& catName, const wxString& desc = wxEmptyString)
        : paymentMethodName(methodName)
        , categoryName(catName)
        , description(desc)
    {
    }
};

#endif // PAYMENTCATEGORY_H