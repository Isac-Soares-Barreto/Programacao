#include "InvestmentsPanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/DateUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/InvestmentDAO.h"
#include "../databases/Database.h"
#include "../dialogs/InvestmentDialog.h"
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 700,
    ID_AddButton,
    ID_EditButton,
    ID_DeleteButton,
    ID_FilterMonth,
    ID_FilterYear,
    ID_FilterInvestmentType,
    ID_FilterSource,
    ID_ClearFilterButton
};

// Método auxiliar para obter o índice do item selecionado
static long GetSelectedItemIndex(wxListCtrl* listCtrl)
{
    return listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

InvestmentsPanel::InvestmentsPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , m_sortColumn(-1)
    , m_sortAscending(true)
{
    CreateInterface();
    LoadInvestments();
}

InvestmentsPanel::~InvestmentsPanel()
{
}

void InvestmentsPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Investimentos"));
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

    // Tipo de Investimento
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Tipo:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 5);

    m_investmentTypeChoice = new wxChoice(this, ID_FilterInvestmentType,
                                          wxDefaultPosition, wxSize(120, -1));
    m_investmentTypeChoice->Append(wxT("Todos"));
    filterBox->Add(m_investmentTypeChoice, 0, wxRIGHT, 10);

    // Fonte
    filterBox->Add(new wxStaticText(this, wxID_ANY, wxT("Fonte:")),
                   0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    m_sourceChoice = new wxChoice(this, ID_FilterSource,
                                  wxDefaultPosition, wxSize(120, -1));
    m_sourceChoice->Append(wxT("Todos"));
    filterBox->Add(m_sourceChoice, 0, wxRIGHT, 15);

    // Separador visual
    filterBox->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxLI_VERTICAL),
                   0, wxEXPAND | wxRIGHT | wxLEFT, 5);

    // Botão limpar filtros
    wxButton* btnClearFilter = ButtonUtils::CreateButton(this, wxT("Limpar Filtros"), wxT("filter.png"));
    btnClearFilter->SetId(ID_ClearFilterButton);
    filterBox->Add(btnClearFilter, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    mainSizer->Add(filterBox, 0, wxEXPAND | wxALL, 10);

    // Lista de investimentos
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    // Adicionar colunas
    m_listView->AppendColumn(wxT("Data Investimento"), wxLIST_FORMAT_LEFT, 140);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 300);
    m_listView->AppendColumn(wxT("Valor"), wxLIST_FORMAT_RIGHT, 120);
    m_listView->AppendColumn(wxT("Tipo de Investimento"), wxLIST_FORMAT_LEFT, 150);
    m_listView->AppendColumn(wxT("Fonte"), wxLIST_FORMAT_LEFT, 150);

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
    Bind(wxEVT_BUTTON, &InvestmentsPanel::OnAddInvestment, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &InvestmentsPanel::OnEditInvestment, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &InvestmentsPanel::OnDeleteInvestment, this, ID_DeleteButton);
    Bind(wxEVT_BUTTON, &InvestmentsPanel::OnClearFilter, this, ID_ClearFilterButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &InvestmentsPanel::OnItemActivated, this, ID_ListView);
    Bind(wxEVT_LIST_COL_CLICK, &InvestmentsPanel::OnColumnClick, this, ID_ListView);

    // Bind para aplicar filtro automaticamente
    Bind(wxEVT_CHOICE, &InvestmentsPanel::OnFilter, this, ID_FilterMonth);
    Bind(wxEVT_SPINCTRL, &InvestmentsPanel::OnFilter, this, ID_FilterYear);
    Bind(wxEVT_CHOICE, &InvestmentsPanel::OnFilter, this, ID_FilterInvestmentType);
    Bind(wxEVT_CHOICE, &InvestmentsPanel::OnFilter, this, ID_FilterSource);
}

void InvestmentsPanel::LoadFilterCombos()
{
    Database& db = Database::GetInstance();

    // Carregar tipos de investimento
    auto result = db.Query(wxT("SELECT DISTINCT name FROM investment_types ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_investmentTypeChoice->Append(row[0]);
        }
    }
    m_investmentTypeChoice->SetSelection(0);

    // Carregar fontes
    result = db.Query(wxT("SELECT DISTINCT name FROM income_destinations ORDER BY name"));
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            m_sourceChoice->Append(row[0]);
        }
    }
    m_sourceChoice->SetSelection(0);
}

void InvestmentsPanel::LoadInvestments()
{
    m_listView->DeleteAllItems();
    m_investments.clear();

    InvestmentDAO dao;

    // Obter valores dos filtros
    int monthIndex = m_monthChoice->GetSelection();
    int year = m_yearSpin->GetValue();
    wxString investmentType = m_investmentTypeChoice->GetStringSelection();
    wxString source = m_sourceChoice->GetStringSelection();

    // Aplicar filtros
    if (monthIndex == 0)
    {
        m_investments = dao.GetAll();
    }
    else
    {
        m_investments = dao.GetByMonth(monthIndex, year);
    }

    // Filtrar por tipo de investimento
    if (investmentType != wxT("Todos") && !investmentType.IsEmpty())
    {
        std::vector<Investment> filtered;
        for (const auto& investment : m_investments)
        {
            if (investment.investmentType == investmentType)
            {
                filtered.push_back(investment);
            }
        }
        m_investments = filtered;
    }

    // Filtrar por fonte
    if (source != wxT("Todos") && !source.IsEmpty())
    {
        std::vector<Investment> filtered;
        for (const auto& investment : m_investments)
        {
            if (investment.source == source)
            {
                filtered.push_back(investment);
            }
        }
        m_investments = filtered;
    }

    // Preencher lista
    double totalAmount = 0.0;

    for (size_t i = 0; i < m_investments.size(); i++)
    {
        const Investment& investment = m_investments[i];

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(investment.investmentDate));
        m_listView->SetItem(itemIndex, 1, investment.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(investment.amount));
        m_listView->SetItem(itemIndex, 3, investment.investmentType);
        m_listView->SetItem(itemIndex, 4, investment.source);

        // Colorir linhas alternadas
        if (i % 2 == 1)
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(240, 245, 255)); // Azul claro
        }

        totalAmount += investment.amount;
    }

    // Ajustar largura das colunas
    if (m_investments.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 140); // Data Investimento
        m_listView->SetColumnWidth(1, 300); // Descrição
        m_listView->SetColumnWidth(2, 120); // Valor
        m_listView->SetColumnWidth(3, 150); // Tipo de Investimento
        m_listView->SetColumnWidth(4, 150); // Fonte
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
    if (investmentType != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Tipo: ") + investmentType;
    }
    if (source != wxT("Todos"))
    {
        if (!filterInfo.IsEmpty()) filterInfo += wxT(" | ");
        filterInfo += wxT("Fonte: ") + source;
    }

    if (!filterInfo.IsEmpty())
    {
        summary = wxT("Filtros: ") + filterInfo + wxT("\n");
    }

    summary += wxString::Format(
        wxT("Total: %d investimento(s) | Valor Total Investido: %s"),
        (int)m_investments.size(),
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

void InvestmentsPanel::OnAddInvestment(wxCommandEvent& event)
{
    InvestmentDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        Investment investment = dialog.GetInvestment();
        InvestmentDAO dao;

        if (dao.Insert(investment))
        {
            LoadInvestments();
            wxMessageBox(wxT("Investimento adicionado com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar investimento!\n\nVerifique se já não existe um investimento\ncom a mesma data e descrição."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void InvestmentsPanel::OnEditInvestment(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione um investimento para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Investment& oldInvestment = m_investments[selectedIndex];
    InvestmentDialog dialog(this, oldInvestment);

    if (dialog.ShowModal() == wxID_OK)
    {
        Investment newInvestment = dialog.GetInvestment();
        InvestmentDAO dao;

        if (dao.Update(oldInvestment, newInvestment))
        {
            LoadInvestments();
            wxMessageBox(wxT("Investimento atualizado com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar investimento!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void InvestmentsPanel::OnDeleteInvestment(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione um investimento para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Investment& investment = m_investments[selectedIndex];

    wxString message = wxString::Format(
        wxT("Confirma a exclusão do investimento:\n\n%s\nData: %s\nValor: %s"),
        investment.description,
        DateUtils::FormatDate(investment.investmentDate),
        MoneyUtils::FormatMoney(investment.amount)
    );

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        InvestmentDAO dao;

        if (dao.Delete(investment.investmentDate, investment.description))
        {
            LoadInvestments();
            wxMessageBox(wxT("Investimento excluído com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir investimento!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void InvestmentsPanel::OnFilter(wxCommandEvent& event)
{
    LoadInvestments();
}

void InvestmentsPanel::OnClearFilter(wxCommandEvent& event)
{
    ClearAllFilters();
    LoadInvestments();
}

void InvestmentsPanel::ClearAllFilters()
{
    m_monthChoice->SetSelection(0);                      // "Todos"
    m_yearSpin->SetValue(wxDateTime::Now().GetYear());  // Ano atual
    m_investmentTypeChoice->SetSelection(0);             // "Todos"
    m_sourceChoice->SetSelection(0);                     // "Todos"
}

void InvestmentsPanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditInvestment(evt);
}

void InvestmentsPanel::OnColumnClick(wxListEvent& event)
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

void InvestmentsPanel::SortByColumn(int column)
{
    if (m_investments.empty()) return;

    // Ordenar usando std::sort com comparador customizado
    std::sort(m_investments.begin(), m_investments.end(),
              [this, column](const Investment& a, const Investment& b)
              {
                  return CompareInvestments(a, b, column, m_sortAscending) < 0;
              });
}

int InvestmentsPanel::CompareInvestments(const Investment& i1, const Investment& i2, int column, bool ascending)
{
    int result = 0;

    switch (column)
    {
        case 0: // Data do Investimento
            if (i1.investmentDate < i2.investmentDate) result = -1;
            else if (i1.investmentDate > i2.investmentDate) result = 1;
            break;

        case 1: // Descrição
            result = i1.description.CmpNoCase(i2.description);
            break;

        case 2: // Valor
            if (i1.amount < i2.amount) result = -1;
            else if (i1.amount > i2.amount) result = 1;
            break;

        case 3: // Tipo de Investimento
            result = i1.investmentType.CmpNoCase(i2.investmentType);
            break;

        case 4: // Fonte
            result = i1.source.CmpNoCase(i2.source);
            break;
    }

    // Inverter resultado se for ordem decrescente
    if (!ascending)
    {
        result = -result;
    }

    return result;
}

void InvestmentsPanel::RefreshListView()
{
    m_listView->DeleteAllItems();

    double totalAmount = 0.0;

    // Preencher lista com dados ordenados
    for (size_t i = 0; i < m_investments.size(); i++)
    {
        const Investment& investment = m_investments[i];

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(investment.investmentDate));
        m_listView->SetItem(itemIndex, 1, investment.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(investment.amount));
        m_listView->SetItem(itemIndex, 3, investment.investmentType);
        m_listView->SetItem(itemIndex, 4, investment.source);

        // Colorir linhas alternadas
        if (i % 2 == 1)
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(240, 245, 255));
        }

        totalAmount += investment.amount;
    }

    // Ajustar largura das colunas
    if (m_investments.size() > 0)
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
