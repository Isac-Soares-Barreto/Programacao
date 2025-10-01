#include "InstallmentExpensesPanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/DateUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/InstallmentExpenseDAO.h"
#include "../dialogs/InstallmentExpenseDialog.h"
#include <wx/listctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 900,
    ID_AddButton,
    ID_EditButton,
    ID_DeleteButton,
    ID_ToggleActiveButton
};

// Método auxiliar para obter o índice do item selecionado
static long GetSelectedItemIndex(wxListCtrl* listCtrl)
{
    return listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

InstallmentExpensesPanel::InstallmentExpensesPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateInterface();
    LoadInstallmentExpenses();
}

InstallmentExpensesPanel::~InstallmentExpensesPanel()
{
}

void InstallmentExpensesPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Despesas Parceladas"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);

    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    // Descrição
    wxStaticText* description = new wxStaticText(this, wxID_ANY,
        wxT("Despesas parceladas são compras no cartão de crédito que serão pagas em múltiplas parcelas.\n")
        wxT("Use o botão 'Importar Parceladas' na tela de Despesas para gerar as parcelas automaticamente."));
    wxFont descFont = description->GetFont();
    descFont.SetPointSize(descFont.GetPointSize() - 1);
    description->SetFont(descFont);
    description->SetForegroundColour(wxColour(100, 100, 100));

    mainSizer->Add(description, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Barra de ferramentas
    wxBoxSizer* toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnAdd = ButtonUtils::CreateButton(this, wxT("Adicionar"), wxT("add.png"));
    btnAdd->SetId(ID_AddButton);
    wxButton* btnEdit = ButtonUtils::CreateButton(this, wxT("Editar"), wxT("edit.png"));
    btnEdit->SetId(ID_EditButton);
    wxButton* btnDelete = ButtonUtils::CreateButton(this, wxT("Excluir"), wxT("delete.png"));
    btnDelete->SetId(ID_DeleteButton);
    wxButton* btnToggleActive = ButtonUtils::CreateButton(this, wxT("Ativar/Desativar"), wxT("check.png"));
    btnToggleActive->SetId(ID_ToggleActiveButton);

    toolbarSizer->Add(btnAdd, 0, wxALL, 5);
    toolbarSizer->Add(btnEdit, 0, wxALL, 5);
    toolbarSizer->Add(btnDelete, 0, wxALL, 5);
    toolbarSizer->AddStretchSpacer();
    toolbarSizer->Add(btnToggleActive, 0, wxALL, 5);

    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Lista de despesas parceladas
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    // Adicionar colunas
    m_listView->AppendColumn(wxT("Data Compra"), wxLIST_FORMAT_LEFT, 110);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 200);
    m_listView->AppendColumn(wxT("Valor Total"), wxLIST_FORMAT_RIGHT, 110);
    m_listView->AppendColumn(wxT("Qtde. Parcelas"), wxLIST_FORMAT_CENTER, 110);
    m_listView->AppendColumn(wxT("Tipo de Despesa"), wxLIST_FORMAT_LEFT, 130);
    m_listView->AppendColumn(wxT("Cartão"), wxLIST_FORMAT_LEFT, 120);
    m_listView->AppendColumn(wxT("Data Vencimento"), wxLIST_FORMAT_LEFT, 130);
    m_listView->AppendColumn(wxT("Valor Parcela"), wxLIST_FORMAT_RIGHT, 110);
    m_listView->AppendColumn(wxT("Ativa"), wxLIST_FORMAT_CENTER, 60);

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

    // Bind dos eventos
    Bind(wxEVT_BUTTON, &InstallmentExpensesPanel::OnAddInstallmentExpense, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &InstallmentExpensesPanel::OnEditInstallmentExpense, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &InstallmentExpensesPanel::OnDeleteInstallmentExpense, this, ID_DeleteButton);
    Bind(wxEVT_BUTTON, &InstallmentExpensesPanel::OnToggleActive, this, ID_ToggleActiveButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &InstallmentExpensesPanel::OnItemActivated, this, ID_ListView);
}

void InstallmentExpensesPanel::LoadInstallmentExpenses()
{
    m_listView->DeleteAllItems();
    m_installmentExpenses.clear();

    InstallmentExpenseDAO dao;
    m_installmentExpenses = dao.GetAll();

    // Preencher lista
    double totalActive = 0.0;
    double totalInactive = 0.0;
    int countActive = 0;
    int countInactive = 0;

    for (size_t i = 0; i < m_installmentExpenses.size(); i++)
    {
        const InstallmentExpense& expense = m_installmentExpenses[i];

        long itemIndex = m_listView->InsertItem(i, DateUtils::FormatDate(expense.purchaseDate));
        m_listView->SetItem(itemIndex, 1, expense.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(expense.totalAmount));
        m_listView->SetItem(itemIndex, 3, wxString::Format(wxT("%d"), expense.installments));
        m_listView->SetItem(itemIndex, 4, expense.expenseType);
        m_listView->SetItem(itemIndex, 5, expense.cardName);
        m_listView->SetItem(itemIndex, 6, DateUtils::FormatDate(expense.dueDate));
        m_listView->SetItem(itemIndex, 7, MoneyUtils::FormatMoney(expense.installmentAmount));
        m_listView->SetItem(itemIndex, 8, expense.active ? wxT("Sim") : wxT("Não"));

        // Colorir linha baseado no status
        if (expense.active)
        {
            totalActive += expense.totalAmount;
            countActive++;
        }
        else
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(240, 240, 240));
            m_listView->SetItemTextColour(itemIndex, wxColour(150, 150, 150));
            totalInactive += expense.totalAmount;
            countInactive++;
        }
    }

    // Ajustar largura das colunas
    if (m_installmentExpenses.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 110); // Data Compra
        m_listView->SetColumnWidth(1, 200); // Descrição
        m_listView->SetColumnWidth(2, 110); // Valor Total
        m_listView->SetColumnWidth(3, 110); // Qtde. Parcelas
        m_listView->SetColumnWidth(4, 130); // Tipo de Despesa
        m_listView->SetColumnWidth(5, 120); // Cartão
        m_listView->SetColumnWidth(6, 130); // Data Vencimento
        m_listView->SetColumnWidth(7, 110); // Valor Parcela
        m_listView->SetColumnWidth(8, 60);  // Ativa
    }

    // Atualizar resumo
    wxString summary = wxString::Format(
        wxT("Total: %d despesa(s) parcelada(s) | Ativas: %d (%s) | Inativas: %d (%s) | Total Geral: %s"),
        (int)m_installmentExpenses.size(),
        countActive, MoneyUtils::FormatMoney(totalActive),
        countInactive, MoneyUtils::FormatMoney(totalInactive),
        MoneyUtils::FormatMoney(totalActive + totalInactive)
    );

    m_summaryText->SetLabel(summary);
    m_summaryText->InvalidateBestSize();
    m_summaryText->GetParent()->Layout();

    wxSize bestSize = m_summaryText->GetBestSize();
    m_summaryText->SetMinSize(bestSize);

    this->Layout();
    this->Refresh();
}

void InstallmentExpensesPanel::OnAddInstallmentExpense(wxCommandEvent& event)
{
    InstallmentExpenseDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        InstallmentExpense expense = dialog.GetInstallmentExpense();
        InstallmentExpenseDAO dao;

        if (dao.Insert(expense))
        {
            LoadInstallmentExpenses();
            wxMessageBox(
                wxString::Format(
                    wxT("Despesa parcelada cadastrada com sucesso!\n\n")
                    wxT("%s\n")
                    wxT("Valor total: %s\n")
                    wxT("Parcelas: %dx de %s\n\n")
                    wxT("Use o botão 'Importar Parceladas' na tela de Despesas\n")
                    wxT("para gerar as parcelas no módulo de despesas."),
                    expense.description,
                    MoneyUtils::FormatMoney(expense.totalAmount),
                    expense.installments,
                    MoneyUtils::FormatMoney(expense.CalculateInstallmentAmount())
                ),
                wxT("Sucesso"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar despesa parcelada!\n\nVerifique se já não existe uma despesa\ncom a mesma data, descrição e cartão."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void InstallmentExpensesPanel::OnEditInstallmentExpense(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa parcelada para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    InstallmentExpense& oldExpense = m_installmentExpenses[selectedIndex];
    InstallmentExpenseDialog dialog(this, oldExpense);

    if (dialog.ShowModal() == wxID_OK)
    {
        InstallmentExpense newExpense = dialog.GetInstallmentExpense();
        InstallmentExpenseDAO dao;

        if (dao.Update(oldExpense, newExpense))
        {
            LoadInstallmentExpenses();
            wxMessageBox(wxT("Despesa parcelada atualizada com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar despesa parcelada!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void InstallmentExpensesPanel::OnDeleteInstallmentExpense(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa parcelada para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    InstallmentExpense& expense = m_installmentExpenses[selectedIndex];

    wxString message = wxString::Format(
        wxT("Confirma a exclusão da despesa parcelada:\n\n")
        wxT("%s\n")
        wxT("Data: %s\n")
        wxT("Valor total: %s\n")
        wxT("Parcelas: %dx de %s\n")
        wxT("Cartão: %s\n\n")
        wxT("⚠ ATENÇÃO: Esta ação não pode ser desfeita!"),
        expense.description,
        DateUtils::FormatDate(expense.purchaseDate),
        MoneyUtils::FormatMoney(expense.totalAmount),
        expense.installments,
        MoneyUtils::FormatMoney(expense.installmentAmount),
        expense.cardName
    );

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        InstallmentExpenseDAO dao;

        if (dao.Delete(expense.purchaseDate, expense.description, expense.cardName))
        {
            LoadInstallmentExpenses();
            wxMessageBox(wxT("Despesa parcelada excluída com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir despesa parcelada!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void InstallmentExpensesPanel::OnToggleActive(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa parcelada para ativar/desativar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    InstallmentExpense& expense = m_installmentExpenses[selectedIndex];
    InstallmentExpenseDAO dao;

    bool newStatus = !expense.active;

    if (dao.SetActive(expense.purchaseDate, expense.description, expense.cardName, newStatus))
    {
        wxString message = wxString::Format(
            wxT("Despesa parcelada '%s' foi %s com sucesso!"),
            expense.description,
            newStatus ? wxT("ativada") : wxT("desativada")
        );

        wxMessageBox(message, wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        LoadInstallmentExpenses();
    }
    else
    {
        wxMessageBox(wxT("Erro ao alterar status da despesa parcelada!"),
                    wxT("Erro"), wxOK | wxICON_ERROR, this);
    }
}

void InstallmentExpensesPanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditInstallmentExpense(evt);
}
