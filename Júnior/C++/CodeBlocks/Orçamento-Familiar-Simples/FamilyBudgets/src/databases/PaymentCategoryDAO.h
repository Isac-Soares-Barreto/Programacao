#ifndef PAYMENTCATEGORYDAO_H
#define PAYMENTCATEGORYDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/PaymentCategory.h"

// Classe para acesso a dados de categorias de formas de pagamento
class PaymentCategoryDAO
{
public:
    PaymentCategoryDAO();
    ~PaymentCategoryDAO();
    
    // Operações CRUD
    bool Insert(const PaymentCategory& category);
    bool Update(const wxString& paymentMethodName, const wxString& oldCategoryName, const PaymentCategory& newCategory);
    bool Delete(const wxString& paymentMethodName, const wxString& categoryName);
    
    // Deletar todas as categorias de uma forma de pagamento
    bool DeleteAllByPaymentMethod(const wxString& paymentMethodName);
    
    // Consultas
    std::vector<PaymentCategory> GetByPaymentMethod(const wxString& paymentMethodName);
    std::unique_ptr<PaymentCategory> GetByKey(const wxString& paymentMethodName, const wxString& categoryName);
    
    // Verificar se categoria existe
    bool Exists(const wxString& paymentMethodName, const wxString& categoryName);
    
    // Contar categorias de uma forma de pagamento
    int CountByPaymentMethod(const wxString& paymentMethodName);
    
private:
    // Converter linha do banco para objeto PaymentCategory
    PaymentCategory RowToPaymentCategory(const wxArrayString& row);
};

#endif // PAYMENTCATEGORYDAO_H