#include "IncomePanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/DateUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/IncomeDAO.h"
#include "../databases/Database.h"
#include "../dialogs/IncomeDialog.h"
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 600,
    ID_AddButton,
    ID_EditButton,
    ID_DeleteButton,
    ID_FilterMonth,
    ID_FilterYear,
    ID_FilterIncomeType,
    ID_FilterDestination,
    ID_ClearFilterButton
};

// Método auxiliar para obter o índice do item selecionado
static long GetSelectedItemIndex(wxListCtrl* listCtrl)
{
    return listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

IncomePanel::IncomePanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , m_sortColumn(-1)
    , m_sortAscending(true)
{
    CreateInterface();
    LoadIncomes();
}

IncomePanel::~IncomePanel()
{
}

void IncomePanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Receitas"));
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

    toolbarSizer->Add(btnAdd, 0, wxALL, 5);
    toolbarSizer->Add(btnEdit, 0, wxALL, 5);
    toolbarSizer->Add(btnDelete, 0, wxALL, 5);
    toolbarSizer->AddStretchSpacer();

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

    // Tipo de Receita
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Tipo:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);

    m_incomeTypeChoice = new wxChoice(this, ID_FilterIncomeType,
                                      wxDefaultPosition, wxSize(120, -1));
    m_incomeTypeChoice->Append(wxT("Todos"));
    filterBox->Add(m_incomeTypeChoice, 0, wxRIGHT, 10);

    // Destino
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Destino:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    m_destinationChoice = new wxChoice(this, ID_FilterDestination,
                                       wxDefaultPosition, wxSize(120, -1));
    m_destinationChoice->Append(wxT("Todos"));
    filterBox->Add(m_destinationChoice, 0, wxRIGHT, 15);

    // Separador visual
    filterBox->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_VERTICAL),
                   0, wxEXPAND | wxRIGHT | wxLEFT, 5);

    // Botão limpar filtros
    wxButton* btnClearFilter = ButtonUtils::CreateButton(this, wxT("Limpar Filtros"), wxT("filter.png"));
    btnClearFilter->SetId(ID_ClearFilterButton);
    filterBox->Add(btnClearFilter, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    mainSizer->Add(filterBox, 0, wxEXPAND | wxALL, 10);

    // Lista de receitas
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    // Adicionar colunas
    m_listView->AppendColumn(wxT("Data Recebimento"), wxLIST_FORMAT_LEFT, 140);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 300);
    m_listView->AppendColumn(wxT("Valor"), wxLIST_FORMAT_RIGHT, 120);
    m_listView->AppendColumn(wxT("Tipo de Receita"), wxLIST_FORMAT_LEFT, 150);
    m_listView->AppendColumn(wxT("Destino"), wxLIST_FORMAT_LEFT, 150);

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
    Bind(wxEVT_BUTTON, &IncomePanel::OnAddIncome, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &IncomePanel::OnEditIncome, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &IncomePanel::OnDeleteIncome, this, ID_DeleteButton);
    Bind(wxEVT_BUTTON, &IncomePanel::OnClearFilter, this, ID_ClearFilterButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &IncomePanel::OnItemActivated, this, ID_ListView);
    Bind(wxEVT_LIST_COL_CLICK, &IncomePanel::OnColumnClick, this, ID_ListView);

    // Bind para aplicar filtro automaticamente
    Bind(wxEVT_CHOICE, &IncomePanel::OnFilter, this, ID_FilterMonth);
    Bind(wxEVT_SPINCTRL, &IncomePanel::OnFilter, this, ID_FilterYear);
    Bind(wxEVT_CHOICE, &IncomePanel::OnFilter, this, ID_FilterIncomeType);
    Bind(wxEVT_CHOICE, &IncomePanel::OnFilter, this, ID_FilterDestination);
}

void IncomePanel::LoadFilterCombos()
{
    Database& db = Database::GetInstance();

    // Carregar tipos de receita
    auto result = db.Query(wxT("SELECT DISTINCT name FROM income_categories ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_incomeTypeChoice->Append(row[0]);
        }
    }
    m_incomeTypeChoice->SetSelection(0);

    // Carregar destinos
    result = db.Query(wxT("SELECT DISTINCT name FROM income_destinations ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_destinationChoice->Append(row[0]);
        }
    }
    m_destinationChoice->SetSelection(0);
}

void IncomePanel::LoadIncomes()
{
    m_listView->DeleteAllItems();
    m_incomes.clear();

    IncomeDAO dao;

    // Obter valores dos filtros
    int monthIndex = m_monthChoice->GetSelection();
    int year = m_yearSpin->GetValue();
    wxString incomeType = m_incomeTypeChoice->GetStringSelection();
    wxString destination = m_destinationChoice->GetStringSelection();

    // Aplicar filtros
    if (monthIndex == 0)
    {
        m_incomes = dao.GetAll();
    }
    else
    {
        m_incomes = dao.GetByMonth(monthIndex, year);
    }

    // Filtrar por tipo de receita
    if (incomeType != wxT("Todos") && !incomeType.IsEmpty())
    {
        std::vector<Income> filtered;
        for (const auto& income : m_incomes)
        {
            if (income.incomeType == incomeType)
            {
                filtered.push_back(income);
            }
        }
        m_incomes = filtered;
    }

    // Filtrar por destino
    if (destination != wxT("Todos") && !destination.IsEmpty())
    {
        std::vector<Income> filtered;
        for (const auto& income : m_incomes)
        {
            if (income.destination == destination)
            {
                filtered.push_back(income);
            }
        }
        m_incomes = filtered;
    }

    // Preencher lista
    double totalAmount = 0.0;

    for (size_t i = 0; i < m_incomes.size(); i++)
    {
        const Income& income = m_incomes[i];

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(income.receiptDate));
        m_listView->SetItem(itemIndex, 1, income.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(income.amount));
        m_listView->SetItem(itemIndex, 3, income.incomeType);
        m_listView->SetItem(itemIndex, 4, income.destination);

        // Colorir linhas alternadas
        if (i % 2 == 1)
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(240, 255, 240)); // Verde claro
        }

        totalAmount += income.amount;
    }

    // Ajustar largura das colunas
    if (m_incomes.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 140); // Data Recebimento
        m_listView->SetColumnWidth(1, 300); // Descrição
        m_listView->SetColumnWidth(2, 120); // Valor
        m_listView->SetColumnWidth(3, 150); // Tipo de Receita
        m_listView->SetColumnWidth(4, 150); // Destino
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
    if (incomeType != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Tipo: ") + incomeType;
    }
    if (destination != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Destino: ") + destination;
    }

    if (!filterInfo.IsEmpty())
    {
        summary = wxT("Filtros: ") + filterInfo + wxT("\n");
    }

    summary += wxString::Format(
        wxT("Total: %d receita(s) | Valor Total: %s"),
        (int)m_incomes.size(),
        MoneyUtils::FormatMoney(totalAmount)
    );

    m_summaryText->SetLabel(summary);
    m_summaryText->InvalidateBestSize();
    m_summaryText->GetParent()->Layout();

    wxSize bestSize = m_summaryText->GetBestSize();
    m_summaryText->SetMinSize(bestSize);

    this->Layout();
    this->Refresh();
}

void IncomePanel::OnAddIncome(wxCommandEvent& event)
{
    IncomeDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        Income income = dialog.GetIncome();
        IncomeDAO dao;

        if (dao.Insert(income))
        {
            LoadIncomes();
            wxMessageBox(wxT("Receita adicionada com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar receita!\n\nVerifique se já não existe uma receita\ncom a mesma data e descrição."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void IncomePanel::OnEditIncome(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma receita para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Income& oldIncome = m_incomes[selectedIndex];
    IncomeDialog dialog(this, oldIncome);

    if (dialog.ShowModal() == wxID_OK)
    {
        Income newIncome = dialog.GetIncome();
        IncomeDAO dao;

        if (dao.Update(oldIncome, newIncome))
        {
            LoadIncomes();
            wxMessageBox(wxT("Receita atualizada com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar receita!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void IncomePanel::OnDeleteIncome(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma receita para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Income& income = m_incomes[selectedIndex];

    wxString message = wxString::Format(
        wxT("Confirma a exclusão da receita:\n\n%s\nData: %s\nValor: %s"),
        income.description,
        DateUtils::FormatDate(income.receiptDate),
        MoneyUtils::FormatMoney(income.amount)
    );

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        IncomeDAO dao;

        if (dao.Delete(income.receiptDate, income.description))
        {
            LoadIncomes();
            wxMessageBox(wxT("Receita excluída com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir receita!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void IncomePanel::OnFilter(wxCommandEvent& event)
{
    LoadIncomes();
}

void IncomePanel::OnClearFilter(wxCommandEvent& event)
{
    ClearAllFilters();
    LoadIncomes();
}

void IncomePanel::ClearAllFilters()
{
    m_monthChoice->SetSelection(0);                      // "Todos"
    m_yearSpin->SetValue(wxDateTime::Now().GetYear());  // Ano atual
    m_incomeTypeChoice->SetSelection(0);                 // "Todos"
    m_destinationChoice->SetSelection(0);                // "Todos"
}

void IncomePanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditIncome(evt);
}

void IncomePanel::OnColumnClick(wxListEvent& event)
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

void IncomePanel::SortByColumn(int column)
{
    if (m_incomes.empty()) return;

    // Ordenar usando std::sort com comparador customizado
    std::sort(m_incomes.begin(), m_incomes.end(),
              [this, column](const Income& a, const Income& b)
              {
                  return CompareIncomes(a, b, column, m_sortAscending) < 0;
              });
}

int IncomePanel::CompareIncomes(const Income& i1, const Income& i2, int column, bool ascending)
{
    int result = 0;

    switch (column)
    {
        case 0: // Data de Recebimento
            if (i1.receiptDate < i2.receiptDate) result = -1;
            else if (i1.receiptDate > i2.receiptDate) result = 1;
            break;

        case 1: // Descrição
            result = i1.description.CmpNoCase(i2.description);
            break;

        case 2: // Valor
            if (i1.amount < i2.amount) result = -1;
            else if (i1.amount > i2.amount) result = 1;
            break;

        case 3: // Tipo de Receita
            result = i1.incomeType.CmpNoCase(i2.incomeType);
            break;

        case 4: // Destino
            result = i1.destination.CmpNoCase(i2.destination);
            break;
    }

    // Inverter resultado se for ordem decrescente
    if (!ascending)
    {
        result = -result;
    }

    return result;
}

void IncomePanel::RefreshListView()
{
    m_listView->DeleteAllItems();

    double totalAmount = 0.0;

    // Preencher lista com dados ordenados
    for (size_t i = 0; i < m_incomes.size(); i++)
    {
        const Income& income = m_incomes[i];

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(income.receiptDate));
        m_listView->SetItem(itemIndex, 1, income.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(income.amount));
        m_listView->SetItem(itemIndex, 3, income.incomeType);
        m_listView->SetItem(itemIndex, 4, income.destination);

        // Colorir linhas alternadas
        if (i % 2 == 1)
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(240, 255, 240));
        }

        totalAmount += income.amount;
    }

    // Ajustar largura das colunas
    if (m_incomes.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
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

        // Ajustar largura para o indicador
        int currentWidth = m_listView->GetColumnWidth(m_sortColumn);
        m_listView->SetColumnWidth(m_sortColumn, currentWidth + 20);
    }
}
