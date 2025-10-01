#include "ExpenseDialog.h"
#include "../utils/DecimalTextCtrl.h"
#include "../utils/MoneyUtils.h"
#include "../databases/Database.h"
#include <wx/valtext.h>
#include <wx/statline.h>

ExpenseDialog::ExpenseDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Nova Despesa"),
               wxDefaultPosition, wxSize(500, 550))
    , m_isEdit(false)
{
    CreateInterface();
    LoadCombos();
    UpdateStatus();
    Centre();
}

ExpenseDialog::ExpenseDialog(wxWindow* parent, const Expense& expense)
    : wxDialog(parent, wxID_ANY, wxT("Editar Despesa"),
               wxDefaultPosition, wxSize(500, 550))
    , m_expense(expense)
    , m_isEdit(true)
{
    CreateInterface();
    LoadCombos();
    LoadData();
    UpdateStatus();
    Centre();
}

ExpenseDialog::~ExpenseDialog()
{
}

void ExpenseDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Data de Vencimento - USAR DATA VÁLIDA
    wxStaticText* dueDateLabel = new wxStaticText(this, wxID_ANY, wxT("Data de Vencimento:"));
    m_dueDatePicker = new wxDatePickerCtrl(this, ID_DueDate, wxDateTime::Today(),  // <-- MUDANÇA AQUI
                                           wxDefaultPosition, wxDefaultSize,
                                           wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    gridSizer->Add(dueDateLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_dueDatePicker, 1, wxEXPAND);

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

    // Tipo de Despesa
    wxStaticText* typeLabel = new wxStaticText(this, wxID_ANY, wxT("Tipo de Despesa:"));
    m_expenseTypeCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                        wxDefaultPosition, wxDefaultSize,
                                        0, nullptr, wxCB_READONLY);
    gridSizer->Add(typeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_expenseTypeCombo, 1, wxEXPAND);

    // Forma de Pagamento
    wxStaticText* methodLabel = new wxStaticText(this, wxID_ANY, wxT("Forma de Pagamento:"));
    m_paymentMethodCombo = new wxComboBox(this, ID_PaymentMethod, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize,
                                          0, nullptr, wxCB_READONLY);
    gridSizer->Add(methodLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_paymentMethodCombo, 1, wxEXPAND);

    // Categoria de Pagamento
    wxStaticText* categoryLabel = new wxStaticText(this, wxID_ANY, wxT("Categoria de Pagamento:"));
    m_paymentCategoryCombo = new wxComboBox(this, wxID_ANY, wxEmptyString,
                                            wxDefaultPosition, wxDefaultSize,
                                            0, nullptr, wxCB_READONLY);
    m_paymentCategoryCombo->Enable(false);
    gridSizer->Add(categoryLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_paymentCategoryCombo, 1, wxEXPAND);

    mainSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Seção de pagamento
    wxBoxSizer* paymentSizer = new wxBoxSizer(wxVERTICAL);

    // Checkbox despesa paga
    m_paidCheck = new wxCheckBox(this, ID_PaidCheck, wxT("Despesa paga"));
    paymentSizer->Add(m_paidCheck, 0, wxALL, 5);

    // Data de pagamento com checkbox - USAR DATA VÁLIDA
    wxBoxSizer* paymentDateSizer = new wxBoxSizer(wxHORIZONTAL);
    m_paymentDateCheck = new wxCheckBox(this, ID_PaymentDateCheck, wxT("Data de Pagamento:"));
    m_paymentDatePicker = new wxDatePickerCtrl(this, ID_PaymentDate, wxDateTime::Today(),  // <-- MUDANÇA AQUI
                                               wxDefaultPosition, wxDefaultSize,
                                               wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    m_paymentDatePicker->Enable(false);
    paymentDateSizer->Add(m_paymentDateCheck, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    paymentDateSizer->Add(m_paymentDatePicker, 1, wxEXPAND);
    paymentSizer->Add(paymentDateSizer, 0, wxEXPAND | wxALL, 5);

    mainSizer->Add(paymentSizer, 0, wxEXPAND | wxALL, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Situação e Valor Reajustado
    wxFlexGridSizer* statusSizer = new wxFlexGridSizer(2, 5, 10);
    statusSizer->AddGrowableCol(1);

    // Situação
    wxStaticText* statusLabel = new wxStaticText(this, wxID_ANY, wxT("Situação:"));
    m_statusText = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    m_statusText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    statusSizer->Add(statusLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    statusSizer->Add(m_statusText, 1, wxEXPAND);

    // Valor Reajustado
    wxStaticText* adjustedLabel = new wxStaticText(this, wxID_ANY, wxT("Valor Reajustado (R$):"));
    m_adjustedAmountText = new DecimalTextCtrl(this);
    m_adjustedAmountText->Enable(false);
    statusSizer->Add(adjustedLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    statusSizer->Add(m_adjustedAmountText, 1, wxEXPAND);

    mainSizer->Add(statusSizer, 0, wxALL | wxEXPAND, 10);

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
    Bind(wxEVT_COMBOBOX, &ExpenseDialog::OnPaymentMethodChanged, this, ID_PaymentMethod);
    Bind(wxEVT_CHECKBOX, &ExpenseDialog::OnPaidCheckChanged, this, ID_PaidCheck);
    Bind(wxEVT_CHECKBOX, &ExpenseDialog::OnPaymentDateCheckChanged, this, ID_PaymentDateCheck);
    Bind(wxEVT_DATE_CHANGED, &ExpenseDialog::OnPaymentDateChanged, this, ID_PaymentDate);
    Bind(wxEVT_DATE_CHANGED, &ExpenseDialog::OnDueDateChanged, this, ID_DueDate);
    Bind(wxEVT_BUTTON, &ExpenseDialog::OnOK, this, wxID_OK);

    okButton->SetDefault();
}

void ExpenseDialog::LoadCombos()
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

    // Carregar formas de pagamento
    result = db.Query(wxT("SELECT name FROM payment_methods ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_paymentMethodCombo->Append(row[0]);
        }
    }
}

void ExpenseDialog::LoadData()
{
    // Preencher campos com dados da despesa
    if (m_expense.dueDate.IsValid())
        m_dueDatePicker->SetValue(m_expense.dueDate);
    else
        m_dueDatePicker->SetValue(wxDateTime::Today());

    m_descriptionText->SetValue(m_expense.description);
    m_amountText->SetDoubleValue(m_expense.amount);

    if (!m_expense.expenseType.IsEmpty())
        m_expenseTypeCombo->SetStringSelection(m_expense.expenseType);

    if (!m_expense.paymentMethod.IsEmpty())
    {
        m_paymentMethodCombo->SetStringSelection(m_expense.paymentMethod);
        wxCommandEvent evt(wxEVT_COMBOBOX, ID_PaymentMethod);
        OnPaymentMethodChanged(evt);
    }

    if (!m_expense.paymentCategory.IsEmpty())
        m_paymentCategoryCombo->SetStringSelection(m_expense.paymentCategory);

    // Status de pagamento - CORREÇÃO
    bool shouldCheckPaid = false;

    if (m_expense.status == ExpenseStatus::PAID)
    {
        // Se o status é PAID, verificar se deve marcar checkbox
        if (m_expense.paymentDate.IsValid())
        {
            // Se tem data de pagamento e está pago, não marcar checkbox automaticamente
            // deixar o usuário decidir
            shouldCheckPaid = false;
        }
        else
        {
            // Se está pago mas não tem data, marcar checkbox
            shouldCheckPaid = true;
        }
    }

    m_paidCheck->SetValue(shouldCheckPaid);

    if (m_expense.paymentDate.IsValid())
    {
        m_paymentDateCheck->SetValue(true);
        m_paymentDatePicker->SetValue(m_expense.paymentDate);
        m_paymentDatePicker->Enable(true);
    }

    if (m_expense.adjustedAmount > 0)
    {
        m_adjustedAmountText->SetDoubleValue(m_expense.adjustedAmount);
    }
}

void ExpenseDialog::UpdateStatus()
{
    // Calcular status baseado nas regras
    bool isPaidChecked = m_paidCheck->GetValue();
    wxDateTime dueDate = m_dueDatePicker->GetValue();
    wxDateTime paymentDate;

    if (m_paymentDateCheck->GetValue() && m_paymentDatePicker->IsEnabled())
    {
        paymentDate = m_paymentDatePicker->GetValue();
    }

    if (isPaidChecked)
    {
        m_statusText->SetValue(wxT("Pago"));
        m_paymentDateCheck->Enable(false);
        m_paymentDatePicker->Enable(false);
    }
    else
    {
        m_paymentDateCheck->Enable(true);
        if (m_paymentDateCheck->GetValue())
        {
            m_paymentDatePicker->Enable(true);
        }

        // CORREÇÃO: Verificar primeiro se há data de pagamento válida
        if (m_paymentDateCheck->GetValue() && paymentDate.IsValid())
        {
            // Se tem data de pagamento, calcular status baseado nela
            if (paymentDate <= dueDate)
            {
                m_statusText->SetValue(wxT("Pago"));
            }
            else
            {
                m_statusText->SetValue(wxT("Atrasado"));
            }
        }
        else
        {
            // Sem data de pagamento, verificar se está vencida
            wxDateTime today = wxDateTime::Today();
            if (dueDate < today)
            {
                m_statusText->SetValue(wxT("Em dívida"));
            }
            else
            {
                m_statusText->SetValue(wxT("A pagar"));
            }
        }
    }

    UpdateAdjustedAmountState();
}

void ExpenseDialog::UpdateAdjustedAmountState()
{
    wxString status = m_statusText->GetValue();
    bool enableAdjusted = (status == wxT("Em dívida") || status == wxT("Atrasado"))
                         && m_amountText->GetDoubleValue() > 0;

    m_adjustedAmountText->Enable(enableAdjusted);

    if (!enableAdjusted)
    {
        m_adjustedAmountText->Clear();
    }
}

void ExpenseDialog::OnPaymentMethodChanged(wxCommandEvent& event)
{
    wxString method = m_paymentMethodCombo->GetStringSelection();

    // Limpar categorias
    m_paymentCategoryCombo->Clear();

    if (method == wxT("Cartão"))
    {
        // Carregar cartões da tabela cards
        Database& db = Database::GetInstance();
        auto result = db.Query(wxT("SELECT name FROM cards ORDER BY name"));

        if (result && !result->IsEmpty())
        {
            for (const auto& row : result->rows)
            {
                m_paymentCategoryCombo->Append(row[0]);
            }
        }

        m_paymentCategoryCombo->Enable(true);
    }
    else if (!method.IsEmpty())
    {
        // Verificar se é uma forma de pagamento com categorias customizadas
        Database& db = Database::GetInstance();

        wxString sql = wxString::Format(
            wxT("SELECT category_name FROM payment_categories ")
            wxT("WHERE payment_method_name = '%s' ORDER BY category_name"),
            StringUtils::EscapeSQL(method)
        );

        auto result = db.Query(sql);

        if (result && !result->IsEmpty())
        {
            // Tem categorias customizadas, carregar e habilitar
            for (const auto& row : result->rows)
            {
                m_paymentCategoryCombo->Append(row[0]);
            }
            m_paymentCategoryCombo->Enable(true);
        }
        else
        {
            // Não tem categorias, desabilitar
            m_paymentCategoryCombo->Enable(false);
        }
    }
    else
    {
        m_paymentCategoryCombo->Enable(false);
    }
}

void ExpenseDialog::OnPaidCheckChanged(wxCommandEvent& event)
{
    UpdateStatus();
}

void ExpenseDialog::OnPaymentDateCheckChanged(wxCommandEvent& event)
{
    bool checked = m_paymentDateCheck->GetValue();
    m_paymentDatePicker->Enable(checked && !m_paidCheck->GetValue());

    UpdateStatus();
}

void ExpenseDialog::OnPaymentDateChanged(wxDateEvent& event)
{
    UpdateStatus();
}

void ExpenseDialog::OnDueDateChanged(wxDateEvent& event)
{
    UpdateStatus();
}

void ExpenseDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
    if (!m_dueDatePicker->GetValue().IsValid())
    {
        wxMessageBox(wxT("Data de vencimento é obrigatória!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_dueDatePicker->SetFocus();
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

    // Verificar se a forma de pagamento selecionada requer categoria
    wxString paymentMethod = m_paymentMethodCombo->GetStringSelection();

    if (!paymentMethod.IsEmpty() && m_paymentCategoryCombo->IsEnabled())
    {
        // Se o combo de categoria está habilitado, significa que precisa de categoria
        if (m_paymentCategoryCombo->GetStringSelection().IsEmpty())
        {
            wxString message;

            if (paymentMethod == wxT("Cartão"))
            {
                message = wxT("Selecione o cartão!");
            }
            else
            {
                message = wxString::Format(
                    wxT("Selecione a categoria para a forma de pagamento '%s'!"),
                    paymentMethod
                );
            }

            wxMessageBox(message, wxT("Aviso"), wxOK | wxICON_WARNING, this);
            m_paymentCategoryCombo->SetFocus();
            return;
        }
    }

    event.Skip();
}

Expense ExpenseDialog::GetExpense() const
{
    Expense expense;

    expense.dueDate = m_dueDatePicker->GetValue();
    expense.description = m_descriptionText->GetValue();
    expense.amount = m_amountText->GetDoubleValue();
    expense.expenseType = m_expenseTypeCombo->GetStringSelection();
    expense.paymentMethod = m_paymentMethodCombo->GetStringSelection();
    expense.paymentCategory = m_paymentCategoryCombo->GetStringSelection();

    if (m_paymentDateCheck->GetValue() && m_paymentDatePicker->GetValue().IsValid())
    {
        expense.paymentDate = m_paymentDatePicker->GetValue();
    }

    // Calcular status
    expense.CalculateStatus(m_paidCheck->GetValue());

    // Valor reajustado
    if (m_adjustedAmountText->IsEnabled() && m_adjustedAmountText->GetDoubleValue() > 0)
    {
        expense.adjustedAmount = m_adjustedAmountText->GetDoubleValue();
    }

    return expense;
}
