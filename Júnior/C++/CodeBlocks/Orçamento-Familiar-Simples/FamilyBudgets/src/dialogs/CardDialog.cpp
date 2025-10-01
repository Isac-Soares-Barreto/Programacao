#include "CardDialog.h"
#include "../utils/DecimalTextCtrl.h"
#include "../utils/MoneyUtils.h"
#include "../databases/CardDAO.h"
#include <wx/statline.h>

CardDialog::CardDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Novo Cartão"),
               wxDefaultPosition, wxSize(500, 600))
    , m_isEdit(false)
{
    CreateInterface();
    Centre();
}

CardDialog::CardDialog(wxWindow* parent, const Card& card)
    : wxDialog(parent, wxID_ANY, wxT("Editar Cartão"),
               wxDefaultPosition, wxSize(500, 600))
    , m_card(card)
    , m_isEdit(true)
{
    CreateInterface();
    LoadData();
    Centre();
}

CardDialog::~CardDialog()
{
}

void CardDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Nome do Cartão
    wxStaticText* nameLabel = new wxStaticText(this, wxID_ANY, wxT("Nome do Cartão:"));
    m_nameText = new wxTextCtrl(this, wxID_ANY);
    m_nameText->SetMaxLength(100);
    if (m_isEdit)
    {
        // Em edição, desabilitar alteração do nome (é chave primária)
        m_nameText->Enable(false);
    }
    gridSizer->Add(nameLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_nameText, 1, wxEXPAND);

    // Dia de Fechamento
    wxStaticText* closingLabel = new wxStaticText(this, wxID_ANY, wxT("Dia de Fechamento:"));
    m_closingDaySpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxSP_ARROW_KEYS, 1, 31, 1);
    gridSizer->Add(closingLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_closingDaySpin, 1, wxEXPAND);

    // Dia de Vencimento
    wxStaticText* dueLabel = new wxStaticText(this, wxID_ANY, wxT("Dia de Vencimento:"));
    m_dueDaySpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxSP_ARROW_KEYS, 1, 31, 10);
    gridSizer->Add(dueLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_dueDaySpin, 1, wxEXPAND);

    // Limite do Cartão
    wxStaticText* limitLabel = new wxStaticText(this, wxID_ANY, wxT("Limite do Cartão (R$):"));
    m_limitText = new DecimalTextCtrl(this);
    gridSizer->Add(limitLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_limitText, 1, wxEXPAND);

    // Descrição/Observações
    wxStaticText* descLabel = new wxStaticText(this, wxID_ANY, wxT("Descrição/Observações:"));
    m_descriptionText = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxSize(-1, 80),
                                       wxTE_MULTILINE);
    m_descriptionText->SetMaxLength(500);
    gridSizer->Add(descLabel, 0, wxALIGN_TOP | wxRIGHT, 5);
    gridSizer->Add(m_descriptionText, 1, wxEXPAND);

    mainSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Informação do ciclo do cartão
    wxStaticBoxSizer* cycleBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Informações do Ciclo"));

    m_cycleInfoText = new wxStaticText(this, wxID_ANY,
        wxT("O ciclo do cartão é o período entre o fechamento e o vencimento.\n\n")
        wxT("• Compras feitas ANTES do dia de fechamento entram na próxima fatura\n")
        wxT("• Compras feitas APÓS o dia de fechamento entram na fatura seguinte\n\n")
        wxT("Exemplo: Fechamento dia 10, vencimento dia 15\n")
        wxT("→ Compra no dia 8: vence no dia 15 do mesmo mês\n")
        wxT("→ Compra no dia 12: vence no dia 15 do mês seguinte"));

    wxFont infoFont = m_cycleInfoText->GetFont();
    infoFont.SetPointSize(infoFont.GetPointSize() - 1);
    m_cycleInfoText->SetFont(infoFont);
    m_cycleInfoText->SetForegroundColour(wxColour(60, 60, 60));

    cycleBox->Add(m_cycleInfoText, 1, wxALL | wxEXPAND, 10);

    mainSizer->Add(cycleBox, 0, wxALL | wxEXPAND, 10);

    // Nota sobre limite
    wxStaticText* noteText = new wxStaticText(this, wxID_ANY,
        wxT("Nota: O limite do cartão é opcional e serve apenas para controle.\n")
        wxT("Deixe em branco ou zero se não quiser informar o limite."));
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
    Bind(wxEVT_BUTTON, &CardDialog::OnOK, this, wxID_OK);
    Bind(wxEVT_SPINCTRL, &CardDialog::OnClosingDayChanged, this, wxID_ANY);
    Bind(wxEVT_SPINCTRL, &CardDialog::OnDueDayChanged, this, wxID_ANY);

    okButton->SetDefault();
}

void CardDialog::LoadData()
{
    // Preencher campos com dados do cartão
    m_nameText->SetValue(m_card.name);
    m_closingDaySpin->SetValue(m_card.closingDay);
    m_dueDaySpin->SetValue(m_card.dueDay);

    if (m_card.cardLimit > 0)
    {
        m_limitText->SetDoubleValue(m_card.cardLimit);
    }

    m_descriptionText->SetValue(m_card.description);
}

void CardDialog::OnClosingDayChanged(wxSpinEvent& event)
{
    ValidateCardCycle();
}

void CardDialog::OnDueDayChanged(wxSpinEvent& event)
{
    ValidateCardCycle();
}

bool CardDialog::ValidateCardCycle()
{
    int closingDay = m_closingDaySpin->GetValue();
    int dueDay = m_dueDaySpin->GetValue();

    // Atualizar texto informativo baseado nos valores selecionados
    wxString cycleInfo = wxString::Format(
        wxT("Configuração atual:\n")
        wxT("• Fechamento: dia %d de cada mês\n")
        wxT("• Vencimento: dia %d de cada mês\n\n"),
        closingDay, dueDay
    );

    if (dueDay > closingDay)
    {
        cycleInfo += wxT("✓ Ciclo no mesmo mês (fechamento antes do vencimento)\n");
        cycleInfo += wxString::Format(
            wxT("→ Compras até dia %d: vence no dia %d do mesmo mês\n")
            wxT("→ Compras após dia %d: vence no dia %d do mês seguinte"),
            closingDay, dueDay, closingDay, dueDay
        );
    }
    else if (dueDay < closingDay)
    {
        cycleInfo += wxT("✓ Ciclo entre meses (vencimento no mês seguinte ao fechamento)\n");
        cycleInfo += wxString::Format(
            wxT("→ Compras até dia %d: vence no dia %d do mês seguinte\n")
            wxT("→ Compras após dia %d: vence no dia %d do segundo mês seguinte"),
            closingDay, dueDay, closingDay, dueDay
        );
    }
    else
    {
        cycleInfo += wxT("⚠ Atenção: Fechamento e vencimento no mesmo dia!\n");
        cycleInfo += wxT("→ Todas as compras vencerão no mesmo dia do mês seguinte");
    }

    m_cycleInfoText->SetLabel(cycleInfo);
    m_cycleInfoText->Wrap(450);

    Layout();

    return true;
}

void CardDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
    if (m_nameText->GetValue().Trim().IsEmpty())
    {
        wxMessageBox(wxT("Nome do cartão é obrigatório!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_nameText->SetFocus();
        return;
    }

    // Verificar se o nome já existe (apenas ao adicionar novo)
    if (!m_isEdit)
    {
        CardDAO dao;
        if (dao.Exists(m_nameText->GetValue().Trim()))
        {
            wxMessageBox(wxT("Já existe um cartão com este nome!\n\nEscolha outro nome."),
                        wxT("Aviso"), wxOK | wxICON_WARNING, this);
            m_nameText->SetFocus();
            return;
        }
    }

    // Validar dias
    int closingDay = m_closingDaySpin->GetValue();
    int dueDay = m_dueDaySpin->GetValue();

    if (closingDay < 1 || closingDay > 31)
    {
        wxMessageBox(wxT("Dia de fechamento deve estar entre 1 e 31!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_closingDaySpin->SetFocus();
        return;
    }

    if (dueDay < 1 || dueDay > 31)
    {
        wxMessageBox(wxT("Dia de vencimento deve estar entre 1 e 31!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_dueDaySpin->SetFocus();
        return;
    }

    event.Skip();
}

Card CardDialog::GetCard() const
{
    Card card;

    card.name = m_nameText->GetValue().Trim();
    card.closingDay = m_closingDaySpin->GetValue();
    card.dueDay = m_dueDaySpin->GetValue();
    card.cardLimit = m_limitText->GetDoubleValue();
    card.description = m_descriptionText->GetValue().Trim();

    return card;
}
