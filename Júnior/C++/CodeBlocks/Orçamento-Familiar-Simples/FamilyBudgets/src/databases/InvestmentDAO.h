#ifndef INVESTMENTDAO_H
#define INVESTMENTDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/Investment.h"

// Classe para acesso a dados de investimentos
class InvestmentDAO
{
public:
    InvestmentDAO();
    ~InvestmentDAO();

    // Operações CRUD
    bool Insert(const Investment& investment);
    bool Update(const Investment& oldInvestment, const Investment& newInvestment);
    bool Delete(const wxDateTime& investmentDate, const wxString& description);

    // Consultas
    std::vector<Investment> GetAll();
    std::vector<Investment> GetByMonth(int month, int year);
    std::vector<Investment> GetByDateRange(const wxDateTime& startDate, const wxDateTime& endDate);
    std::unique_ptr<Investment> GetByKey(const wxDateTime& investmentDate, const wxString& description);

    // Estatísticas
    double GetTotalByMonth(int month, int year);
    double GetTotalByDateRange(const wxDateTime& startDate, const wxDateTime& endDate);
    double GetTotalByType(const wxString& investmentType);

private:
    // Converter linha do banco para objeto Investment
    Investment RowToInvestment(const wxArrayString& row);
};

#endif // INVESTMENTDAO_H
