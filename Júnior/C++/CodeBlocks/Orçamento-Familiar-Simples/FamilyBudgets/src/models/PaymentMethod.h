#ifndef PAYMENTMETHOD_H
#define PAYMENTMETHOD_H

#include <wx/wx.h>

// Estrutura para representar uma forma de pagamento
struct PaymentMethod
{
    wxString name;              // Nome da forma de pagamento
    wxString description;       // Descrição/observações
    
    // Construtor padrão
    PaymentMethod() 
    {
    }
    
    // Verificar se é uma forma de pagamento que precisa de categoria
    bool NeedsCategory() const
    {
        // Apenas "Cartão" precisa de categoria (que são os cartões cadastrados)
        return name == wxT("Cartão");
    }
    
    // Verificar se é uma forma de pagamento padrão do sistema
    bool IsDefault() const
    {
        return name == wxT("Dinheiro") || 
               name == wxT("PIX") || 
               name == wxT("Boleto") || 
               name == wxT("Cartão") ||
               name == wxT("Transferência");
    }
};

#endif // PAYMENTMETHOD_H