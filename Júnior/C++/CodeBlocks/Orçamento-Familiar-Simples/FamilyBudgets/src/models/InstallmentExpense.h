#ifndef INSTALLMENTEXPENSE_H
#define INSTALLMENTEXPENSE_H

#include <wx/wx.h>
#include <wx/datetime.h>

// Estrutura para representar uma despesa parcelada
struct InstallmentExpense
{
    wxDateTime purchaseDate;      // Data da compra
    wxString description;         // Descrição
    double totalAmount;           // Valor total
    int installments;             // Quantidade de parcelas
    wxString expenseType;         // Tipo de despesa
    wxString cardName;            // Nome do cartão
    bool active;                  // Se está ativa

    // Campos calculados (não estão no banco, mas úteis para exibição)
    wxDateTime dueDate;           // Data de vencimento da primeira parcela
    double installmentAmount;     // Valor de cada parcela

    // Construtor padrão
    InstallmentExpense()
        : totalAmount(0.0)
        , installments(1)
        , active(true)
        , installmentAmount(0.0)
    {
    }

    // Calcular valor da parcela
    double CalculateInstallmentAmount() const
    {
        if (installments > 0)
            return totalAmount / installments;
        return 0.0;
    }

    // Verificar se está completa
    bool IsValid() const
    {
        return purchaseDate.IsValid() &&
               !description.IsEmpty() &&
               totalAmount > 0 &&
               installments > 0 &&
               !cardName.IsEmpty();
    }
};

#endif // INSTALLMENTEXPENSE_H
