#ifndef INSTALLMENTEXPENSEDAO_H
#define INSTALLMENTEXPENSEDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/InstallmentExpense.h"

// Classe para acesso a dados de despesas parceladas
class InstallmentExpenseDAO
{
public:
    InstallmentExpenseDAO();
    ~InstallmentExpenseDAO();

    // Operações CRUD
    bool Insert(const InstallmentExpense& expense);
    bool Update(const InstallmentExpense& oldExpense, const InstallmentExpense& newExpense);
    bool Delete(const wxDateTime& purchaseDate, const wxString& description, const wxString& cardName);

    // Consultas
    std::vector<InstallmentExpense> GetAll();
    std::vector<InstallmentExpense> GetActive();
    std::unique_ptr<InstallmentExpense> GetByKey(const wxDateTime& purchaseDate, const wxString& description, const wxString& cardName);

    // Ativar/Desativar despesa parcelada
    bool SetActive(const wxDateTime& purchaseDate, const wxString& description, const wxString& cardName, bool active);

    // Obter informações do cartão
    bool GetCardInfo(const wxString& cardName, int& closingDay, int& dueDay);

private:
    // Converter linha do banco para objeto InstallmentExpense
    InstallmentExpense RowToInstallmentExpense(const wxArrayString& row);
};

#endif // INSTALLMENTEXPENSEDAO_H
