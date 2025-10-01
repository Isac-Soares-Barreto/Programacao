#ifndef DATABASE_H
#define DATABASE_H

#include <wx/wx.h>
#include <sqlite3.h>
#include <memory>
#include <vector>

// Classe para resultado de consultas
class QueryResult
{
public:
    std::vector<wxArrayString> rows;
    wxArrayString columns;
    
    bool IsEmpty() const { return rows.empty(); }
    size_t GetRowCount() const { return rows.size(); }
    size_t GetColumnCount() const { return columns.size(); }
};

// Classe singleton para gerenciar conexão com banco de dados
class Database
{
public:
    // Obter instância única
    static Database& GetInstance();
    
    // Abrir conexão
    bool Open();
    
    // Fechar conexão
    void Close();
    
    // Executar query sem retorno (INSERT, UPDATE, DELETE)
    bool Execute(const wxString& sql);
    
    // Executar query com retorno (SELECT)
    std::unique_ptr<QueryResult> Query(const wxString& sql);
    
    // Executar script SQL (múltiplas queries)
    bool ExecuteScript(const wxString& script);
    
    // Verificar se está conectado
    bool IsOpen() const { return m_db != nullptr; }
    
    // Obter último erro
    wxString GetLastError() const { return m_lastError; }
    
    // Iniciar transação
    bool BeginTransaction();
    
    // Confirmar transação
    bool CommitTransaction();
    
    // Reverter transação
    bool RollbackTransaction();
    
private:
    Database();
    ~Database();
    
    // Deletar construtores de cópia
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    sqlite3* m_db;
    wxString m_lastError;
    
    // Callback para queries SELECT
    static int QueryCallback(void* data, int argc, char** argv, char** columns);
};

#endif // DATABASE_H