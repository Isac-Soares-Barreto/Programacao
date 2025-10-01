#ifndef INCOMEDAO_H
#define INCOMEDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/Income.h"

// Classe para acesso a dados de receitas
class IncomeDAO
{
public:
    IncomeDAO();
    ~IncomeDAO();

    // Operações CRUD
    bool Insert(const Income& income);
    bool Update(const Income& oldIncome, const Income& newIncome);
    bool Delete(const wxDateTime& receiptDate, const wxString& description);

    // Consultas
    std::vector<Income> GetAll();
    std::vector<Income> GetByMonth(int month, int year);
    std::vector<Income> GetByDateRange(const wxDateTime& startDate, const wxDateTime& endDate);
    std::unique_ptr<Income> GetByKey(const wxDateTime& receiptDate, const wxString& description);

    // Estatísticas
    double GetTotalByMonth(int month, int year);
    double GetTotalByDateRange(const wxDateTime& startDate, const wxDateTime& endDate);

private:
    // Converter linha do banco para objeto Income
    Income RowToIncome(const wxArrayString& row);
};

#endif // INCOMEDAO_H
