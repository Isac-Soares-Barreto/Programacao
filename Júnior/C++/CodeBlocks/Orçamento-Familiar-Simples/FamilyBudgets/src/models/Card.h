#ifndef CARD_H
#define CARD_H

#include <wx/wx.h>

// Estrutura para representar um cartão
struct Card
{
    wxString name;              // Nome do cartão
    int closingDay;             // Dia de fechamento (1-31)
    int dueDay;                 // Dia de vencimento (1-31)
    double cardLimit;           // Limite do cartão
    wxString description;       // Descrição/observações
    
    // Construtor padrão
    Card() 
        : closingDay(1)
        , dueDay(10)
        , cardLimit(0.0)
    {
    }
    
    // Validar dias
    bool IsValidClosingDay() const
    {
        return closingDay >= 1 && closingDay <= 31;
    }
    
    bool IsValidDueDay() const
    {
        return dueDay >= 1 && dueDay <= 31;
    }
    
    // Verificar se tem limite definido
    bool HasLimit() const
    {
        return cardLimit > 0.0;
    }
};

#endif // CARD_H