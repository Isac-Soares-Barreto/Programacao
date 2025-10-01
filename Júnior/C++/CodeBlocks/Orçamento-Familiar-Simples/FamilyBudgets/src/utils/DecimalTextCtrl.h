#ifndef DECIMALTEXTCTRL_H
#define DECIMALTEXTCTRL_H

#include <wx/wx.h>

// Controle customizado para entrada de valores decimais
class DecimalTextCtrl : public wxTextCtrl
{
public:
    DecimalTextCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0)
        : wxTextCtrl(parent, id, value, pos, size, style)
    {
        // Configurar eventos
        Bind(wxEVT_TEXT, &DecimalTextCtrl::OnTextChange, this);
        Bind(wxEVT_KILL_FOCUS, &DecimalTextCtrl::OnKillFocus, this);
        SetHint(wxT("0,00"));
    }

    // Obter valor como double
    double GetDoubleValue() const
    {
        wxString value = GetValue();
        if (value.IsEmpty()) return 0.0;

        // Substituir vírgula por ponto para conversão
        value.Replace(wxT(","), wxT("."));

        double val = 0.0;
        value.ToDouble(&val);
        return val;
    }

    // Definir valor a partir de double
    void SetDoubleValue(double value)
    {
        wxString formatted = wxString::Format(wxT("%.2f"), value);
        formatted.Replace(wxT("."), wxT(","));
        SetValue(formatted);
    }

private:
    void OnTextChange(wxCommandEvent& event)
    {
        wxString value = GetValue();
        wxString filtered;
        bool hasComma = false;
        int commaPos = -1;

        // Filtrar caracteres e controlar vírgula
        for (size_t i = 0; i < value.length(); i++)
        {
            wxChar ch = value[i];
            if (wxIsdigit(ch))
            {
                filtered += ch;
                if (hasComma)
                {
                    // Limitar a 2 casas decimais
                    if (filtered.length() - commaPos > 3)
                    {
                        filtered = filtered.Left(commaPos + 3);
                        break;
                    }
                }
            }
            else if ((ch == wxT(',') || ch == wxT('.')) && !hasComma)
            {
                filtered += wxT(',');
                hasComma = true;
                commaPos = filtered.length() - 1;
            }
        }

        if (filtered != value)
        {
            long insertionPoint = GetInsertionPoint();
            SetValue(filtered);
            SetInsertionPoint(wxMin(insertionPoint, (long)filtered.length()));
        }

        event.Skip();
    }

    void OnKillFocus(wxFocusEvent& event)
    {
        wxString value = GetValue();
        if (!value.IsEmpty())
        {
            double val = GetDoubleValue();
            SetDoubleValue(val);
        }
        event.Skip();
    }
};

#endif // DECIMALTEXTCTRL_H
