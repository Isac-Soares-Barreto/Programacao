#ifndef PAYMENTMETHODDAO_H
#define PAYMENTMETHODDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/PaymentMethod.h"

// Classe para acesso a dados de formas de pagamento
class PaymentMethodDAO
{
public:
    PaymentMethodDAO();
    ~PaymentMethodDAO();
    
    // Operações CRUD
    bool Insert(const PaymentMethod& method);
    bool Update(const wxString& oldName, const PaymentMethod& newMethod);
    bool Delete(const wxString& name);
    
    // Consultas
    std::vector<PaymentMethod> GetAll();
    std::unique_ptr<PaymentMethod> GetByName(const wxString& name);
    
    // Verificar se forma de pagamento existe
    bool Exists(const wxString& name);
    
    // Contar formas de pagamento
    int Count();
    
private:
    // Converter linha do banco para objeto PaymentMethod
    PaymentMethod RowToPaymentMethod(const wxArrayString& row);
};

#endif // PAYMENTMETHODDAO_H