#include "FixedExpensesPanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/FixedExpenseDAO.h"
#include "../dialogs/FixedExpenseDialog.h"
#include <wx/listctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 200,
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

FixedExpensesPanel::FixedExpensesPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateInterface();
    LoadFixedExpenses();
}

FixedExpensesPanel::~FixedExpensesPanel()
{
}

void FixedExpensesPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Despesas Fixas"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    
    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);
    
    // Descrição
    wxStaticText* description = new wxStaticText(this, wxID_ANY,
        wxT("Despesas fixas são contas que se repetem todo mês no mesmo dia.\n")
        wxT("Use o botão 'Importar Despesas Fixas' na tela de Despesas para importá-las automaticamente."));
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
    
    // Lista de despesas fixas
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);
    
    // Adicionar colunas
    m_listView->AppendColumn(wxT("Dia"), wxLIST_FORMAT_CENTER, 60);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 250);
    m_listView->AppendColumn(wxT("Valor"), wxLIST_FORMAT_RIGHT, 100);
    m_listView->AppendColumn(wxT("Tipo de Despesa"), wxLIST_FORMAT_LEFT, 130);
    m_listView->AppendColumn(wxT("Forma de Pagamento"), wxLIST_FORMAT_LEFT, 140);
    m_listView->AppendColumn(wxT("Categoria de Pagamento"), wxLIST_FORMAT_LEFT, 150);
    m_listView->AppendColumn(wxT("Ativa"), wxLIST_FORMAT_CENTER, 60);
    
    mainSizer->Add(m_listView, 1, wxALL | wxEXPAND, 10);
    
    // Resumo
    m_summaryText = new wxStaticText(this, wxID_ANY, wxT("Carregando..."));
    wxFont summaryFont = m_summaryText->GetFont();
    summaryFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_summaryText->SetFont(summaryFont);
    
    mainSizer->Add(m_summaryText, 0, wxALL | wxALIGN_RIGHT, 10);
    
    SetSizer(mainSizer);
    
    // Bind dos eventos
    Bind(wxEVT_BUTTON, &FixedExpensesPanel::OnAddFixedExpense, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &FixedExpensesPanel::OnEditFixedExpense, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &FixedExpensesPanel::OnDeleteFixedExpense, this, ID_DeleteButton);
    Bind(wxEVT_BUTTON, &FixedExpensesPanel::OnToggleActive, this, ID_ToggleActiveButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &FixedExpensesPanel::OnItemActivated, this, ID_ListView);
}

void FixedExpensesPanel::LoadFixedExpenses()
{
    m_listView->DeleteAllItems();
    m_fixedExpenses.clear();
    
    FixedExpenseDAO dao;
    m_fixedExpenses = dao.GetAll();
    
    // Preencher lista
    double totalActive = 0.0;
    double totalInactive = 0.0;
    int countActive = 0;
    int countInactive = 0;
    
    for (size_t i = 0; i < m_fixedExpenses.size(); i++)
    {
        const FixedExpense& expense = m_fixedExpenses[i];
        
        long itemIndex = m_listView->InsertItem(i, wxString::Format(wxT("%02d"), expense.dueDay));
        m_listView->SetItem(itemIndex, 1, expense.description);
        m_listView->SetItem(itemIndex, 2, MoneyUtils::FormatMoney(expense.amount));
        m_listView->SetItem(itemIndex, 3, expense.expenseType);
        m_listView->SetItem(itemIndex, 4, expense.paymentMethod);
        m_listView->SetItem(itemIndex, 5, expense.paymentCategory);
        m_listView->SetItem(itemIndex, 6, expense.active ? wxT("Sim") : wxT("Não"));
        
        // Colorir linha baseado no status
        if (expense.active)
        {
            totalActive += expense.amount;
            countActive++;
        }
        else
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(240, 240, 240));
            m_listView->SetItemTextColour(itemIndex, wxColour(150, 150, 150));
            totalInactive += expense.amount;
            countInactive++;
        }
    }
    
    // Ajustar largura das colunas
    if (m_fixedExpenses.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    
    // Atualizar resumo
    wxString summary = wxString::Format(
        wxT("Total: %d despesas | Ativas: %d (%s) | Inativas: %d (%s) | Total Geral: %s"),
        (int)m_fixedExpenses.size(),
        countActive, MoneyUtils::FormatMoney(totalActive),
        countInactive, MoneyUtils::FormatMoney(totalInactive),
        MoneyUtils::FormatMoney(totalActive + totalInactive)
    );
    
    m_summaryText->SetLabel(summary);
}

void FixedExpensesPanel::OnAddFixedExpense(wxCommandEvent& event)
{
    FixedExpenseDialog dialog(this);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        FixedExpense expense = dialog.GetFixedExpense();
        FixedExpenseDAO dao;
        
        if (dao.Insert(expense))
        {
            LoadFixedExpenses();
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar despesa fixa!\n\nVerifique se já não existe uma despesa\ncom o mesmo dia e descrição."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void FixedExpensesPanel::OnEditFixedExpense(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);
    
    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa fixa para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }
    
    FixedExpense& oldExpense = m_fixedExpenses[selectedIndex];
    FixedExpenseDialog dialog(this, oldExpense);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        FixedExpense newExpense = dialog.GetFixedExpense();
        FixedExpenseDAO dao;
        
        if (dao.Update(oldExpense, newExpense))
        {
            LoadFixedExpenses();
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar despesa fixa!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void FixedExpensesPanel::OnDeleteFixedExpense(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);
    
    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa fixa para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }
    
    FixedExpense& expense = m_fixedExpenses[selectedIndex];
    
    wxString message = wxString::Format(
        wxT("Confirma a exclusão da despesa fixa:\n\n%s\nDia: %02d\nValor: %s"),
        expense.description,
        expense.dueDay,
        MoneyUtils::FormatMoney(expense.amount)
    );
    
    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        FixedExpenseDAO dao;
        
        if (dao.Delete(expense.dueDay, expense.description))
        {
            LoadFixedExpenses();
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir despesa fixa!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void FixedExpensesPanel::OnToggleActive(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);
    
    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma despesa fixa para ativar/desativar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }
    
    FixedExpense& expense = m_fixedExpenses[selectedIndex];
    FixedExpenseDAO dao;
    
    bool newStatus = !expense.active;
    
    if (dao.SetActive(expense.dueDay, expense.description, newStatus))
    {
        wxString message = wxString::Format(
            wxT("Despesa fixa '%s' foi %s com sucesso!"),
            expense.description,
            newStatus ? wxT("ativada") : wxT("desativada")
        );
        
        wxMessageBox(message, wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        LoadFixedExpenses();
    }
    else
    {
        wxMessageBox(wxT("Erro ao alterar status da despesa fixa!"),
                    wxT("Erro"), wxOK | wxICON_ERROR, this);
    }
}

void FixedExpensesPanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditFixedExpense(evt);
}