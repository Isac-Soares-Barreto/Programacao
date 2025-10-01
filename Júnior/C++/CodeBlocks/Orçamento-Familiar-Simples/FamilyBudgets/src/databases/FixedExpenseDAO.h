#ifndef FIXEDEXPENSEDAO_H
#define FIXEDEXPENSEDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/FixedExpense.h"

// Classe para acesso a dados de despesas fixas
class FixedExpenseDAO
{
public:
    FixedExpenseDAO();
    ~FixedExpenseDAO();
    
    // Operações CRUD
    bool Insert(const FixedExpense& expense);
    bool Update(const FixedExpense& oldExpense, const FixedExpense& newExpense);
    bool Delete(int dueDay, const wxString& description);
    
    // Consultas
    std::vector<FixedExpense> GetAll();
    std::vector<FixedExpense> GetActive();
    std::unique_ptr<FixedExpense> GetByKey(int dueDay, const wxString& description);
    
    // Ativar/Desativar despesa fixa
    bool SetActive(int dueDay, const wxString& description, bool active);
    
private:
    // Converter linha do banco para objeto FixedExpense
    FixedExpense RowToFixedExpense(const wxArrayString& row);
};

#endif // FIXEDEXPENSEDAO_H