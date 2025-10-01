#include "AboutDialog.h"
#include "../utils/PathUtils.h"
#include <wx/statline.h>
#include <wx/hyperlink.h>
#include <wx/wrapsizer.h>

AboutDialog::AboutDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Sobre o Kaiser Family Budgets"),
               wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
    CreateInterface();
    SetInitialSize(wxSize(450, 420));
    CentreOnParent();
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::CreateInterface()
{
    // Painel principal para cor de fundo
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY);
    mainPanel->SetBackgroundColour(wxColour(255, 255, 255)); // Fundo branco

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título principal
    wxStaticText* title = new wxStaticText(mainPanel, wxID_ANY, wxT("Orçamento Familiar"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(18);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    title->SetForegroundColour(wxColour(40, 40, 40));
    mainSizer->Add(title, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 20);

    // Subtítulo
    wxStaticText* subtitle = new wxStaticText(mainPanel, wxID_ANY, wxT("Kaiser Family Budgets"));
    wxFont subtitleFont = subtitle->GetFont();
    subtitleFont.SetPointSize(14);
    subtitleFont.SetFamily(wxFONTFAMILY_SWISS);
    subtitle->SetFont(subtitleFont);
    subtitle->SetForegroundColour(wxColour(80, 80, 80));
    mainSizer->Add(subtitle, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 5);

    // Ícone da aplicação
    wxString iconPath = PathUtils::GetIconsDir() + wxT("app_icon.ico");
    if (wxFileExists(iconPath))
    {
        wxIcon icon;
        icon.LoadFile(iconPath, wxBITMAP_TYPE_ICO);
        if (icon.IsOk())
        {
            // O ícone precisa ser convertido para um bitmap para ser exibido
            wxBitmap bmp;
            bmp.CopyFromIcon(icon);
            // Redimensionar se necessário (ex: para 64x64)
            if (bmp.IsOk() && (bmp.GetWidth() > 64 || bmp.GetHeight() > 64))
            {
                wxImage img = bmp.ConvertToImage();
                img.Rescale(64, 64, wxIMAGE_QUALITY_HIGH);
                bmp = wxBitmap(img);
            }
            wxStaticBitmap* iconBitmap = new wxStaticBitmap(mainPanel, wxID_ANY, bmp);
            mainSizer->Add(iconBitmap, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 15);
        }
    }

    // Linha separadora
    mainSizer->Add(new wxStaticLine(mainPanel), 0, wxEXPAND | wxLEFT | wxRIGHT, 20);

    // Informações de versão e autor
    wxStaticText* versionInfo = new wxStaticText(mainPanel, wxID_ANY,
        wxT("Versão 1.0 - Desenvolvido por Isac Soares Barreto"));
    mainSizer->Add(versionInfo, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 15);

    // Descrição do programa com quebra de linha
    wxStaticText* description = new wxStaticText(mainPanel, wxID_ANY,
        wxT("Programa de Orçamento Familiar Simples para controle de gastos!"));
    wxWrapSizer* descriptionSizer = new wxWrapSizer(wxHORIZONTAL);
    descriptionSizer->Add(description, 1, wxALIGN_CENTER);
    mainSizer->Add(descriptionSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxTOP, 10);

    // Link do LinkedIn
    wxBoxSizer* linkSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* linkedinLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("LinkedIn: "));
    wxHyperlinkCtrl* linkedinLink = new wxHyperlinkCtrl(mainPanel, wxID_ANY,
        wxT("linkedin.com/in/isac-engenheiro-mecatronico/"),
        wxT("https://linkedin.com/in/isac-engenheiro-mecatronico/"));

    wxFont linkFont = linkedinLink->GetFont();
    linkFont.SetPointSize(9);
    linkedinLink->SetFont(linkFont);
    linkedinLabel->SetFont(linkFont);

    linkSizer->Add(linkedinLabel, 0, wxALIGN_CENTER_VERTICAL);
    linkSizer->Add(linkedinLink, 0, wxALIGN_CENTER_VERTICAL);
    mainSizer->Add(linkSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 15);

    // Espaçador para empurrar o copyright para baixo
    mainSizer->AddStretchSpacer();

    // Copyright
    wxStaticText* copyright = new wxStaticText(mainPanel, wxID_ANY,
        wxT("© 2025 Isac Soares Barreto - Todos os direitos reservados"));
    wxFont copyrightFont = copyright->GetFont();
    copyrightFont.SetPointSize(8);
    copyright->SetFont(copyrightFont);
    copyright->SetForegroundColour(wxColour(150, 150, 150));
    mainSizer->Add(copyright, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);

    // Botão OK
    wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();
    wxButton* okButton = new wxButton(mainPanel, wxID_OK, wxT("OK"));
    buttonSizer->AddButton(okButton);
    buttonSizer->Realize();
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

    mainPanel->SetSizer(mainSizer);

    // Sizer principal do diálogo para conter o painel
    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);
    dialogSizer->Add(mainPanel, 1, wxEXPAND);
    SetSizer(dialogSizer);

    okButton->SetDefault();
}

void AboutDialog::OnLinkedInClick(wxHyperlinkEvent& event)
{
    // A ação padrão do wxHyperlinkCtrl já é abrir o link no navegador.
    // Este método é útil se você quisesse adicionar um log ou outra ação.
    event.Skip(); // Permite que o evento continue e o link seja aberto.
}
