#ifndef INSTALLMENTCALCULATOR_H
#define INSTALLMENTCALCULATOR_H

#include <wx/wx.h>
#include <wx/datetime.h>
#include <vector>

// Estrutura para representar uma parcela calculada
struct CalculatedInstallment
{
    int installmentNumber;        // Número da parcela (1, 2, 3...)
    wxDateTime dueDate;           // Data de vencimento
    double amount;                // Valor da parcela
    wxString description;         // Descrição formatada (ex: "Compra XYZ - Parcela 1/12")

    CalculatedInstallment()
        : installmentNumber(0)
        , amount(0.0)
    {
    }

    CalculatedInstallment(int number, const wxDateTime& date, double amt, const wxString& desc)
        : installmentNumber(number)
        , dueDate(date)
        , amount(amt)
        , description(desc)
    {
    }
};

// Classe utilitária para cálculos de parcelas
class InstallmentCalculator
{
public:
    // Calcular data de vencimento de uma parcela específica
    // baseada no ciclo do cartão (dia de fechamento e dia de vencimento)
    static wxDateTime CalculateDueDate(const wxDateTime& purchaseDate,
                                       int installmentNumber,
                                       int cardClosingDay,
                                       int cardDueDay)
    {
        if (!purchaseDate.IsValid() || installmentNumber < 1)
        {
            return wxDateTime();
        }

        // Lógica:
        // 1. Se a compra foi ANTES do fechamento, a primeira parcela vence no próximo vencimento
        // 2. Se foi DEPOIS do fechamento, vence no vencimento seguinte
        // 3. Cada parcela subsequente adiciona 1 mês

        wxDateTime calculatedDate = purchaseDate;
        int purchaseDay = purchaseDate.GetDay();

        // Determinar em qual ciclo a compra se encaixa
        if (purchaseDay <= cardClosingDay)
        {
            // Compra ANTES ou NO dia do fechamento
            // Primeira parcela vence no próximo vencimento

            // Se o vencimento é DEPOIS do fechamento no mesmo mês
            if (cardDueDay > cardClosingDay)
            {
                // Vence no mesmo mês
                calculatedDate.SetDay(cardDueDay);
            }
            else
            {
                // Vencimento é no mês seguinte
                calculatedDate.Add(wxDateSpan::Month());
                calculatedDate.SetDay(cardDueDay);
            }
        }
        else
        {
            // Compra DEPOIS do dia de fechamento
            // Primeira parcela vence no vencimento do MÊS SEGUINTE
            calculatedDate.Add(wxDateSpan::Month());

            if (cardDueDay > cardClosingDay)
            {
                // Vence no mesmo mês do próximo ciclo
                calculatedDate.SetDay(cardDueDay);
            }
            else
            {
                // Vence no mês seguinte ao próximo ciclo
                calculatedDate.Add(wxDateSpan::Month());
                calculatedDate.SetDay(cardDueDay);
            }
        }

        // Adicionar meses para parcelas subsequentes (2, 3, 4...)
        if (installmentNumber > 1)
        {
            calculatedDate.Add(wxDateSpan::Months(installmentNumber - 1));
        }

        // Ajustar se o dia não existe no mês (ex: 31 em fevereiro)
        int maxDayInMonth = wxDateTime::GetNumberOfDays(
            calculatedDate.GetMonth(),
            calculatedDate.GetYear()
        );

        if (cardDueDay > maxDayInMonth)
        {
            calculatedDate.SetDay(maxDayInMonth);
        }

        return calculatedDate;
    }

    // Calcular todas as parcelas de uma compra
    static std::vector<CalculatedInstallment> CalculateAllInstallments(
        const wxDateTime& purchaseDate,
        const wxString& description,
        double totalAmount,
        int installments,
        int cardClosingDay,
        int cardDueDay)
    {
        std::vector<CalculatedInstallment> result;

        if (!purchaseDate.IsValid() || installments <= 0 || totalAmount <= 0)
        {
            return result;
        }

        // Calcular valor de cada parcela
        double installmentAmount = totalAmount / installments;

        // Gerar cada parcela
        for (int i = 1; i <= installments; i++)
        {
            wxDateTime dueDate = CalculateDueDate(
                purchaseDate, i, cardClosingDay, cardDueDay
            );

            // Formatar descrição com número da parcela
            wxString formattedDesc = wxString::Format(
                wxT("%s - Parcela %d/%d"),
                description, i, installments
            );

            result.push_back(CalculatedInstallment(
                i, dueDate, installmentAmount, formattedDesc
            ));
        }

        return result;
    }

    // Calcular valor da parcela
    static double CalculateInstallmentAmount(double totalAmount, int installments)
    {
        if (installments <= 0)
            return 0.0;

        return totalAmount / installments;
    }

    // Validar se os dias do cartão são válidos
    static bool ValidateCardDays(int closingDay, int dueDay)
    {
        return (closingDay >= 1 && closingDay <= 31) &&
               (dueDay >= 1 && dueDay <= 31);
    }

    // Formatar descrição de uma parcela
    static wxString FormatInstallmentDescription(const wxString& baseDescription,
                                                  int currentInstallment,
                                                  int totalInstallments)
    {
        return wxString::Format(
            wxT("%s - Parcela %d/%d"),
            baseDescription,
            currentInstallment,
            totalInstallments
        );
    }
};

#endif // INSTALLMENTCALCULATOR_H
