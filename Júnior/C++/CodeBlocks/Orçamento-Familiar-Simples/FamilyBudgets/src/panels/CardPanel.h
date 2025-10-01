#ifndef CARDPANEL_H
#define CARDPANEL_H

#include <wx/wx.h>

// Painel de card para exibir informações
class CardPanel : public wxPanel
{
public:
    CardPanel(wxWindow* parent, 
              const wxString& title,
              const wxString& value,
              const wxColour& color = wxColour(52, 152, 219));
    
    void SetValue(const wxString& value);
    void SetColor(const wxColour& color);
    
private:
    wxStaticText* m_titleText;
    wxStaticText* m_valueText;
};

#endif // CARDPANEL_H