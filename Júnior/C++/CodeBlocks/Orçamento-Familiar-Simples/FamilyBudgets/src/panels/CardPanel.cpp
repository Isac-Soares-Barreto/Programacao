#include "CardPanel.h"

CardPanel::CardPanel(wxWindow* parent, 
                     const wxString& title,
                     const wxString& value,
                     const wxColour& color)
    : wxPanel(parent, wxID_ANY)
{
    SetBackgroundColour(*wxWHITE);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    m_titleText = new wxStaticText(this, wxID_ANY, title);
    wxFont titleFont = m_titleText->GetFont();
    titleFont.SetPointSize(9);
    m_titleText->SetFont(titleFont);
    m_titleText->SetForegroundColour(wxColour(100, 100, 100));
    mainSizer->Add(m_titleText, 0, wxALL | wxALIGN_CENTER, 5);
    
    // Valor
    m_valueText = new wxStaticText(this, wxID_ANY, value);
    wxFont valueFont = m_valueText->GetFont();
    valueFont.SetPointSize(16);
    valueFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_valueText->SetFont(valueFont);
    m_valueText->SetForegroundColour(color);
    mainSizer->Add(m_valueText, 0, wxALL | wxALIGN_CENTER, 5);
    
    SetSizer(mainSizer);
    
    // Borda
    SetWindowStyle(GetWindowStyle() | wxBORDER_SIMPLE);
}

void CardPanel::SetValue(const wxString& value)
{
    m_valueText->SetLabel(value);
}

void CardPanel::SetColor(const wxColour& color)
{
    m_valueText->SetForegroundColour(color);
}