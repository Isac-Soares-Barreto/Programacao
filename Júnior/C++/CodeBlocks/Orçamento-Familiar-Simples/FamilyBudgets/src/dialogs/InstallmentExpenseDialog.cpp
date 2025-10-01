#include "InstallmentExpenseDialog.h"
#include "../utils/DecimalTextCtrl.h"
#include "../utils/MoneyUtils.h"
#include "../utils/DateUtils.h"
#include "../utils/InstallmentCalculator.h"
#include "../databases/Database.h"
#include "../databases/InstallmentExpenseDAO.h"
#include <wx/statline.h>

InstallmentExpenseDialog::InstallmentExpenseDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Nova Despesa Parcelada"),
               wxDefaultPosition, wxSize(550, 600))
    , m_isEdit(false)
{
    CreateInterface();
    LoadCombos();
    Centre();
}

InstallmentExpenseDialog::InstallmentExpenseDialog(wxWindow* parent, const InstallmentExpense& expense)
    : wxDialog(parent, wxID_ANY, wxT("Editar Despesa Parcelada"),
               wxDefaultPosition, wxSize(550, 600))
    , m_expense(expense)
    , m_isEdit(true)
{
    CreateInterface();
    LoadCombos();
    LoadData();
    Centre();
}

InstallmentExpenseDialog::~InstallmentExpenseDialog()
{
}

void InstallmentExpenseDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Data de Compra
    wxStaticText* dateLabel = new wxStaticText(this, wxID_ANY, wxT("Data de Compra:"));
    m_purchaseDatePicker = new wxDatePickerCtrl(this, ID_PurchaseDate, wxDateTime::Today(),
                                                wxDefaultPosition, wxDefaultSize,
                                                wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    gridSizer->Add(dateLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_purchaseDatePicker, 1, wxEXPAND);

    // Descrição
    wxStaticText* descLabel = new wxStaticText(this, wxID_ANY, wxT("Descrição:"));
    m_descriptionText = new wxTextCtrl(this, wxID_ANY);
    m_descriptionText->SetMaxLength(200);
    gridSizer->Add(descLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_descriptionText, 1, wxEXPAND);

    // Valor Total
    wxStaticText* totalLabel = new wxStaticText(this, wxID_ANY, wxT("Valor Total (R$):"));
    m_totalAmountText = new DecimalTextCtrl(this, ID_TotalAmount);
    gridSizer->Add(totalLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_totalAmountText, 1, wxEXPAND);

    // Quantidade de Parcelas
    wxStaticText* installmentsLabel = new wxStaticText(this, wxID_ANY, wxT("Qtde. Parcelas:"));
    m_installmentsSpin = new wxSpinCtrl(this, ID_Installments, wxEmptyString,
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSP_ARROW_KEYS, 1, 120, 1);
    gridSizer->Add(installmentsLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_installmentsSpin, 1, wxEXPAND);

    // Tipo de Despesa
    wxStaticText* typeLabel = new wxStaticText(this, wxID_ANY, wxT("Tipo de Despesa:"));
    m_expenseTypeCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                        wxDefaultPosition, wxDefaultSize,
                                        0, nullptr, wxCB_READONLY);
    gridSizer->Add(typeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_expenseTypeCombo, 1, wxEXPAND);

    // Forma de Pagamento (desabilitado)
    wxStaticText* methodLabel = new wxStaticText(this, wxID_ANY, wxT("Forma de Pagamento:"));
    m_paymentMethodText = new wxTextCtrl(this, wxID_ANY, wxT("Cartão"),
                                         wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    m_paymentMethodText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    gridSizer->Add(methodLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_paymentMethodText, 1, wxEXPAND);

    // Categoria de Pagamento (Cartão)
    wxStaticText* cardLabel = new wxStaticText(this, wxID_ANY, wxT("Cartão:"));
    m_cardCombo = new wxComboBox(this, ID_Card, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 0, nullptr, wxCB_READONLY);
    gridSizer->Add(cardLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_cardCombo, 1, wxEXPAND);

    mainSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Campos calculados
    wxStaticBoxSizer* calcBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Cálculos Automáticos"));

    wxFlexGridSizer* calcGrid = new wxFlexGridSizer(2, 5, 10);
    calcGrid->AddGrowableCol(1);

    // Data de Vencimento (1ª parcela)
    wxStaticText* dueDateLabel = new wxStaticText(this, wxID_ANY, wxT("Data Vencimento (1ª parcela):"));
    m_dueDateText = new wxTextCtrl(this, wxID_ANY, wxT("-"),
                                   wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    m_dueDateText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    calcGrid->Add(dueDateLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    calcGrid->Add(m_dueDateText, 1, wxEXPAND);

    // Valor da Parcela
    wxStaticText* installmentAmountLabel = new wxStaticText(this, wxID_ANY, wxT("Valor da Parcela:"));
    m_installmentAmountText = new wxTextCtrl(this, wxID_ANY, wxT("R$ 0,00"),
                                             wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    m_installmentAmountText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    calcGrid->Add(installmentAmountLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    calcGrid->Add(m_installmentAmountText, 1, wxEXPAND);

    calcBox->Add(calcGrid, 1, wxALL | wxEXPAND, 5);

    mainSizer->Add(calcBox, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Checkbox ativa
    m_activeCheck = new wxCheckBox(this, wxID_ANY, wxT("Despesa ativa"));
    m_activeCheck->SetValue(true);
    mainSizer->Add(m_activeCheck, 0, wxALL, 10);

    // Informações
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Informações"));

    wxStaticText* infoText = new wxStaticText(this, wxID_ANY,
        wxT("Despesas parceladas são compras realizadas no cartão de crédito\n")
        wxT("que serão pagas em múltiplas parcelas.\n\n")
        wxT("O sistema calcula automaticamente:\n")
        wxT("• Data de vencimento de cada parcela baseada no ciclo do cartão\n")
        wxT("• Valor de cada parcela (valor total ÷ quantidade de parcelas)\n\n")
        wxT("As parcelas serão geradas automaticamente ao salvar.\n")
        wxT("Você poderá importá-las para o módulo de Despesas quando desejar."));

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
    Bind(wxEVT_COMBOBOX, &InstallmentExpenseDialog::OnCardChanged, this, ID_Card);
    Bind(wxEVT_DATE_CHANGED, &InstallmentExpenseDialog::OnPurchaseDateChanged, this, ID_PurchaseDate);
    Bind(wxEVT_TEXT, &InstallmentExpenseDialog::OnTotalAmountChanged, this, ID_TotalAmount);
    Bind(wxEVT_SPINCTRL, &InstallmentExpenseDialog::OnInstallmentsChanged, this, ID_Installments);
    Bind(wxEVT_BUTTON, &InstallmentExpenseDialog::OnOK, this, wxID_OK);

    okButton->SetDefault();
}

void InstallmentExpenseDialog::LoadCombos()
{
    Database& db = Database::GetInstance();

    // Carregar tipos de despesa
    auto result = db.Query(wxT("SELECT name FROM expense_categories ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_expenseTypeCombo->Append(row[0]);
        }
    }

    // Carregar cartões
    result = db.Query(wxT("SELECT name FROM cards ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_cardCombo->Append(row[0]);
        }
    }
}

void InstallmentExpenseDialog::LoadData()
{
    // Preencher campos com dados da despesa parcelada
    if (m_expense.purchaseDate.IsValid())
        m_purchaseDatePicker->SetValue(m_expense.purchaseDate);
    else
        m_purchaseDatePicker->SetValue(wxDateTime::Today());

    m_descriptionText->SetValue(m_expense.description);
    m_totalAmountText->SetDoubleValue(m_expense.totalAmount);
    m_installmentsSpin->SetValue(m_expense.installments);

    if (!m_expense.expenseType.IsEmpty())
        m_expenseTypeCombo->SetStringSelection(m_expense.expenseType);

    if (!m_expense.cardName.IsEmpty())
    {
        m_cardCombo->SetStringSelection(m_expense.cardName);
        UpdateCalculatedFields();
    }

    m_activeCheck->SetValue(m_expense.active);
}

void InstallmentExpenseDialog::UpdateCalculatedFields()
{
    wxString cardName = m_cardCombo->GetStringSelection();

    if (cardName.IsEmpty())
    {
        m_dueDateText->SetValue(wxT("-"));
        m_installmentAmountText->SetValue(wxT("R$ 0,00"));
        return;
    }

    // Obter informações do cartão
    InstallmentExpenseDAO dao;
    int closingDay, dueDay;

    if (!dao.GetCardInfo(cardName, closingDay, dueDay))
    {
        m_dueDateText->SetValue(wxT("Erro: Cartão não encontrado"));
        m_installmentAmountText->SetValue(wxT("R$ 0,00"));
        return;
    }

    // Calcular data de vencimento da primeira parcela
    wxDateTime purchaseDate = m_purchaseDatePicker->GetValue();
    wxDateTime dueDate = InstallmentCalculator::CalculateDueDate(
        purchaseDate, 1, closingDay, dueDay);

    m_dueDateText->SetValue(DateUtils::FormatDate(dueDate));

    // Calcular valor da parcela
    double totalAmount = m_totalAmountText->GetDoubleValue();
    int installments = m_installmentsSpin->GetValue();

    double installmentAmount = InstallmentCalculator::CalculateInstallmentAmount(
        totalAmount, installments);

    if (installmentAmount > 0)
    {
        m_installmentAmountText->SetValue(MoneyUtils::FormatMoney(installmentAmount));
    }
    else
    {
        m_installmentAmountText->SetValue(wxT("R$ 0,00"));
    }
}

void InstallmentExpenseDialog::OnCardChanged(wxCommandEvent& event)
{
    UpdateCalculatedFields();
}

void InstallmentExpenseDialog::OnPurchaseDateChanged(wxDateEvent& event)
{
    UpdateCalculatedFields();
}

void InstallmentExpenseDialog::OnTotalAmountChanged(wxCommandEvent& event)
{
    UpdateCalculatedFields();
}

void InstallmentExpenseDialog::OnInstallmentsChanged(wxSpinEvent& event)
{
    UpdateCalculatedFields();
}

void InstallmentExpenseDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
    if (!m_purchaseDatePicker->GetValue().IsValid())
    {
        wxMessageBox(wxT("Data de compra é obrigatória!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_purchaseDatePicker->SetFocus();
        return;
    }

    if (m_descriptionText->GetValue().IsEmpty())
    {
        wxMessageBox(wxT("Descrição é obrigatória!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_descriptionText->SetFocus();
        return;
    }

    if (m_totalAmountText->GetDoubleValue() <= 0)
    {
        wxMessageBox(wxT("Valor total deve ser maior que zero!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_totalAmountText->SetFocus();
        return;
    }

    if (m_installmentsSpin->GetValue() <= 0)
    {
        wxMessageBox(wxT("Quantidade de parcelas deve ser maior que zero!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_installmentsSpin->SetFocus();
        return;
    }

    if (m_cardCombo->GetStringSelection().IsEmpty())
    {
        wxMessageBox(wxT("Selecione o cartão!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_cardCombo->SetFocus();
        return;
    }

    event.Skip();
}

InstallmentExpense InstallmentExpenseDialog::GetInstallmentExpense() const
{
    InstallmentExpense expense;

    expense.purchaseDate = m_purchaseDatePicker->GetValue();
    expense.description = m_descriptionText->GetValue();
    expense.totalAmount = m_totalAmountText->GetDoubleValue();
    expense.installments = m_installmentsSpin->GetValue();
    expense.expenseType = m_expenseTypeCombo->GetStringSelection();
    expense.cardName = m_cardCombo->GetStringSelection();
    expense.active = m_activeCheck->GetValue();

    return expense;
}
