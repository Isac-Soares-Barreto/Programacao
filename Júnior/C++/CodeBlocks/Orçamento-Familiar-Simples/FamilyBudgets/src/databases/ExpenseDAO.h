#ifndef EXPENSEDAO_H
#define EXPENSEDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/Expense.h"

// Classe para acesso a dados de despesas
class ExpenseDAO
{
public:
    ExpenseDAO();
    ~ExpenseDAO();

    // Operações CRUD
    bool Insert(const Expense& expense);
    bool Update(const Expense& oldExpense, const Expense& newExpense);
    bool Delete(const wxDateTime& dueDate, const wxString& description);

    // Consultas
    std::vector<Expense> GetAll();
    std::vector<Expense> GetByMonth(int month, int year);
    std::vector<Expense> GetByDateRange(const wxDateTime& startDate, const wxDateTime& endDate);
    std::unique_ptr<Expense> GetByKey(const wxDateTime& dueDate, const wxString& description);

    // Importar despesas fixas
    bool ImportFixedExpenses(int month, int year);

    // Método para verificar despesas existentes em um mês/ano
    std::vector<wxString> GetExistingDescriptionsForMonth(int month, int year);

    // Importar despesas parceladas
    bool ImportInstallmentExpenses(int month, int year);

private:
    // Converter linha do banco para objeto Expense
    Expense RowToExpense(const wxArrayString& row);
};

#endif // EXPENSEDAO_H
