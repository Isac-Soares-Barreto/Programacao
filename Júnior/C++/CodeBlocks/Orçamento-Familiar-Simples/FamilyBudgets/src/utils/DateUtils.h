#ifndef DATEUTILS_H
#define DATEUTILS_H

#include <wx/wx.h>
#include <wx/datetime.h>

// Classe utilitária para manipulação de datas
class DateUtils
{
public:
    // Formatar data para exibição (dd/mm/aaaa)
    static wxString FormatDate(const wxDateTime& date)
    {
        if (!date.IsValid())
            return wxT("-");

        return date.Format(wxT("%d/%m/%Y"));
    }

    // Formatar data para banco de dados (yyyy-mm-dd)
    static wxString FormatDateDB(const wxDateTime& date)
    {
        if (!date.IsValid())
            return wxT("");

        return date.Format(wxT("%Y-%m-%d"));
    }

    // Parse de data do banco (yyyy-mm-dd)
    static wxDateTime ParseDateDB(const wxString& dateStr)
    {
        wxDateTime date;
        if (!dateStr.IsEmpty())
        {
            date.ParseFormat(dateStr, wxT("%Y-%m-%d"));
        }
        return date;
    }

    // Parse de data brasileira (dd/mm/aaaa)
    static wxDateTime ParseDateBR(const wxString& dateStr)
    {
        wxDateTime date;
        if (!dateStr.IsEmpty())
        {
            date.ParseFormat(dateStr, wxT("%d/%m/%Y"));
        }
        return date;
    }

    // Obter primeiro dia do mês
    static wxDateTime GetFirstDayOfMonth(int month, int year)
    {
        wxDateTime date(1, (wxDateTime::Month)(month - 1), year);
        return date;
    }

    // Obter último dia do mês
    static wxDateTime GetLastDayOfMonth(int month, int year)
    {
        wxDateTime date = GetFirstDayOfMonth(month, year);
        date.Add(wxDateSpan::Month());
        date.Subtract(wxDateSpan::Day());
        return date;
    }

    // Calcular próxima data de vencimento baseada em dia do mês
    static wxDateTime CalculateNextDueDate(int dueDay, int fromMonth, int fromYear)
    {
        // Garantir que o mês seja válido
        if (fromMonth < 1) fromMonth = 1;
        if (fromMonth > 12) fromMonth = 12;

        // Ajustar dia se for maior que o último dia do mês
        wxDateTime lastDay = GetLastDayOfMonth(fromMonth, fromYear);
        int actualDay = wxMin(dueDay, lastDay.GetDay());

        wxDateTime dueDate(actualDay, (wxDateTime::Month)(fromMonth - 1), fromYear);

        return dueDate;
    }
};

#endif // DATEUTILS_H
