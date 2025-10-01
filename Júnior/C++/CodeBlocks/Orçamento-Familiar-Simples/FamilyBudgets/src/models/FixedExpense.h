#ifndef FIXEDEXPENSE_H
#define FIXEDEXPENSE_H

#include <wx/wx.h>

// Estrutura para representar uma despesa fixa
struct FixedExpense
{
    int dueDay;                   // Dia de vencimento (1-31)
    wxString description;         // Descrição
    double amount;                // Valor
    wxString expenseType;         // Tipo de despesa
    wxString paymentMethod;       // Forma de pagamento
    wxString paymentCategory;     // Categoria de pagamento (cartão específico)
    bool active;                  // Se está ativa
    
    // Construtor padrão
    FixedExpense() 
        : dueDay(1)
        , amount(0.0)
        , active(true)
    {
    }
    
    // Validar dia do mês
    bool IsValidDueDay() const
    {
        return dueDay >= 1 && dueDay <= 31;
    }
};

#endif // FIXEDEXPENSE_H