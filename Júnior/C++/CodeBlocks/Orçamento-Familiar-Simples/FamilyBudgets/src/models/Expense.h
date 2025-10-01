#ifndef EXPENSE_H
#define EXPENSE_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include "Enums.h"

// Estrutura para representar uma despesa
struct Expense
{
    wxDateTime dueDate;           // Data de vencimento
    wxString description;         // Descrição
    double amount;                // Valor
    wxString expenseType;         // Tipo de despesa
    wxString paymentMethod;       // Forma de pagamento
    wxString paymentCategory;     // Categoria de pagamento (cartão específico)
    wxDateTime paymentDate;       // Data de pagamento
    ExpenseStatus status;         // Situação
    double adjustedAmount;        // Valor reajustado
    
    // Construtor padrão
    Expense() 
        : amount(0.0)
        , status(ExpenseStatus::TO_PAY)
        , adjustedAmount(0.0)
    {
    }
    
    // Verificar se está pago
    bool IsPaid() const
    {
        return status == ExpenseStatus::PAID;
    }
    
    // Calcular status baseado nas datas
    void CalculateStatus(bool isPaidChecked)
    {
        if (isPaidChecked)
        {
            status = ExpenseStatus::PAID;
        }
        else if (paymentDate.IsValid())
        {
            if (dueDate >= paymentDate)
            {
                status = ExpenseStatus::PAID;
            }
            else
            {
                status = ExpenseStatus::LATE;
            }
        }
        else
        {
            // Sem data de pagamento
            wxDateTime today = wxDateTime::Today();
            if (dueDate < today)
            {
                status = ExpenseStatus::IN_DEBT;
            }
            else
            {
                status = ExpenseStatus::TO_PAY;
            }
        }
    }
};

#endif // EXPENSE_H