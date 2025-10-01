#include "FixedExpenseDialog.h"
#include "../utils/DecimalTextCtrl.h"
#include "../utils/MoneyUtils.h"
#include "../databases/Database.h"
#include <wx/statline.h>

FixedExpenseDialog::FixedExpenseDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Nova Despesa Fixa"),
               wxDefaultPosition, wxSize(450, 400))
    , m_isEdit(false)
{
    CreateInterface();
    LoadCombos();
    Centre();
}

FixedExpenseDialog::FixedExpenseDialog(wxWindow* parent, const FixedExpense& expense)
    : wxDialog(parent, wxID_ANY, wxT("Editar Despesa Fixa"),
               wxDefaultPosition, wxSize(450, 400))
    , m_expense(expense)
    , m_isEdit(true)
{
    CreateInterface();
    LoadCombos();
    LoadData();
    Centre();
}

FixedExpenseDialog::~FixedExpenseDialog()
{
}

void FixedExpenseDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Dia de Vencimento
    wxStaticText* dueDayLabel = new wxStaticText(this, wxID_ANY, wxT("Dia de Vencimento:"));
    m_dueDaySpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize,
                                  wxSP_ARROW_KEYS, 1, 31, 1);
    gridSizer->Add(dueDayLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_dueDaySpin, 1, wxEXPAND);

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

    // Checkbox ativo
    m_activeCheck = new wxCheckBox(this, wxID_ANY, wxT("Despesa ativa"));
    m_activeCheck->SetValue(true);
    mainSizer->Add(m_activeCheck, 0, wxALL, 10);

    // Nota informativa
    wxStaticText* noteText = new wxStaticText(this, wxID_ANY,
        wxT("Despesas fixas ativas serão importadas automaticamente\n")
        wxT("todos os meses no dia de vencimento especificado."));
    wxFont noteFont = noteText->GetFont();
    noteFont.SetPointSize(noteFont.GetPointSize() - 1);
    noteText->SetFont(noteFont);
    noteText->SetForegroundColour(wxColour(100, 100, 100));
    mainSizer->Add(noteText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

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
    Bind(wxEVT_COMBOBOX, &FixedExpenseDialog::OnPaymentMethodChanged, this, ID_PaymentMethod);
    Bind(wxEVT_BUTTON, &FixedExpenseDialog::OnOK, this, wxID_OK);

    okButton->SetDefault();
}

void FixedExpenseDialog::LoadCombos()
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

void FixedExpenseDialog::LoadData()
{
    // Preencher campos com dados da despesa fixa
    m_dueDaySpin->SetValue(m_expense.dueDay);
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

    m_activeCheck->SetValue(m_expense.active);
}

void FixedExpenseDialog::OnPaymentMethodChanged(wxCommandEvent& event)
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

void FixedExpenseDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
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

FixedExpense FixedExpenseDialog::GetFixedExpense() const
{
    FixedExpense expense;

    expense.dueDay = m_dueDaySpin->GetValue();
    expense.description = m_descriptionText->GetValue();
    expense.amount = m_amountText->GetDoubleValue();
    expense.expenseType = m_expenseTypeCombo->GetStringSelection();
    expense.paymentMethod = m_paymentMethodCombo->GetStringSelection();
    expense.paymentCategory = m_paymentCategoryCombo->GetStringSelection();
    expense.active = m_activeCheck->GetValue();

    return expense;
}
