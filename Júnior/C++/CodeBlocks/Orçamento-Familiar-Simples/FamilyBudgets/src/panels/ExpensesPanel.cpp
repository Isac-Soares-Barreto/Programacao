#include "ExpensesPanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/DateUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/ExpenseDAO.h"
#include "../databases/Database.h"
#include "../dialogs/ExpenseDialog.h"
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 100,
    ID_AddButton,
    ID_EditButton,
    ID_DeleteButton,
    ID_ImportFixedButton,
    ID_ImportInstallmentsButton,
    ID_FilterMonth,
    ID_FilterYear,
    ID_FilterExpenseType,
    ID_FilterPaymentMethod,
    ID_FilterStatus,
    ID_ClearFilterButton
};

// Método auxiliar para obter o índice do item selecionado
static long GetSelectedItemIndex(wxListCtrl* listCtrl)
{
    return listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

ExpensesPanel::ExpensesPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , m_sortColumn(-1)      // Inicializar sem ordenação
    , m_sortAscending(true)
{
    CreateInterface();
    LoadExpenses();
}

ExpensesPanel::~ExpensesPanel()
{
}

void ExpensesPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Despesas"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);

    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    // Barra de ferramentas
    wxBoxSizer* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnAdd = ButtonUtils::CreateButton(this, wxT("Adicionar"), wxT("add.png"));
    btnAdd->SetId(ID_AddButton);
    wxButton* btnEdit = ButtonUtils::CreateButton(this, wxT("Editar"), wxT("edit.png"));
    btnEdit->SetId(ID_EditButton);
    wxButton* btnDelete = ButtonUtils::CreateButton(this, wxT("Excluir"), wxT("delete.png"));
    btnDelete->SetId(ID_DeleteButton);
    wxButton* btnImportFixed = ButtonUtils::CreateButton(this, wxT("Importar Despesas Fixas"), wxT("import.png"));
    btnImportFixed->SetId(ID_ImportFixedButton);
    wxButton* btnImportInstallments = ButtonUtils::CreateButton(this, wxT("Importar Parceladas"), wxT("import.png"));
    btnImportInstallments->SetId(ID_ImportInstallmentsButton);

    toolbarSizer->Add(btnAdd, 0, wxALL, 5);
    toolbarSizer->Add(btnEdit, 0, wxALL, 5);
    toolbarSizer->Add(btnDelete, 0, wxALL, 5);
    toolbarSizer->AddStretchSpacer();
    toolbarSizer->Add(btnImportFixed, 0, wxALL, 5);
    toolbarSizer->Add(btnImportInstallments, 0, wxALL, 5);

    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // FILTROS
    wxStaticBoxSizer* filterBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT("Filtros"));

    // Mês
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Mês:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxArrayString months;
    months.Add(wxT("Todos"));
    months.Add(wxT("Janeiro"));
    months.Add(wxT("Fevereiro"));
    months.Add(wxT("Março"));
    months.Add(wxT("Abril"));
    months.Add(wxT("Maio"));
    months.Add(wxT("Junho"));
    months.Add(wxT("Julho"));
    months.Add(wxT("Agosto"));
    months.Add(wxT("Setembro"));
    months.Add(wxT("Outubro"));
    months.Add(wxT("Novembro"));
    months.Add(wxT("Dezembro"));

    m_monthChoice = new wxChoice(this, ID_FilterMonth, wxDefaultPosition,
                                 wxSize(100, -1), months);
    wxDateTime now = wxDateTime::Now();
    m_monthChoice->SetSelection(now.GetMonth() + 1);
    filterBox->Add(m_monthChoice, 0, wxRIGHT, 10);

    // Ano
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Ano:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    m_yearSpin = new wxSpinCtrl(this, ID_FilterYear, wxEmptyString,
                                wxDefaultPosition, wxSize(80, -1),
                                wxSP_ARROW_KEYS, 1800, 3000, now.GetYear());
    filterBox->Add(m_yearSpin, 0, wxRIGHT, 15);

    // Separador visual
    filterBox->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_VERTICAL),
                   0, wxEXPAND | wxRIGHT | wxLEFT, 5);

    // Tipo de Despesa
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Tipo:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);

    m_expenseTypeChoice = new wxChoice(this, ID_FilterExpenseType,
                                       wxDefaultPosition, wxSize(120, -1));
    m_expenseTypeChoice->Append(wxT("Todos"));
    filterBox->Add(m_expenseTypeChoice, 0, wxRIGHT, 10);

    // Forma de Pagamento
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Pagamento:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    m_paymentMethodChoice = new wxChoice(this, ID_FilterPaymentMethod,
                                         wxDefaultPosition, wxSize(120, -1));
    m_paymentMethodChoice->Append(wxT("Todos"));
    filterBox->Add(m_paymentMethodChoice, 0, wxRIGHT, 10);

    // Situação
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Situação:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    m_statusChoice = new wxChoice(this, ID_FilterStatus,
                                  wxDefaultPosition, wxSize(100, -1));
    m_statusChoice->Append(wxT("Todos"));
    m_statusChoice->Append(wxT("A pagar"));
    m_statusChoice->Append(wxT("Atrasado"));
    m_statusChoice->Append(wxT("Em dívida"));
    m_statusChoice->Append(wxT("Pago"));
    m_statusChoice->SetSelection(0);
    filterBox->Add(m_statusChoice, 0, wxRIGHT, 15);

    // Separador visual
    filterBox->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_VERTICAL),
                   0, wxEXPAND | wxRIGHT | wxLEFT, 5);

    // Botão limpar filtros
    wxButton* btnClearFilter = ButtonUtils::CreateButton(this, wxT("Limpar Filtros"), wxT("filter.png"));
    btnClearFilter->SetId(ID_ClearFilterButton);
    filterBox->Add(btnClearFilter, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    mainSizer->Add(filterBox, 0, wxEXPAND | wxALL, 10);

    // Lista de despesas
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    // Adicionar colunas
    m_listView->AppendColumn(wxT("Data Vencimento"), wxLIST_FORMAT_LEFT, 125);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 250);
    m_listView->AppendColumn(wxT("Valor"), wxLIST_FORMAT_RIGHT, 100);
    m_listView->AppendColumn(wxT("Tipo de Despesa"), wxLIST_FORMAT_LEFT, 130);
    m_listView->AppendColumn(wxT("Forma de Pagamento"), wxLIST_FORMAT_LEFT, 155);
    m_listView->AppendColumn(wxT("Categoria de Pagamento"), wxLIST_FORMAT_LEFT, 170);
    m_listView->AppendColumn(wxT("Data de Pagamento"), wxLIST_FORMAT_LEFT, 145);
    m_listView->AppendColumn(wxT("Situação"), wxLIST_FORMAT_CENTER, 90);
    m_listView->AppendColumn(wxT("Valor Reajustado"), wxLIST_FORMAT_RIGHT, 130);
    m_listView->AppendColumn(wxT("Valor a Pagar"), wxLIST_FORMAT_RIGHT, 115);

    mainSizer->Add(m_listView, 1, wxALL | wxEXPAND, 10);

    // Resumo
    m_summaryText = new wxStaticText(this, wxID_ANY, wxT("Carregando..."),
                                     wxDefaultPosition, wxDefaultSize,
                                     wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
    wxFont summaryFont = m_summaryText->GetFont();
    summaryFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_summaryText->SetFont(summaryFont);
    m_summaryText->SetMinSize(wxSize(800, 40));

    mainSizer->Add(m_summaryText, 0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(mainSizer);

    // Carregar combos de filtro
    LoadFilterCombos();

    // Bind dos eventos
    Bind(wxEVT_BUTTON, &ExpensesPanel::OnAddExpense, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &ExpensesPanel::OnEditExpense, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &ExpensesPanel::OnDeleteExpense, this, ID_DeleteButton);
    Bind(wxEVT_BUTTON, &ExpensesPanel::OnImportFixed, this, ID_ImportFixedButton);
    Bind(wxEVT_BUTTON, &ExpensesPanel::OnImportInstallments, this, ID_ImportInstallmentsButton);
    Bind(wxEVT_BUTTON, &ExpensesPanel::OnClearFilter, this, ID_ClearFilterButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ExpensesPanel::OnItemActivated, this, ID_ListView);
    Bind(wxEVT_LIST_COL_CLICK, &ExpensesPanel::OnColumnClick, this, ID_ListView);

    // Bind para aplicar filtro automaticamente
    Bind(wxEVT_CHOICE, &ExpensesPanel::OnFilter, this, ID_FilterMonth);
    Bind(wxEVT_SPINCTRL, &ExpensesPanel::OnFilter, this, ID_FilterYear);
    Bind(wxEVT_CHOICE, &ExpensesPanel::OnFilter, this, ID_FilterExpenseType);
    Bind(wxEVT_CHOICE, &ExpensesPanel::OnFilter, this, ID_FilterPaymentMethod);
    Bind(wxEVT_CHOICE, &ExpensesPanel::OnFilter, this, ID_FilterStatus);
}

void ExpensesPanel::LoadFilterCombos()
{
    Database& db = Database::GetInstance();

    // Carregar tipos de despesa
    auto result = db.Query(wxT("SELECT DISTINCT name FROM expense_categories ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_expenseTypeChoice->Append(row[0]);
        }
    }
    m_expenseTypeChoice->SetSelection(0);

    // Carregar formas de pagamento
    result = db.Query(wxT("SELECT DISTINCT name FROM payment_methods ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_paymentMethodChoice->Append(row[0]);
        }
    }
    m_paymentMethodChoice->SetSelection(0);
}

void ExpensesPanel::LoadExpenses()
{
    m_listView->DeleteAllItems();
    m_expenses.clear();

    ExpenseDAO dao;

    // Obter valores dos filtros
    int monthIndex = m_monthChoice->GetSelection();
    int year = m_yearSpin->GetValue();
    wxString expenseType = m_expenseTypeChoice->GetStringSelection();
    wxString paymentMethod = m_paymentMethodChoice->GetStringSelection();
    wxString status = m_statusChoice->GetStringSelection();

    // Aplicar filtros
    if (monthIndex == 0)
    {
        m_expenses = dao.GetAll();
    }
    else
    {
        m_expenses = dao.GetByMonth(monthIndex, year);
    }

    // Filtrar por tipo de despesa
    if (expenseType != wxT("Todos") && !expenseType.IsEmpty())
    {
        std::vector<Expense> filtered;
        for (const auto& expense : m_expenses)
        {
            if (expense.expenseType == expenseType)
            {
                filtered.push_back(expense);
            }
        }
        m_expenses = filtered;
    }

    // Filtrar por forma de pagamento
    if (paymentMethod != wxT("Todos") && !paymentMethod.IsEmpty())
    {
        std::vector<Expense> filtered;
        for (const auto& expense : m_expenses)
        {
            if (expense.paymentMethod == paymentMethod)
            {
                filtered.push_back(expense);
            }
        }
        m_expenses = filtered;
    }

    // Filtrar por situação
    if (status != wxT("Todos") && !status.IsEmpty())
    {
        std::vector<Expense> filtered;
        for (const auto& expense : m_expenses)
        {
            if (ExpenseStatusToString(expense.status) == status)
            {
                filtered.push_back(expense);
            }
        }
        m_expenses = filtered;
    }

    // Preencher lista
    double totalAmount = 0.0;
    double totalPaid = 0.0;      // Pago + Atrasado
    double totalPending = 0.0;    // A pagar + Em dívida
    int countPaid = 0;
    int countToPay = 0;
    int countLate = 0;
    int countInDebt = 0;

    for (size_t i = 0; i < m_expenses.size(); i++)
    {
        const Expense& expense = m_expenses[i];

        // Calcular valor a pagar
        double valuePay = expense.adjustedAmount > 0 ? expense.adjustedAmount : expense.amount;

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(expense.dueDate));
        m_listView->SetItem(itemIndex, 1, expense.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(expense.amount));
        m_listView->SetItem(itemIndex, 3, expense.expenseType);
        m_listView->SetItem(itemIndex, 4, expense.paymentMethod);
        m_listView->SetItem(itemIndex, 5, expense.paymentCategory);
        m_listView->SetItem(itemIndex, 6, DateUtils::FormatDate(expense.paymentDate));
        m_listView->SetItem(itemIndex, 7, ExpenseStatusToString(expense.status));

        if (expense.adjustedAmount > 0)
        {
            m_listView->SetItem(itemIndex, 8, MoneyUtils::FormatMoney(expense.adjustedAmount));
        }

        // Valor a Pagar
        m_listView->SetItem(itemIndex, 9, MoneyUtils::FormatMoney(valuePay));

        // Colorir linha baseado no status
        wxColour color;
        switch (expense.status)
        {
            case ExpenseStatus::PAID:
                color = wxColour(220, 255, 220); // Verde claro
                countPaid++;
                break;
            case ExpenseStatus::LATE:
                color = wxColour(255, 255, 200); // Amarelo claro
                countLate++;
                break;
            case ExpenseStatus::IN_DEBT:
                color = wxColour(255, 220, 220); // Vermelho claro
                countInDebt++;
                break;
            default:
                color = *wxWHITE;
                countToPay++;
        }

        m_listView->SetItemBackgroundColour(itemIndex, color);

        // Cálculo de totais
        totalAmount += valuePay;  // Total geral usa valor a pagar

        switch (expense.status)
        {
            case ExpenseStatus::PAID:
            case ExpenseStatus::LATE:
                totalPaid += valuePay;  // Pago + Atrasado
                break;
            case ExpenseStatus::TO_PAY:
            case ExpenseStatus::IN_DEBT:
                totalPending += valuePay;  // A pagar + Em dívida
                break;
        }
    }

    // Ajustar largura das colunas
    if (m_expenses.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 125); // Data Vencimento
        m_listView->SetColumnWidth(1, 250); // Descrição
        m_listView->SetColumnWidth(2, 100); // Valor
        m_listView->SetColumnWidth(3, 130); // Tipo de Despesa
        m_listView->SetColumnWidth(4, 155); // Forma de Pagamento
        m_listView->SetColumnWidth(5, 170); // Categoria de Pagamento
        m_listView->SetColumnWidth(6, 145); // Data de Pagamento
        m_listView->SetColumnWidth(7, 90);  // Situação
        m_listView->SetColumnWidth(8, 130); // Valor Reajustado
        m_listView->SetColumnWidth(9, 115); // Valor a Pagar
    }

    // Resetar ordenação quando recarregar dados
    m_sortColumn = -1;
    m_sortAscending = true;

    // Remover indicadores de ordenação de todos os cabeçalhos
    for (int i = 0; i < m_listView->GetColumnCount(); i++)
    {
        wxListItem item;
        item.SetMask(wxLIST_MASK_TEXT);
        m_listView->GetColumn(i, item);

        wxString text = item.GetText();
        text.Replace(wxT(" ▲"), wxT(""));
        text.Replace(wxT(" ▼"), wxT(""));

        item.SetText(text);
        m_listView->SetColumn(i, item);
    }

    // Atualizar resumo
    wxString summary;

    // Mostrar filtros ativos no resumo
    wxString filterInfo;
    if (monthIndex > 0)
    {
        filterInfo += wxString::Format(wxT("%s/%d"), m_monthChoice->GetStringSelection(), year);
    }
    if (expenseType != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Tipo: ") + expenseType;
    }
    if (paymentMethod != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Pagamento: ") + paymentMethod;
    }
    if (status != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Situação: ") + status;
    }

    if (!filterInfo.IsEmpty())
    {
        summary = wxT("Filtros: ") + filterInfo + wxT("\n");
    }

    summary += wxString::Format(
        wxT("Total: %d despesas | Pagas: %d | A pagar: %d | Atrasadas: %d | Em dívida: %d\n")
        wxT("Valores - Total: %s | Pago: %s | Pendente: %s"),
        (int)m_expenses.size(), countPaid, countToPay, countLate, countInDebt,
        MoneyUtils::FormatMoney(totalAmount),
        MoneyUtils::FormatMoney(totalPaid),      // Pago + Atrasado
        MoneyUtils::FormatMoney(totalPending)     // A pagar + Em dívida
    );

    m_summaryText->SetLabel(summary);
    m_summaryText->InvalidateBestSize();
    m_summaryText->GetParent()->Layout();

    wxSize bestSize = m_summaryText->GetBestSize();
    m_summaryText->SetMinSize(bestSize);

    this->Layout();
    this->Refresh();
}

void ExpensesPanel::OnAddExpense(wxCommandEvent& event)
{
    ExpenseDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        Expense expense = dialog.GetExpense();
        ExpenseDAO dao;

        if (dao.Insert(expense))
        {
            LoadExpenses();
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar despesa!\n\nVerifique se já não existe uma despesa\ncom a mesma data e descrição."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void ExpensesPanel::OnEditExpense(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Expense& oldExpense = m_expenses[selectedIndex];
    ExpenseDialog dialog(this, oldExpense);

    if (dialog.ShowModal() == wxID_OK)
    {
        Expense newExpense = dialog.GetExpense();
        ExpenseDAO dao;

        if (dao.Update(oldExpense, newExpense))
        {
            LoadExpenses();
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar despesa!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void ExpensesPanel::OnDeleteExpense(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Expense& expense = m_expenses[selectedIndex];

    wxString message = wxString::Format(
        wxT("Confirma a exclusão da despesa:\n\n%s\nVencimento: %s\nValor: %s"),
        expense.description,
        DateUtils::FormatDate(expense.dueDate),
        MoneyUtils::FormatMoney(expense.amount)
    );

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        ExpenseDAO dao;

        if (dao.Delete(expense.dueDate, expense.description))
        {
            LoadExpenses();
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir despesa!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void ExpensesPanel::OnImportFixed(wxCommandEvent& event)
{
    // Diálogo para selecionar mês/ano
    wxDialog dlg(this, wxID_ANY, wxT("Importar Despesas Fixas"),
                 wxDefaultPosition, wxSize(350, 300));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(&dlg, wxID_ANY,
        wxT("Selecione o mês/ano para importar as despesas fixas:"));
    sizer->Add(label, 0, wxALL, 10);

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    wxArrayString months;
    for (int i = 1; i <= 12; i++)
    {
        months.Add(wxDateTime::GetMonthName((wxDateTime::Month)(i-1)));
    }

    wxChoice* monthChoice = new wxChoice(&dlg, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, months);
    monthChoice->SetSelection(wxDateTime::Now().GetMonth());

    wxSpinCtrl* yearSpin = new wxSpinCtrl(&dlg, wxID_ANY, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxSP_ARROW_KEYS, 1800, 3000,
                                          wxDateTime::Now().GetYear());

    gridSizer->Add(new wxStaticText(&dlg, wxID_ANY, wxT("Mês:")),
                   0, wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(monthChoice, 1, wxEXPAND);
    gridSizer->Add(new wxStaticText(&dlg, wxID_ANY, wxT("Ano:")),
                   0, wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(yearSpin, 1, wxEXPAND);

    sizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Nota informativa
    wxStaticText* noteText = new wxStaticText(&dlg, wxID_ANY,
        wxT("Nota: Despesas que já existem com a mesma data\n")
        wxT("de vencimento e descrição não serão importadas\n")
        wxT("novamente para evitar duplicação."));
    wxFont noteFont = noteText->GetFont();
    noteFont.SetPointSize(noteFont.GetPointSize() - 1);
    noteText->SetFont(noteFont);
    noteText->SetForegroundColour(wxColour(100, 100, 100));
    sizer->Add(noteText, 0, wxALL | wxEXPAND, 10);

    wxStdDialogButtonSizer* btnSizer = dlg.CreateStdDialogButtonSizer(wxOK | wxCANCEL);
    sizer->Add(btnSizer, 0, wxALL | wxALIGN_CENTER, 10);

    dlg.SetSizer(sizer);
    dlg.Centre();

    if (dlg.ShowModal() == wxID_OK)
    {
        int selectedMonth = monthChoice->GetSelection() + 1;
        int selectedYear = yearSpin->GetValue();

        // Log para debug
        wxLogDebug(wxT("Importando despesas fixas para %02d/%04d"), selectedMonth, selectedYear);

        ExpenseDAO dao;
        if (dao.ImportFixedExpenses(selectedMonth, selectedYear))
        {
            // Limpar todos os filtros
            ClearAllFilters();

            // Recarregar a lista
            LoadExpenses();

            // Informar o usuário
            wxString monthName = wxDateTime::GetMonthName((wxDateTime::Month)(selectedMonth - 1));
            wxMessageBox(
                wxString::Format(
                    wxT("As despesas fixas de %s/%d foram importadas com sucesso!\n\n")
                    wxT("Todos os filtros foram limpos para mostrar todas as despesas cadastradas."),
                    monthName,
                    selectedYear
                ),
                wxT("Importação Concluída"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
    }
}

void ExpensesPanel::OnImportInstallments(wxCommandEvent& event)
{
    // Diálogo para selecionar mês/ano
    wxDialog dlg(this, wxID_ANY, wxT("Importar Despesas Parceladas"),
                 wxDefaultPosition, wxSize(400, 500));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(&dlg, wxID_ANY,
        wxT("Selecione o mês/ano para importar as parcelas das despesas parceladas:"));
    sizer->Add(label, 0, wxALL, 10);

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    wxArrayString months;
    for (int i = 1; i <= 12; i++)
    {
        months.Add(wxDateTime::GetMonthName((wxDateTime::Month)(i-1)));
    }

    wxChoice* monthChoice = new wxChoice(&dlg, wxID_ANY, wxDefaultPosition,
                                        wxDefaultSize, months);
    monthChoice->SetSelection(wxDateTime::Now().GetMonth());

    wxSpinCtrl* yearSpin = new wxSpinCtrl(&dlg, wxID_ANY, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxSP_ARROW_KEYS, 1800, 3000,
                                          wxDateTime::Now().GetYear());

    gridSizer->Add(new wxStaticText(&dlg, wxID_ANY, wxT("Mês:")),
                   0, wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(monthChoice, 1, wxEXPAND);
    gridSizer->Add(new wxStaticText(&dlg, wxID_ANY, wxT("Ano:")),
                   0, wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(yearSpin, 1, wxEXPAND);

    sizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Nota informativa
    wxStaticText* noteText = new wxStaticText(&dlg, wxID_ANY,
        wxT("O sistema irá:\n\n")
        wxT("1. Buscar todas as despesas parceladas ATIVAS\n")
        wxT("2. Calcular automaticamente as datas de vencimento\n")
        wxT("   baseadas no ciclo de cada cartão\n")
        wxT("3. Importar apenas as parcelas que vencem no mês\n")
        wxT("   selecionado\n")
        wxT("4. Pular parcelas que já foram importadas\n\n")
        wxT("Nota: Despesas que já existem com a mesma data\n")
        wxT("e descrição não serão importadas novamente."));
    wxFont noteFont = noteText->GetFont();
    noteFont.SetPointSize(noteFont.GetPointSize() - 1);
    noteText->SetFont(noteFont);
    noteText->SetForegroundColour(wxColour(100, 100, 100));
    sizer->Add(noteText, 0, wxALL | wxEXPAND, 10);

    wxStdDialogButtonSizer* btnSizer = dlg.CreateStdDialogButtonSizer(wxOK | wxCANCEL);
    sizer->Add(btnSizer, 0, wxALL | wxALIGN_CENTER, 10);

    dlg.SetSizer(sizer);
    dlg.Centre();

    if (dlg.ShowModal() == wxID_OK)
    {
        int selectedMonth = monthChoice->GetSelection() + 1;
        int selectedYear = yearSpin->GetValue();

        // Log para debug
        wxLogDebug(wxT("Importando parcelas para %02d/%04d"), selectedMonth, selectedYear);

        ExpenseDAO dao;
        if (dao.ImportInstallmentExpenses(selectedMonth, selectedYear))
        {
            // Limpar todos os filtros
            ClearAllFilters();

            // Recarregar a lista
            LoadExpenses();

            // Informar o usuário
            wxString monthName = wxDateTime::GetMonthName((wxDateTime::Month)(selectedMonth - 1));
            wxMessageBox(
                wxString::Format(
                    wxT("As parcelas de %s/%d foram importadas com sucesso!\n\n")
                    wxT("Todos os filtros foram limpos para mostrar todas as despesas cadastradas."),
                    monthName,
                    selectedYear
                ),
                wxT("Importação Concluída"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
    }
}

void ExpensesPanel::OnFilter(wxCommandEvent& event)
{
    LoadExpenses();
}

void ExpensesPanel::OnClearFilter(wxCommandEvent& event)
{
    ClearAllFilters();
    LoadExpenses();
}

void ExpensesPanel::ClearAllFilters()
{
    m_monthChoice->SetSelection(0);                      // "Todos"
    m_yearSpin->SetValue(wxDateTime::Now().GetYear());  // Ano atual
    m_expenseTypeChoice->SetSelection(0);                // "Todos"
    m_paymentMethodChoice->SetSelection(0);              // "Todos"
    m_statusChoice->SetSelection(0);                     // "Todos"
}

void ExpensesPanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditExpense(evt);
}

// Método para lidar com clique na coluna
void ExpensesPanel::OnColumnClick(wxListEvent& event)
{
    int column = event.GetColumn();

    // Se clicar na mesma coluna, inverter a ordenação
    if (column == m_sortColumn)
    {
        m_sortAscending = !m_sortAscending;
    }
    else
    {
        // Nova coluna, começar com ordem crescente
        m_sortColumn = column;
        m_sortAscending = true;
    }

    SortByColumn(column);
    RefreshListView();
}

// Método para ordenar por coluna
void ExpensesPanel::SortByColumn(int column)
{
    if (m_expenses.empty()) return;

    // Ordenar usando std::sort com comparador customizado
    std::sort(m_expenses.begin(), m_expenses.end(),
              [this, column](const Expense& a, const Expense& b)
              {
                  return CompareExpenses(a, b, column, m_sortAscending) < 0;
              });
}

// Comparador estático
int ExpensesPanel::CompareExpenses(const Expense& e1, const Expense& e2, int column, bool ascending)
{
    int result = 0;

    switch (column)
    {
        case 0: // Data de Vencimento
            if (e1.dueDate < e2.dueDate) result = -1;
            else if (e1.dueDate > e2.dueDate) result = 1;
            break;

        case 1: // Descrição
            result = e1.description.CmpNoCase(e2.description);
            break;

        case 2: // Valor
            if (e1.amount < e2.amount) result = -1;
            else if (e1.amount > e2.amount) result = 1;
            break;

        case 3: // Tipo de Despesa
            result = e1.expenseType.CmpNoCase(e2.expenseType);
            break;

        case 4: // Forma de Pagamento
            result = e1.paymentMethod.CmpNoCase(e2.paymentMethod);
            break;

        case 5: // Categoria de Pagamento
            result = e1.paymentCategory.CmpNoCase(e2.paymentCategory);
            break;

        case 6: // Data de Pagamento
            if (!e1.paymentDate.IsValid() && !e2.paymentDate.IsValid()) result = 0;
            else if (!e1.paymentDate.IsValid()) result = 1;
            else if (!e2.paymentDate.IsValid()) result = -1;
            else if (e1.paymentDate < e2.paymentDate) result = -1;
            else if (e1.paymentDate > e2.paymentDate) result = 1;
            break;

        case 7: // Situação
            result = ExpenseStatusToString(e1.status).CmpNoCase(ExpenseStatusToString(e2.status));
            break;

        case 8: // Valor Reajustado
            if (e1.adjustedAmount < e2.adjustedAmount) result = -1;
            else if (e1.adjustedAmount > e2.adjustedAmount) result = 1;
            break;

        case 9: // Valor a Pagar
        {
            double value1 = e1.adjustedAmount > 0 ? e1.adjustedAmount : e1.amount;
            double value2 = e2.adjustedAmount > 0 ? e2.adjustedAmount : e2.amount;
            if (value1 < value2) result = -1;
            else if (value1 > value2) result = 1;
            break;
        }
    }

    // Inverter resultado se for ordem decrescente
    if (!ascending)
    {
        result = -result;
    }

    return result;
}

// Método para atualizar apenas a visualização da lista
void ExpensesPanel::RefreshListView()
{
    m_listView->DeleteAllItems();

    // Variáveis para totais
    double totalAmount = 0.0;
    double totalPaid = 0.0;      // Pago + Atrasado
    double totalPending = 0.0;    // A pagar + Em dívida
    int countPaid = 0;
    int countToPay = 0;
    int countLate = 0;
    int countInDebt = 0;

    // Preencher lista com dados ordenados
    for (size_t i = 0; i < m_expenses.size(); i++)
    {
        const Expense& expense = m_expenses[i];

        // Calcular valor a pagar
        double valuePay = expense.adjustedAmount > 0 ? expense.adjustedAmount : expense.amount;

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(expense.dueDate));
        m_listView->SetItem(itemIndex, 1, expense.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(expense.amount));
        m_listView->SetItem(itemIndex, 3, expense.expenseType);
        m_listView->SetItem(itemIndex, 4, expense.paymentMethod);
        m_listView->SetItem(itemIndex, 5, expense.paymentCategory);
        m_listView->SetItem(itemIndex, 6, DateUtils::FormatDate(expense.paymentDate));
        m_listView->SetItem(itemIndex, 7, ExpenseStatusToString(expense.status));

        if (expense.adjustedAmount > 0)
        {
            m_listView->SetItem(itemIndex, 8, MoneyUtils::FormatMoney(expense.adjustedAmount));
        }

        m_listView->SetItem(itemIndex, 9, MoneyUtils::FormatMoney(valuePay));

        // Colorir linha baseado no status
        wxColour color;
        switch (expense.status)
        {
            case ExpenseStatus::PAID:
                color = wxColour(220, 255, 220);
                countPaid++;
                break;
            case ExpenseStatus::LATE:
                color = wxColour(255, 255, 200);
                countLate++;
                break;
            case ExpenseStatus::IN_DEBT:
                color = wxColour(255, 220, 220);
                countInDebt++;
                break;
            default:
                color = *wxWHITE;
                countToPay++;
        }

        m_listView->SetItemBackgroundColour(itemIndex, color);

        // Calcular totais
        totalAmount += valuePay;

        switch (expense.status)
        {
            case ExpenseStatus::PAID:
            case ExpenseStatus::LATE:
                totalPaid += valuePay;
                break;
            case ExpenseStatus::TO_PAY:
            case ExpenseStatus::IN_DEBT:
                totalPending += valuePay;
                break;
        }
    }

    // Ajustar largura das colunas
    if (m_expenses.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 125); // Data Vencimento
        m_listView->SetColumnWidth(1, 250); // Descrição
        m_listView->SetColumnWidth(2, 100); // Valor
        m_listView->SetColumnWidth(3, 130); // Tipo de Despesa
        m_listView->SetColumnWidth(4, 155); // Forma de Pagamento
        m_listView->SetColumnWidth(5, 170); // Categoria de Pagamento
        m_listView->SetColumnWidth(6, 145); // Data de Pagamento
        m_listView->SetColumnWidth(7, 90);  // Situação
        m_listView->SetColumnWidth(8, 130); // Valor Reajustado
    }

    // Adicionar indicador visual de ordenação no cabeçalho
    if (m_sortColumn >= 0)
    {
        wxListItem item;
        item.SetMask(wxLIST_MASK_TEXT);
        m_listView->GetColumn(m_sortColumn, item);

        wxString text = item.GetText();
        // Remover indicadores anteriores se existirem
        text.Replace(wxT(" ▲"), wxT(""));
        text.Replace(wxT(" ▼"), wxT(""));

        // Adicionar novo indicador
        text += m_sortAscending ? wxT(" ▲") : wxT(" ▼");

        item.SetText(text);
        m_listView->SetColumn(m_sortColumn, item);

        // AJUSTAR LARGURA PARA COLUNAS ESPECÍFICAS QUANDO ORDENADAS
        int currentWidth = m_listView->GetColumnWidth(m_sortColumn);
        int extraWidth = 20; // Espaço extra para o indicador

        switch (m_sortColumn)
        {
            case 4: // Forma de Pagamento
                if (currentWidth < 175)
                {
                    m_listView->SetColumnWidth(m_sortColumn, 175);
                }
                else
                {
                    m_listView->SetColumnWidth(m_sortColumn, currentWidth + extraWidth);
                }
                break;

            case 5: // Categoria de Pagamento
                if (currentWidth < 190)
                {
                    m_listView->SetColumnWidth(m_sortColumn, 190);
                }
                else
                {
                    m_listView->SetColumnWidth(m_sortColumn, currentWidth + extraWidth);
                }
                break;

            case 6: // Data de Pagamento
                if (currentWidth < 165)
                {
                    m_listView->SetColumnWidth(m_sortColumn, 165);
                }
                else
                {
                    m_listView->SetColumnWidth(m_sortColumn, currentWidth + extraWidth);
                }
                break;

            default:
                // Para outras colunas, apenas adicionar um pequeno espaço extra
                m_listView->SetColumnWidth(m_sortColumn, currentWidth + extraWidth);
                break;
        }
    }
}
