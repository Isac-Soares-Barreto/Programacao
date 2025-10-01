#ifndef ENUMS_H
#define ENUMS_H

#include <wx/wx.h>

// Enumeração para status de despesas
enum class ExpenseStatus
{
    TO_PAY,      // A pagar
    LATE,        // Atrasado
    IN_DEBT,     // Em dívida
    PAID         // Pago
};

// Converter status para string
inline wxString ExpenseStatusToString(ExpenseStatus status)
{
    switch (status)
    {
        case ExpenseStatus::TO_PAY:  return wxT("A pagar");
        case ExpenseStatus::LATE:    return wxT("Atrasado");
        case ExpenseStatus::IN_DEBT: return wxT("Em dívida");
        case ExpenseStatus::PAID:    return wxT("Pago");
        default:                     return wxT("A pagar");
    }
}

// Converter string para status
inline ExpenseStatus StringToExpenseStatus(const wxString& str)
{
    if (str == wxT("Atrasado")) return ExpenseStatus::LATE;
    if (str == wxT("Em dívida")) return ExpenseStatus::IN_DEBT;
    if (str == wxT("Pago")) return ExpenseStatus::PAID;
    return ExpenseStatus::TO_PAY;
}

#endif // ENUMS_H