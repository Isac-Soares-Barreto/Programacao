#ifndef WXUTILS_H
#define WXUTILS_H

#include <wx/wx.h>
#include <wx/listctrl.h>

// Classe com utilitários para wxWidgets
class WxUtils
{
public:
    // Ajustar automaticamente a largura de todas as colunas de um wxListCtrl
    static void AutoSizeColumns(wxListCtrl* listCtrl)
    {
        if (!listCtrl) return;

        int columnCount = listCtrl->GetColumnCount();

        for (int i = 0; i < columnCount; i++)
        {
            // Ajustar pelo conteúdo
            listCtrl->SetColumnWidth(i, wxLIST_AUTOSIZE);
            int contentWidth = listCtrl->GetColumnWidth(i);

            // Ajustar pelo header
            listCtrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            int headerWidth = listCtrl->GetColumnWidth(i);

            // Usar o maior dos dois valores + margem
            int finalWidth = wxMax(contentWidth, headerWidth) + 20;

            // Obter o nome da coluna
            wxListItem item;
            item.SetMask(wxLIST_MASK_TEXT);
            listCtrl->GetColumn(i, item);
            wxString columnName = item.GetText();

            // Remover indicadores de ordenação para comparação
            wxString cleanName = columnName;
            cleanName.Replace(wxT(" ▲"), wxT(""));
            cleanName.Replace(wxT(" ▼"), wxT(""));

            // Definir larguras mínimas e máximas baseadas no tipo de coluna
            if (cleanName.Contains(wxT("Data Vencimento")))
            {
                finalWidth = wxMax(finalWidth, 125);
                finalWidth = wxMin(finalWidth, 140);
            }
            else if (cleanName.Contains(wxT("Data de Pagamento")))
            {
                finalWidth = wxMax(finalWidth, 145);
                finalWidth = wxMin(finalWidth, 180); // Aumentado o máximo
            }
            else if (cleanName.Contains(wxT("Descrição")))
            {
                finalWidth = wxMax(finalWidth, 150);
                finalWidth = wxMin(finalWidth, 400);
            }
            else if (cleanName == wxT("Valor"))
            {
                finalWidth = wxMax(finalWidth, 100);
                finalWidth = wxMin(finalWidth, 130);
            }
            else if (cleanName.Contains(wxT("Valor Reajustado")))
            {
                finalWidth = wxMax(finalWidth, 130);
                finalWidth = wxMin(finalWidth, 150);
            }
            else if (cleanName.Contains(wxT("Tipo")))
            {
                finalWidth = wxMax(finalWidth, 130);
                finalWidth = wxMin(finalWidth, 180);
            }
            else if (cleanName == wxT("Forma de Pagamento"))
            {
                finalWidth = wxMax(finalWidth, 155);
                finalWidth = wxMin(finalWidth, 200); // Aumentado o máximo
            }
            else if (cleanName == wxT("Categoria de Pagamento"))
            {
                finalWidth = wxMax(finalWidth, 170);
                finalWidth = wxMin(finalWidth, 220); // Aumentado o máximo
            }
            else if (cleanName.Contains(wxT("Situação")))
            {
                finalWidth = wxMax(finalWidth, 90);
                finalWidth = wxMin(finalWidth, 110);
            }
            else if (cleanName.Contains(wxT("Valor a Pagar")))
            {
                finalWidth = wxMax(finalWidth, 115);
                finalWidth = wxMin(finalWidth, 140);
            }
            else
            {
                finalWidth = wxMax(finalWidth, 100);
                finalWidth = wxMin(finalWidth, 150);
            }

            // Se a coluna tem indicador de ordenação, adicionar espaço extra
            if (columnName.Contains(wxT(" ▲")) || columnName.Contains(wxT(" ▼")))
            {
                finalWidth += 15;
            }

            // Aplicar a largura calculada
            listCtrl->SetColumnWidth(i, finalWidth);
        }

        // Garantir que a última coluna não expanda automaticamente
        if (columnCount > 0)
        {
            listCtrl->SetColumnWidth(columnCount - 1, listCtrl->GetColumnWidth(columnCount - 1));
        }
    }

    // Configurar wxListCtrl para melhor aparência
    static void SetupListCtrl(wxListCtrl* listCtrl)
    {
        if (!listCtrl) return;

        // Adicionar linhas de grade
        long style = listCtrl->GetWindowStyleFlag();
        style |= wxLC_HRULES | wxLC_VRULES;
        listCtrl->SetWindowStyleFlag(style);
    }
};

#endif // WXUTILS_H
