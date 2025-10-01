#include "InvestmentDialog.h"
#include "../utils/DecimalTextCtrl.h"
#include "../utils/MoneyUtils.h"
#include "../databases/Database.h"
#include <wx/statline.h>

InvestmentDialog::InvestmentDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Novo Investimento"),
               wxDefaultPosition, wxSize(500, 450))
    , m_isEdit(false)
{
    CreateInterface();
    LoadCombos();
    Centre();
}

InvestmentDialog::InvestmentDialog(wxWindow* parent, const Investment& investment)
    : wxDialog(parent, wxID_ANY, wxT("Editar Investimento"),
               wxDefaultPosition, wxSize(500, 450))
    , m_investment(investment)
    , m_isEdit(true)
{
    CreateInterface();
    LoadCombos();
    LoadData();
    Centre();
}

InvestmentDialog::~InvestmentDialog()
{
}

void InvestmentDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Data do Investimento
    wxStaticText* dateLabel = new wxStaticText(this, wxID_ANY, wxT("Data do Investimento:"));
    m_investmentDatePicker = new wxDatePickerCtrl(this, wxID_ANY, wxDateTime::Today(),
                                                   wxDefaultPosition, wxDefaultSize,
                                                   wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    gridSizer->Add(dateLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_investmentDatePicker, 1, wxEXPAND);

    // Descrição
    wxStaticText* descLabel = new wxStaticText(this, wxID_ANY, wxT("Descrição:"));
    m_descriptionText = new wxTextCtrl(this, wxID_ANY);
    m_descriptionText->SetMaxLength(200);
    gridSizer->Add(descLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_descriptionText, 1, wxEXPAND);

    // Valor
    wxStaticText* amountLabel = new wxStaticText(this, wxID_ANY, wxT("Valor (R$):"));
    m_amountText = new DecimalTextCtrl(this);
    gridSizer->Add(amountLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_amountText, 1, wxEXPAND);

    // Tipo de Investimento
    wxStaticText* typeLabel = new wxStaticText(this, wxID_ANY, wxT("Tipo de Investimento:"));
    m_investmentTypeCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                           wxDefaultPosition, wxDefaultSize,
                                           0, nullptr, wxCB_READONLY);
    gridSizer->Add(typeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_investmentTypeCombo, 1, wxEXPAND);

    // Fonte do Investimento
    wxStaticText* sourceLabel = new wxStaticText(this, wxID_ANY, wxT("Fonte do Investimento:"));
    m_sourceCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   0, nullptr, wxCB_READONLY);
    gridSizer->Add(sourceLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_sourceCombo, 1, wxEXPAND);

    mainSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Informações
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Informações"));

    wxStaticText* infoText = new wxStaticText(this, wxID_ANY,
        wxT("Investimentos são aplicações financeiras realizadas com o objetivo\n")
        wxT("de fazer o dinheiro render e crescer ao longo do tempo.\n\n")
        wxT("Tipos de investimento disponíveis:\n")
        wxT("• Poupança: Caderneta de poupança\n")
        wxT("• CDB: Certificado de Depósito Bancário\n")
        wxT("• LCI/LCA: Letra de Crédito Imobiliário/Agronegócio\n")
        wxT("• Tesouro Direto: Títulos públicos\n")
        wxT("• Ações: Mercado de ações\n")
        wxT("• Fundos: Fundos de investimento\n")
        wxT("• Outros: Outros investimentos\n\n")
        wxT("A fonte indica de onde saiu o dinheiro para o investimento\n")
        wxT("(ex: Conta Corrente, Poupança, Dinheiro, etc)."));

    wxFont infoFont = infoText->GetFont();
    infoFont.SetPointSize(infoFont.GetPointSize() - 1);
    infoText->SetFont(infoFont);
    infoText->SetForegroundColour(wxColour(60, 60, 60));

    infoBox->Add(infoText, 1, wxALL | wxEXPAND, 10);

    mainSizer->Add(infoBox, 1, wxALL | wxEXPAND, 10);

    // Botões
    wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();
    wxButton* okButton = new wxButton(this, wxID_OK, wxT("Salvar"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancelar"));
    buttonSizer->AddButton(okButton);
    buttonSizer->AddButton(cancelButton);
    buttonSizer->Realize();

    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 10);

    SetSizer(mainSizer);

    // Bindar eventos
    Bind(wxEVT_BUTTON, &InvestmentDialog::OnOK, this, wxID_OK);

    okButton->SetDefault();
}

void InvestmentDialog::LoadCombos()
{
    Database& db = Database::GetInstance();

    // Carregar tipos de investimento
    auto result = db.Query(wxT("SELECT name FROM investment_types ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_investmentTypeCombo->Append(row[0]);
        }
    }

    // Carregar fontes de investimento (mesma tabela dos destinos de receita)
    result = db.Query(wxT("SELECT name FROM income_destinations ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_sourceCombo->Append(row[0]);
        }
    }
}

void InvestmentDialog::LoadData()
{
    // Preencher campos com dados do investimento
    if (m_investment.investmentDate.IsValid())
        m_investmentDatePicker->SetValue(m_investment.investmentDate);
    else
        m_investmentDatePicker->SetValue(wxDateTime::Today());

    m_descriptionText->SetValue(m_investment.description);
    m_amountText->SetDoubleValue(m_investment.amount);

    if (!m_investment.investmentType.IsEmpty())
        m_investmentTypeCombo->SetStringSelection(m_investment.investmentType);

    if (!m_investment.source.IsEmpty())
        m_sourceCombo->SetStringSelection(m_investment.source);
}

void InvestmentDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
    if (!m_investmentDatePicker->GetValue().IsValid())
    {
        wxMessageBox(wxT("Data do investimento é obrigatória!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_investmentDatePicker->SetFocus();
        return;
    }

    if (m_descriptionText->GetValue().IsEmpty())
    {
        wxMessageBox(wxT("Descrição é obrigatória!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_descriptionText->SetFocus();
        return;
    }

    if (m_amountText->GetDoubleValue() <= 0)
    {
        wxMessageBox(wxT("Valor deve ser maior que zero!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_amountText->SetFocus();
        return;
    }

    event.Skip();
}

Investment InvestmentDialog::GetInvestment() const
{
    Investment investment;

    investment.investmentDate = m_investmentDatePicker->GetValue();
    investment.description = m_descriptionText->GetValue();
    investment.amount = m_amountText->GetDoubleValue();
    investment.investmentType = m_investmentTypeCombo->GetStringSelection();
    investment.source = m_sourceCombo->GetStringSelection();

    return investment;
}
