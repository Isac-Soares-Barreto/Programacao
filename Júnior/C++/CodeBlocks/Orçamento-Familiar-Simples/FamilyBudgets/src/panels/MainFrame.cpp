#include "MainFrame.h"
#include "DashboardPanel.h"
#include "ExpensesPanel.h"
#include "FixedExpensesPanel.h"
#include "InstallmentExpensesPanel.h"
#include "IncomePanel.h"
#include "InvestmentsPanel.h"
#include "CardsPanel.h"
#include "PaymentMethodsPanel.h"
#include "../dialogs/AboutDialog.h"
#include "../dialogs/SettingsDialog.h"
#include "../utils/PathUtils.h"
#include <wx/artprov.h>

// Tabela de eventos
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(ID_Settings, MainFrame::OnSettings)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, MainFrame::OnPageChanged)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, wxT("Orçamento Familiar - Kaiser Family Budgets"),
              wxDefaultPosition, wxSize(1200, 700))
{
    // Definir tamanho mínimo
    SetMinSize(wxSize(1200, 700));

    // Maximizar a janela
    Maximize(true);

    // Centralizar janela
    Centre();

    // Carregar ícone da janela
    LoadWindowIcon();

    // Criar componentes da interface
    CreateMenuBar();
    CreateAppStatusBar();
    CreateNotebook();
}

MainFrame::~MainFrame()
{
}

void MainFrame::CreateMenuBar()
{
    // Criar barra de menus
    wxMenuBar* menuBar = new wxMenuBar();

    // Menu Arquivo
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_EXIT, wxT("&Sair\tAlt+F4"), wxT("Sair do programa"));
    menuBar->Append(fileMenu, wxT("&Arquivo"));

    // Menu Ferramentas
    wxMenu* toolsMenu = new wxMenu();
    toolsMenu->Append(ID_Settings, wxT("&Configurações\tCtrl+,"), wxT("Abrir configurações"));
    menuBar->Append(toolsMenu, wxT("&Ferramentas"));

    // Menu Ajuda
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT, wxT("&Sobre\tF1"), wxT("Sobre o programa"));
    menuBar->Append(helpMenu, wxT("A&juda"));

    SetMenuBar(menuBar);
}

void MainFrame::CreateAppStatusBar()
{
    // Criar barra de status com 2 campos
    wxStatusBar* statusBar = CreateStatusBar(2);

    // Definir larguras dos campos
    int widths[] = {-1, 200};
    statusBar->SetStatusWidths(2, widths);

    // Texto inicial
    SetStatusText(wxT("Bem-vindo ao Kaiser Family Budgets"), 0);
    SetStatusText(wxT("Versão 1.0"), 1);
}

void MainFrame::CreateNotebook()
{
    // Criar painel principal
    wxPanel* mainPanel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Criar notebook com abas
    m_notebook = new wxNotebook(mainPanel, wxID_ANY, wxDefaultPosition,
                                wxDefaultSize, wxNB_TOP);

    // Configurar lista de imagens para os ícones das abas
    wxImageList* imageList = new wxImageList(24, 24);

    // Carregar ícones
    wxString iconsPath = PathUtils::GetIconsDir();

    // Lista de ícones
    wxArrayString iconFiles;
    iconFiles.Add(wxT("dashboard.png"));
    iconFiles.Add(wxT("expenses.png"));
    iconFiles.Add(wxT("fixed_expenses.png"));
    iconFiles.Add(wxT("installments.png"));
    iconFiles.Add(wxT("income.png"));
    iconFiles.Add(wxT("investments.png"));
    iconFiles.Add(wxT("cards.png"));
    iconFiles.Add(wxT("payment_methods.png"));

    for (size_t i = 0; i < iconFiles.GetCount(); i++)
    {
        wxString fullPath = iconsPath + iconFiles[i];
        wxBitmap bitmap(24, 24); // Bitmap padrão

        if (wxFileExists(fullPath))
        {
            wxImage image(fullPath, wxBITMAP_TYPE_PNG);
            if (image.IsOk())
            {
                // Redimensionar se necessário
                if (image.GetWidth() != 24 || image.GetHeight() != 24)
                {
                    image.Rescale(24, 24, wxIMAGE_QUALITY_HIGH);
                }
                bitmap = wxBitmap(image);
            }
        }

        imageList->Add(bitmap);
    }

    m_notebook->SetImageList(imageList);

    // Criar painéis
    m_dashboardPanel = new DashboardPanel(m_notebook);
    m_expensesPanel = new ExpensesPanel(m_notebook);
    m_fixedExpensesPanel = new FixedExpensesPanel(m_notebook);
    m_installmentExpensesPanel = new InstallmentExpensesPanel(m_notebook);
    m_incomePanel = new IncomePanel(m_notebook);
    m_investmentsPanel = new InvestmentsPanel(m_notebook);
    m_cardsPanel = new CardsPanel(m_notebook);
    m_paymentMethodsPanel = new PaymentMethodsPanel(m_notebook);

    // Adicionar painéis ao notebook
    m_notebook->AddPage(m_dashboardPanel, wxT("Dashboard"), true, 0);
    m_notebook->AddPage(m_expensesPanel, wxT("Despesas"), false, 1);
    m_notebook->AddPage(m_fixedExpensesPanel, wxT("Despesas Fixas"), false, 2);
    m_notebook->AddPage(m_installmentExpensesPanel, wxT("Despesas Parceladas"), false, 3);
    m_notebook->AddPage(m_incomePanel, wxT("Receitas"), false, 4);
    m_notebook->AddPage(m_investmentsPanel, wxT("Investimentos"), false, 5);
    m_notebook->AddPage(m_cardsPanel, wxT("Cartões"), false, 6);
    m_notebook->AddPage(m_paymentMethodsPanel, wxT("Formas de Pagamento"), false, 7);

    // Adicionar notebook ao sizer
    mainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

    // Configurar sizer do painel principal
    mainPanel->SetSizer(mainSizer);
}

void MainFrame::LoadWindowIcon()
{
    // Carregar ícone da aplicação
    wxString iconPath = PathUtils::GetIconsDir() + wxT("app_icon.ico");
    if (wxFileExists(iconPath))
    {
        wxIcon icon(iconPath, wxBITMAP_TYPE_ICO);
        if (icon.IsOk())
        {
            SetIcon(icon);
        }
    }
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    // Fechar aplicação
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    // Mostrar diálogo sobre
    AboutDialog dialog(this);
    dialog.ShowModal();
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
    // Mostrar diálogo de configurações
    SettingsDialog dialog(this);
    dialog.ShowModal();
}

void MainFrame::OnPageChanged(wxBookCtrlEvent& event)
{
    // Atualizar barra de status com a aba selecionada
    int selection = event.GetSelection();
    if (selection != wxNOT_FOUND)
    {
        wxString pageName = m_notebook->GetPageText(selection);
        SetStatusText(wxT("Aba selecionada: ") + pageName, 0);
    }
}
