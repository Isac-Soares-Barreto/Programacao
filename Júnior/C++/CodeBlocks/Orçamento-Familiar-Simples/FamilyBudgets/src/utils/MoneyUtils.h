#ifndef MONEYUTILS_H
#define MONEYUTILS_H

#include <wx/wx.h>
#include <wx/regex.h>
#include <iomanip>
#include <sstream>
#include <clocale>

// Classe utilitária para formatação monetária
class MoneyUtils
{
public:
    // Formatar valor para moeda brasileira (R$ 00,00)
    static wxString FormatMoney(double value)
    {
        // Usar stringstream para formatação manual confiável
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value;
        wxString formattedValue(ss.str());

        // Substituir ponto por vírgula
        formattedValue.Replace(wxT("."), wxT(","));

        // Adicionar separador de milhar (se necessário)
        // (Lógica pode ser adicionada aqui se precisar para valores > 999)

        return wxT("R$ ") + formattedValue;
    }

    // Converter string monetária para double de forma robusta
    static double ParseMoney(const wxString& moneyStr)
    {
        wxString cleanStr = moneyStr;

        // Remover tudo que não for dígito, vírgula, ponto ou sinal de menos
        wxRegEx re_digits(wxT("[^0-9,-.]"));
        re_digits.ReplaceAll(&cleanStr, wxT(""));

        // Trocar vírgula por ponto para a conversão
        cleanStr.Replace(wxT(","), wxT("."));

        double value = 0.0;

        // Tentar converter com wxString::ToDouble
        if (cleanStr.ToDouble(&value))
        {
            return value;
        }

        // Se falhar, tentar uma abordagem C-style que é mais flexível
        // Configurar o locale temporariamente para C (que usa '.')
        char* old_locale = setlocale(LC_NUMERIC, "C");
        sscanf(cleanStr.c_str(), "%lf", &value);
        // Restaurar o locale original
        setlocale(LC_NUMERIC, old_locale);

        return value;
    }

    // Validar string monetária (simplificado para apenas verificar se é um número > 0)
    static bool IsValidMoney(const wxString& moneyStr)
    {
        return ParseMoney(moneyStr) > 0.0;
    }
};

#endif // MONEYUTILS_H