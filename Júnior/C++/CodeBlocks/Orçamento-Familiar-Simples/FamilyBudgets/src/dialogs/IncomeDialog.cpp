#include "IncomeDialog.h"
#include "../utils/DecimalTextCtrl.h"
#include "../utils/MoneyUtils.h"
#include "../databases/Database.h"
#include <wx/statline.h>

IncomeDialog::IncomeDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Nova Receita"),
               wxDefaultPosition, wxSize(500, 400))
    , m_isEdit(false)
{
    CreateInterface();
    LoadCombos();
    Centre();
}

IncomeDialog::IncomeDialog(wxWindow* parent, const Income& income)
    : wxDialog(parent, wxID_ANY, wxT("Editar Receita"),
               wxDefaultPosition, wxSize(500, 400))
    , m_income(income)
    , m_isEdit(true)
{
    CreateInterface();
    LoadCombos();
    LoadData();
    Centre();
}

IncomeDialog::~IncomeDialog()
{
}

void IncomeDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Data de Recebimento
    wxStaticText* dateLabel = new wxStaticText(this, wxID_ANY, wxT("Data de Recebimento:"));
    m_receiptDatePicker = new wxDatePickerCtrl(this, wxID_ANY, wxDateTime::Today(),
                                               wxDefaultPosition, wxDefaultSize,
                                               wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    gridSizer->Add(dateLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_receiptDatePicker, 1, wxEXPAND);

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

    // Tipo de Receita
    wxStaticText* typeLabel = new wxStaticText(this, wxID_ANY, wxT("Tipo de Receita:"));
    m_incomeTypeCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxDefaultSize,
                                       0, nullptr, wxCB_READONLY);
    gridSizer->Add(typeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_incomeTypeCombo, 1, wxEXPAND);

    // Destino da Receita
    wxStaticText* destLabel = new wxStaticText(this, wxID_ANY, wxT("Destino da Receita:"));
    m_destinationCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                        wxDefaultPosition, wxDefaultSize,
                                        0, nullptr, wxCB_READONLY);
    gridSizer->Add(destLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_destinationCombo, 1, wxEXPAND);

    mainSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Informações
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Informações"));

    wxStaticText* infoText = new wxStaticText(this, wxID_ANY,
        wxT("Receitas são todas as entradas de dinheiro do orçamento familiar.\n\n")
        wxT("Tipos de receita disponíveis:\n")
        wxT("• Salário: Salário mensal\n")
        wxT("• Freelance: Trabalhos extras\n")
        wxT("• Investimentos: Rendimentos de investimentos\n")
        wxT("• Vendas: Vendas de produtos/serviços\n")
        wxT("• Outros: Outras receitas\n\n")
        wxT("O destino indica onde o dinheiro será depositado ou aplicado."));

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
    Bind(wxEVT_BUTTON, &IncomeDialog::OnOK, this, wxID_OK);

    okButton->SetDefault();
}

void IncomeDialog::LoadCombos()
{
    Database& db = Database::GetInstance();

    // Carregar tipos de receita
    auto result = db.Query(wxT("SELECT name FROM income_categories ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_incomeTypeCombo->Append(row[0]);
        }
    }

    // Carregar destinos de receita
    result = db.Query(wxT("SELECT name FROM income_destinations ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_destinationCombo->Append(row[0]);
        }
    }
}

void IncomeDialog::LoadData()
{
    // Preencher campos com dados da receita
    if (m_income.receiptDate.IsValid())
        m_receiptDatePicker->SetValue(m_income.receiptDate);
    else
        m_receiptDatePicker->SetValue(wxDateTime::Today());

    m_descriptionText->SetValue(m_income.description);
    m_amountText->SetDoubleValue(m_income.amount);

    if (!m_income.incomeType.IsEmpty())
        m_incomeTypeCombo->SetStringSelection(m_income.incomeType);

    if (!m_income.destination.IsEmpty())
        m_destinationCombo->SetStringSelection(m_income.destination);
}

void IncomeDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
    if (!m_receiptDatePicker->GetValue().IsValid())
    {
        wxMessageBox(wxT("Data de recebimento é obrigatória!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_receiptDatePicker->SetFocus();
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

Income IncomeDialog::GetIncome() const
{
    Income income;

    income.receiptDate = m_receiptDatePicker->GetValue();
    income.description = m_descriptionText->GetValue();
    income.amount = m_amountText->GetDoubleValue();
    income.incomeType = m_incomeTypeCombo->GetStringSelection();
    income.destination = m_destinationCombo->GetStringSelection();

    return income;
}
