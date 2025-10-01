#include "PaymentMethodsPanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/PaymentMethodDAO.h"
#include "../dialogs/PaymentMethodDialog.h"
#include <wx/listctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 400,
    ID_AddButton,
    ID_EditButton,
    ID_DeleteButton
};

// Método auxiliar para obter o índice do item selecionado
static long GetSelectedItemIndex(wxListCtrl* listCtrl)
{
    return listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

PaymentMethodsPanel::PaymentMethodsPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateInterface();
    LoadPaymentMethods();
}

PaymentMethodsPanel::~PaymentMethodsPanel()
{
}

void PaymentMethodsPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Formas de Pagamento"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);

    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    // Descrição
    wxStaticText* description = new wxStaticText(this, wxID_ANY,
        wxT("Configure as formas de pagamento disponíveis para uso nas despesas.\n")
        wxT("O sistema já inclui formas padrão (Dinheiro, PIX, Boleto, Cartão, Transferência) que podem ser editadas ou complementadas."));
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

    toolbarSizer->Add(btnAdd, 0, wxALL, 5);
    toolbarSizer->Add(btnEdit, 0, wxALL, 5);
    toolbarSizer->Add(btnDelete, 0, wxALL, 5);
    toolbarSizer->AddStretchSpacer();

    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Lista de formas de pagamento
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    // Adicionar colunas
    m_listView->AppendColumn(wxT("Forma de Pagamento"), wxLIST_FORMAT_LEFT, 200);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 450);
    m_listView->AppendColumn(wxT("Tipo"), wxLIST_FORMAT_CENTER, 100);

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
    Bind(wxEVT_BUTTON, &PaymentMethodsPanel::OnAddPaymentMethod, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &PaymentMethodsPanel::OnEditPaymentMethod, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &PaymentMethodsPanel::OnDeletePaymentMethod, this, ID_DeleteButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &PaymentMethodsPanel::OnItemActivated, this, ID_ListView);
}

void PaymentMethodsPanel::LoadPaymentMethods()
{
    m_listView->DeleteAllItems();
    m_methods.clear();

    PaymentMethodDAO dao;
    m_methods = dao.GetAll();

    // Preencher lista
    int defaultCount = 0;
    int customCount = 0;

    for (size_t i = 0; i < m_methods.size(); i++)
    {
        const PaymentMethod& method = m_methods[i];

        long itemIndex = m_listView->InsertItem(i, method.name);
        m_listView->SetItem(itemIndex, 1, method.description);

        wxString type;
        wxColour bgColor = *wxWHITE;

        if (method.IsDefault())
        {
            type = wxT("Padrão");
            bgColor = wxColour(230, 240, 255); // Azul claro
            defaultCount++;
        }
        else
        {
            type = wxT("Customizada");
            bgColor = wxColour(245, 255, 230); // Verde claro
            customCount++;
        }

        m_listView->SetItem(itemIndex, 2, type);
        m_listView->SetItemBackgroundColour(itemIndex, bgColor);
    }

    // Ajustar largura das colunas
    if (m_methods.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 200); // Forma de Pagamento
        m_listView->SetColumnWidth(1, 450); // Descrição
        m_listView->SetColumnWidth(2, 100); // Tipo
    }

    // Atualizar resumo
    wxString summary;

    if (m_methods.empty())
    {
        summary = wxT("Nenhuma forma de pagamento cadastrada");
    }
    else
    {
        summary = wxString::Format(
            wxT("Total: %d forma(s) de pagamento | Padrão: %d | Customizadas: %d"),
            (int)m_methods.size(),
            defaultCount,
            customCount
        );
    }

    // Atualizar com ajuste de layout
    m_summaryText->SetLabel(summary);
    m_summaryText->InvalidateBestSize();
    m_summaryText->GetParent()->Layout();

    wxSize bestSize = m_summaryText->GetBestSize();
    m_summaryText->SetMinSize(bestSize);

    this->Layout();
    this->Refresh();
}

void PaymentMethodsPanel::OnAddPaymentMethod(wxCommandEvent& event)
{
    PaymentMethodDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        PaymentMethod method = dialog.GetPaymentMethod();
        PaymentMethodDAO dao;

        if (dao.Insert(method))
        {
            LoadPaymentMethods();
            wxMessageBox(
                wxString::Format(
                    wxT("Forma de pagamento '%s' cadastrada com sucesso!\n\n")
                    wxT("Agora você pode usar esta forma de pagamento nas despesas."),
                    method.name
                ),
                wxT("Sucesso"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar forma de pagamento!\n\nVerifique se já não existe uma forma com este nome."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void PaymentMethodsPanel::OnEditPaymentMethod(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma forma de pagamento para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    PaymentMethod& oldMethod = m_methods[selectedIndex];
    PaymentMethodDialog dialog(this, oldMethod);

    if (dialog.ShowModal() == wxID_OK)
    {
        PaymentMethod newMethod = dialog.GetPaymentMethod();
        PaymentMethodDAO dao;

        if (dao.Update(oldMethod.name, newMethod))
        {
            LoadPaymentMethods();
            wxMessageBox(
                wxT("Forma de pagamento atualizada com sucesso!"),
                wxT("Sucesso"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar forma de pagamento!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void PaymentMethodsPanel::OnDeletePaymentMethod(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma forma de pagamento para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    PaymentMethod& method = m_methods[selectedIndex];

    // Verificar se é forma padrão
    if (method.IsDefault())
    {
        wxMessageBox(
            wxT("Não é possível excluir formas de pagamento padrão do sistema!\n\n")
            wxT("As formas padrão são essenciais para o funcionamento do programa.\n")
            wxT("Você pode apenas editar a descrição delas."),
            wxT("Aviso"),
            wxOK | wxICON_WARNING,
            this
        );
        return;
    }

    wxString message = wxString::Format(
        wxT("Confirma a exclusão da forma de pagamento:\n\n")
        wxT("Nome: %s\n")
        wxT("Descrição: %s\n\n")
        wxT("⚠ ATENÇÃO: Esta ação não pode ser desfeita!\n\n")
        wxT("Despesas que usam esta forma de pagamento poderão\n")
        wxT("ficar inconsistentes."),
        method.name,
        method.description
    );

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        PaymentMethodDAO dao;

        if (dao.Delete(method.name))
        {
            LoadPaymentMethods();
            wxMessageBox(wxT("Forma de pagamento excluída com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir forma de pagamento!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void PaymentMethodsPanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditPaymentMethod(evt);
}
