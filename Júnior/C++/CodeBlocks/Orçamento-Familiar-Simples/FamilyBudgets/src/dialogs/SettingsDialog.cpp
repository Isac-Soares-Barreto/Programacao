#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Configurações"), 
               wxDefaultPosition, wxSize(500, 400))
{
    CreateInterface();
    Centre();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Configurações do Sistema"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(12);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    
    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);
    
    // Área temporária
    wxStaticText* tempText = new wxStaticText(this, wxID_ANY,
        wxT("Configurações disponíveis:\n\n")
        wxT("• Backup automático\n")
        wxT("• Formato de moeda\n")
        wxT("• Alertas de vencimento\n")
        wxT("• Temas de cores\n\n")
        wxT("(Em desenvolvimento)"));
    
    mainSizer->Add(tempText, 1, wxALL | wxEXPAND, 20);
    
    // Botões
    wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();
    wxButton* okButton = new wxButton(this, wxID_OK, wxT("OK"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancelar"));
    buttonSizer->AddButton(okButton);
    buttonSizer->AddButton(cancelButton);
    buttonSizer->Realize();
    
    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 10);
    
    SetSizer(mainSizer);
    okButton->SetDefault();
}