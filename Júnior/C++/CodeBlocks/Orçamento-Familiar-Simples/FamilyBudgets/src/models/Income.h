#ifndef INCOME_H
#define INCOME_H

#include <wx/wx.h>
#include <wx/datetime.h>

// Estrutura para representar uma receita
struct Income
{
    wxDateTime receiptDate;       // Data de recebimento
    wxString description;         // Descrição
    double amount;                // Valor
    wxString incomeType;          // Tipo de receita
    wxString destination;         // Destino da receita

    // Construtor padrão
    Income()
        : amount(0.0)
    {
    }

    // Verificar se está completa
    bool IsValid() const
    {
        return receiptDate.IsValid() &&
               !description.IsEmpty() &&
               amount > 0;
    }
};

#endif // INCOME_H
