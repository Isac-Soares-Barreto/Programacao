#include "Application.h"
#include "../panels/MainFrame.h"
#include "../databases/Database.h"
#include "../utils/PathUtils.h"
#include <wx/msgdlg.h>
#include <wx/sysopt.h>
#include <wx/image.h>
#include <wx/file.h>
#include <wx/log.h>

Application::Application()
    : m_locale(nullptr)
{
}

Application::~Application()
{
    if (m_locale)
    {
        delete m_locale;
    }
}

bool Application::OnInit()
{
    // Configurar para não mostrar o aviso de manifest
    wxSystemOptions::SetOption(wxT("msw.no-manifest-check"), 1);

    // IMPORTANTE: Inicializar todos os handlers de imagem
    wxInitAllImageHandlers();

    // Configurar locale para português brasileiro
    SetupLocale();

    // Configurar recursos (copiar se necessário)
    SetupResources();

    // Inicializar banco de dados
    if (!InitializeDatabase())
    {
        wxMessageBox(wxT("Erro ao inicializar banco de dados!\n\n")
                     wxT("Verifique se o arquivo schema.sql está presente em:\n")
                     wxT("resources/database/schema.sql"),
                     wxT("Erro Fatal"), wxOK | wxICON_ERROR);
        return false;
    }

    // Criar e mostrar janela principal
    MainFrame* frame = new MainFrame();
    frame->Show(true);

    return true;
}

int Application::OnExit()
{
    // Fechar conexão com banco de dados
    Database::GetInstance().Close();

    return 0;
}

void Application::SetupLocale()
{
    // Configurar locale para português brasileiro
    m_locale = new wxLocale(wxLANGUAGE_PORTUGUESE_BRAZILIAN);

    // Se falhar, tentar português genérico
    if (!m_locale->IsOk())
    {
        delete m_locale;
        m_locale = new wxLocale(wxLANGUAGE_PORTUGUESE);
    }

    // Configurar encoding UTF-8 para suportar acentos
    #ifdef __WXMSW__
        // No Windows, configurar code page para UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
}

bool Application::InitializeDatabase()
{
    // Obter instância do banco de dados
    Database& db = Database::GetInstance();

    // Tentar abrir conexão
    if (!db.Open())
    {
        wxMessageBox(wxT("Erro ao abrir banco de dados:\n") + db.GetLastError(),
                     wxT("Erro"), wxOK | wxICON_ERROR);
        return false;
    }

    // Executar script de criação das tabelas
    wxString schemaPath = PathUtils::GetSchemaPath();

    wxLogDebug(wxT("Procurando schema em: %s"), schemaPath);

    if (!wxFileExists(schemaPath))
    {
        // Tentar caminho alternativo
        schemaPath = PathUtils::GetProjectDir() + wxT("resources") +
                    wxFileName::GetPathSeparator() + wxT("database") +
                    wxFileName::GetPathSeparator() + wxT("schema.sql");

        wxLogDebug(wxT("Tentando caminho alternativo: %s"), schemaPath);

        if (!wxFileExists(schemaPath))
        {
            wxMessageBox(wxT("Arquivo schema.sql não encontrado!\n\n")
                         wxT("Verifique se o arquivo existe em:\n")
                         wxT("resources/database/schema.sql"),
                         wxT("Erro"), wxOK | wxICON_ERROR);
            return false;
        }
    }

    // Ler arquivo schema
    wxFile file(schemaPath, wxFile::read);
    if (!file.IsOpened())
    {
        wxMessageBox(wxT("Não foi possível abrir o arquivo schema.sql!"),
                     wxT("Erro"), wxOK | wxICON_ERROR);
        return false;
    }

    wxString schema;
    if (!file.ReadAll(&schema))
    {
        wxMessageBox(wxT("Erro ao ler arquivo schema.sql!"),
                     wxT("Erro"), wxOK | wxICON_ERROR);
        file.Close();
        return false;
    }
    file.Close();

    // Executar script SQL
    if (!db.ExecuteScript(schema))
    {
        wxMessageBox(wxT("Erro ao criar estrutura do banco de dados!\n\n")
                     wxT("Detalhes: ") + db.GetLastError(),
                     wxT("Erro"), wxOK | wxICON_ERROR);
        return false;
    }

    wxLogDebug(wxT("Banco de dados inicializado com sucesso!"));

    return true;
}

void Application::SetupResources()
{
    // Em modo de desenvolvimento, copiar recursos se necessário
    #ifdef __WXDEBUG__
        PathUtils::CopyResourcesIfNeeded();
    #endif
}
