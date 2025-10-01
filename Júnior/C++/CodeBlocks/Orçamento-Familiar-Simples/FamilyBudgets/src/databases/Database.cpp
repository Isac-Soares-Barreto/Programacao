#include "Database.h"
#include "../utils/PathUtils.h"
#include <wx/tokenzr.h>
#include <wx/log.h>

Database::Database()
    : m_db(nullptr)
{
}

Database::~Database()
{
    Close();
}

Database& Database::GetInstance()
{
    static Database instance;
    return instance;
}

bool Database::Open()
{
    if (m_db)
    {
        return true; // Já está aberto
    }

    wxString dbPath = PathUtils::GetDatabasePath();

    // Criar diretório se não existir
    wxFileName fn(dbPath);
    wxString dir = fn.GetPath();
    if (!wxDirExists(dir))
    {
        if (!wxMkdir(dir, wxS_DIR_DEFAULT))
        {
            m_lastError = wxT("Não foi possível criar o diretório do banco de dados");
            return false;
        }
    }

    // Log do caminho do banco
    wxLogDebug(wxT("Abrindo banco de dados em: %s"), dbPath);

    // Abrir banco de dados
    int result = sqlite3_open(dbPath.utf8_str(), &m_db);

    if (result != SQLITE_OK)
    {
        m_lastError = wxString::FromUTF8(sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }

    // Habilitar chaves estrangeiras
    Execute(wxT("PRAGMA foreign_keys = ON"));

    // Configurar encoding UTF-8
    Execute(wxT("PRAGMA encoding = 'UTF-8'"));

    return true;
}

void Database::Close()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool Database::Execute(const wxString& sql)
{
    if (!m_db)
    {
        m_lastError = wxT("Banco de dados não está aberto");
        return false;
    }

    char* errorMsg = nullptr;
    int result = sqlite3_exec(m_db, sql.utf8_str(), nullptr, nullptr, &errorMsg);

    if (result != SQLITE_OK)
    {
        m_lastError = errorMsg ? wxString::FromUTF8(errorMsg) : wxT("Unknown error");
        if (errorMsg) sqlite3_free(errorMsg);

        // Log do erro apenas se não for um aviso esperado
        if (!m_lastError.Contains(wxT("already exists")))
        {
            wxLogError(wxT("Erro SQL: %s"), m_lastError);
            wxLogError(wxT("Query: %s"), sql);
        }

        return false;
    }

    return true;
}

std::unique_ptr<QueryResult> Database::Query(const wxString& sql)
{
    auto result = std::make_unique<QueryResult>();

    if (!m_db)
    {
        m_lastError = wxT("Banco de dados não está aberto");
        return result;
    }

    char* errorMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.utf8_str(), QueryCallback, result.get(), &errorMsg);

    if (rc != SQLITE_OK)
    {
        m_lastError = errorMsg ? wxString::FromUTF8(errorMsg) : wxT("Unknown error");
        if (errorMsg) sqlite3_free(errorMsg);

        wxLogError(wxT("Erro SQL: %s"), m_lastError);
        wxLogError(wxT("Query: %s"), sql);
    }

    return result;
}

bool Database::ExecuteScript(const wxString& script)
{
    // Executar o script completo de uma vez só ao invés de dividir
    char* errorMsg = nullptr;
    int result = sqlite3_exec(m_db, script.utf8_str(), nullptr, nullptr, &errorMsg);

    if (result != SQLITE_OK)
    {
        m_lastError = errorMsg ? wxString::FromUTF8(errorMsg) : wxT("Unknown error");
        if (errorMsg)
        {
            wxLogError(wxT("Erro ao executar script: %s"), m_lastError);
            sqlite3_free(errorMsg);
        }
        return false;
    }

    return true;
}

bool Database::BeginTransaction()
{
    return Execute(wxT("BEGIN TRANSACTION"));
}

bool Database::CommitTransaction()
{
    return Execute(wxT("COMMIT"));
}

bool Database::RollbackTransaction()
{
    return Execute(wxT("ROLLBACK"));
}

int Database::QueryCallback(void* data, int argc, char** argv, char** columns)
{
    QueryResult* result = static_cast<QueryResult*>(data);

    // Primeira linha: nomes das colunas
    if (result->columns.IsEmpty())
    {
        for (int i = 0; i < argc; i++)
        {
            result->columns.Add(wxString::FromUTF8(columns[i]));
        }
    }

    // Adicionar linha de dados
    wxArrayString row;
    for (int i = 0; i < argc; i++)
    {
        if (argv[i])
        {
            row.Add(wxString::FromUTF8(argv[i]));
        }
        else
        {
            row.Add(wxT(""));
        }
    }

    result->rows.push_back(row);

    return 0;
}
