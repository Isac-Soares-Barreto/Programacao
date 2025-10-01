#include "CardsPanel.h"
#include "../utils/ButtonUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/WxUtils.h"
#include "../databases/CardDAO.h"
#include "../dialogs/CardDialog.h"
#include <wx/listctrl.h>
#include <wx/statline.h>

// IDs dos controles
enum
{
    ID_ListView = wxID_HIGHEST + 300,
    ID_AddButton,
    ID_EditButton,
    ID_DeleteButton
};

// Método auxiliar para obter o índice do item selecionado
static long GetSelectedItemIndex(wxListCtrl* listCtrl)
{
    return listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

CardsPanel::CardsPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateInterface();
    LoadCards();
}

CardsPanel::~CardsPanel()
{
}

void CardsPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Gerenciamento de Cartões"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);

    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    // Descrição
    wxStaticText* description = new wxStaticText(this, wxID_ANY,
        wxT("Cadastre seus cartões de crédito com as datas de fechamento e vencimento.\n")
        wxT("Estas informações serão usadas no cálculo automático de vencimento das parcelas."));
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

    // Lista de cartões
    m_listView = new wxListCtrl(this, ID_ListView, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    // Adicionar colunas
    m_listView->AppendColumn(wxT("Nome do Cartão"), wxLIST_FORMAT_LEFT, 180);
    m_listView->AppendColumn(wxT("Dia Fechamento"), wxLIST_FORMAT_CENTER, 120);
    m_listView->AppendColumn(wxT("Dia Vencimento"), wxLIST_FORMAT_CENTER, 120);
    m_listView->AppendColumn(wxT("Limite"), wxLIST_FORMAT_RIGHT, 120);
    m_listView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 300);

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
    Bind(wxEVT_BUTTON, &CardsPanel::OnAddCard, this, ID_AddButton);
    Bind(wxEVT_BUTTON, &CardsPanel::OnEditCard, this, ID_EditButton);
    Bind(wxEVT_BUTTON, &CardsPanel::OnDeleteCard, this, ID_DeleteButton);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &CardsPanel::OnItemActivated, this, ID_ListView);
}

void CardsPanel::LoadCards()
{
    m_listView->DeleteAllItems();
    m_cards.clear();

    CardDAO dao;
    m_cards = dao.GetAll();

    // Preencher lista
    double totalLimit = 0.0;
    int cardsWithLimit = 0;

    for (size_t i = 0; i < m_cards.size(); i++)
    {
        const Card& card = m_cards[i];

        long itemIndex = m_listView->InsertItem(i, card.name);
        m_listView->SetItem(itemIndex, 1, wxString::Format(wxT("Dia %02d"), card.closingDay));
        m_listView->SetItem(itemIndex, 2, wxString::Format(wxT("Dia %02d"), card.dueDay));

        if (card.HasLimit())
        {
            m_listView->SetItem(itemIndex, 3, MoneyUtils::FormatMoney(card.cardLimit));
            totalLimit += card.cardLimit;
            cardsWithLimit++;
        }
        else
        {
            m_listView->SetItem(itemIndex, 3, wxT("-"));
        }

        m_listView->SetItem(itemIndex, 4, card.description);

        // Colorir linhas alternadas para melhor visualização
        if (i % 2 == 1)
        {
            m_listView->SetItemBackgroundColour(itemIndex, wxColour(245, 245, 250));
        }
    }

    // Ajustar largura das colunas
    if (m_cards.size() > 0)
    {
        WxUtils::AutoSizeColumns(m_listView);
    }
    else
    {
        // Larguras padrão quando não há dados
        m_listView->SetColumnWidth(0, 180); // Nome do Cartão
        m_listView->SetColumnWidth(1, 120); // Dia Fechamento
        m_listView->SetColumnWidth(2, 120); // Dia Vencimento
        m_listView->SetColumnWidth(3, 120); // Limite
        m_listView->SetColumnWidth(4, 300); // Descrição
    }

    // Atualizar resumo
    wxString summary;

    if (m_cards.empty())
    {
        summary = wxT("Nenhum cartão cadastrado");
    }
    else
    {
        summary = wxString::Format(
            wxT("Total: %d cartão(ões) cadastrado(s)"),
            (int)m_cards.size()
        );

        if (cardsWithLimit > 0)
        {
            summary += wxString::Format(
                wxT(" | %d com limite definido | Limite total: %s"),
                cardsWithLimit,
                MoneyUtils::FormatMoney(totalLimit)
            );
        }
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

void CardsPanel::OnAddCard(wxCommandEvent& event)
{
    CardDialog dialog(this);

    if (dialog.ShowModal() == wxID_OK)
    {
        Card card = dialog.GetCard();
        CardDAO dao;

        if (dao.Insert(card))
        {
            LoadCards();
            wxMessageBox(
                wxString::Format(
                    wxT("Cartão '%s' cadastrado com sucesso!\n\n")
                    wxT("Agora você pode usar este cartão nas despesas e despesas parceladas."),
                    card.name
                ),
                wxT("Sucesso"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar cartão!\n\nVerifique se já não existe um cartão com este nome."),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void CardsPanel::OnEditCard(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione um cartão para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Card& oldCard = m_cards[selectedIndex];
    CardDialog dialog(this, oldCard);

    if (dialog.ShowModal() == wxID_OK)
    {
        Card newCard = dialog.GetCard();
        CardDAO dao;

        if (dao.Update(oldCard.name, newCard))
        {
            LoadCards();
            wxMessageBox(
                wxString::Format(wxT("Cartão atualizado com sucesso!")),
                wxT("Sucesso"),
                wxOK | wxICON_INFORMATION,
                this
            );
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar cartão!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void CardsPanel::OnDeleteCard(wxCommandEvent& event)
{
    long selectedIndex = GetSelectedItemIndex(m_listView);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione um cartão para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    Card& card = m_cards[selectedIndex];

    wxString message = wxString::Format(
        wxT("Confirma a exclusão do cartão:\n\n")
        wxT("Nome: %s\n")
        wxT("Fechamento: Dia %02d\n")
        wxT("Vencimento: Dia %02d"),
        card.name,
        card.closingDay,
        card.dueDay
    );

    if (card.HasLimit())
    {
        message += wxString::Format(wxT("\nLimite: %s"), MoneyUtils::FormatMoney(card.cardLimit));
    }

    message += wxT("\n\n⚠ ATENÇÃO: Esta ação não pode ser desfeita!");
    message += wxT("\n\nDespesas e despesas parceladas vinculadas a este cartão\n");
    message += wxT("poderão ficar inconsistentes.");

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        CardDAO dao;

        if (dao.Delete(card.name))
        {
            LoadCards();
            wxMessageBox(wxT("Cartão excluído com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir cartão!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void CardsPanel::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditButton);
    OnEditCard(evt);
}
