#ifndef INVESTMENT_H
#define INVESTMENT_H

#include <wx/wx.h>
#include <wx/datetime.h>

// Estrutura para representar um investimento
struct Investment
{
    wxDateTime investmentDate;    // Data do investimento
    wxString description;         // Descrição
    double amount;                // Valor
    wxString investmentType;      // Tipo de investimento
    wxString source;              // Fonte do investimento

    // Construtor padrão
    Investment()
        : amount(0.0)
    {
    }

    // Verificar se está completo
    bool IsValid() const
    {
        return investmentDate.IsValid() &&
               !description.IsEmpty() &&
               amount > 0;
    }
};

#endif // INVESTMENT_H
