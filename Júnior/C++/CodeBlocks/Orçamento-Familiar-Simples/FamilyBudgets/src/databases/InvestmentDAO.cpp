#include "InvestmentDAO.h"
#include "Database.h"
#include "../utils/DateUtils.h"
#include "../utils/StringUtils.h"
#include "../utils/MoneyUtils.h"

InvestmentDAO::InvestmentDAO()
{
}

InvestmentDAO::~InvestmentDAO()
{
}

bool InvestmentDAO::Insert(const Investment& investment)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("INSERT INTO investments (investment_date, description, amount, investment_type, source) ")
        wxT("VALUES ('%s', '%s', %s, '%s', '%s')"),
        DateUtils::FormatDateDB(investment.investmentDate),
        StringUtils::EscapeSQL(investment.description),
        StringUtils::FormatDoubleForSQL(investment.amount),
        StringUtils::EscapeSQL(investment.investmentType),
        StringUtils::EscapeSQL(investment.source)
    );

    return db.Execute(sql);
}

bool InvestmentDAO::Update(const Investment& oldInvestment, const Investment& newInvestment)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("UPDATE investments SET ")
        wxT("investment_date = '%s', description = '%s', amount = %s, ")
        wxT("investment_type = '%s', source = '%s' ")
        wxT("WHERE investment_date = '%s' AND description = '%s'"),
        DateUtils::FormatDateDB(newInvestment.investmentDate),
        StringUtils::EscapeSQL(newInvestment.description),
        StringUtils::FormatDoubleForSQL(newInvestment.amount),
        StringUtils::EscapeSQL(newInvestment.investmentType),
        StringUtils::EscapeSQL(newInvestment.source),
        DateUtils::FormatDateDB(oldInvestment.investmentDate),
        StringUtils::EscapeSQL(oldInvestment.description)
    );

    return db.Execute(sql);
}

bool InvestmentDAO::Delete(const wxDateTime& investmentDate, const wxString& description)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("DELETE FROM investments WHERE investment_date = '%s' AND description = '%s'"),
        DateUtils::FormatDateDB(investmentDate),
        StringUtils::EscapeSQL(description)
    );

    return db.Execute(sql);
}

std::vector<Investment> InvestmentDAO::GetAll()
{
    std::vector<Investment> investments;
    Database& db = Database::GetInstance();

    wxString sql = wxT("SELECT investment_date, description, amount, investment_type, ")
                   wxT("source FROM investments ORDER BY investment_date DESC");

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            investments.push_back(RowToInvestment(row));
        }
    }

    return investments;
}

std::vector<Investment> InvestmentDAO::GetByMonth(int month, int year)
{
    wxDateTime startDate = DateUtils::GetFirstDayOfMonth(month, year);
    wxDateTime endDate = DateUtils::GetLastDayOfMonth(month, year);

    return GetByDateRange(startDate, endDate);
}

std::vector<Investment> InvestmentDAO::GetByDateRange(const wxDateTime& startDate, const wxDateTime& endDate)
{
    std::vector<Investment> investments;
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT investment_date, description, amount, investment_type, ")
        wxT("source FROM investments ")
        wxT("WHERE investment_date >= '%s' AND investment_date <= '%s' ")
        wxT("ORDER BY investment_date DESC"),
        DateUtils::FormatDateDB(startDate),
        DateUtils::FormatDateDB(endDate)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            investments.push_back(RowToInvestment(row));
        }
    }

    return investments;
}

std::unique_ptr<Investment> InvestmentDAO::GetByKey(const wxDateTime& investmentDate, const wxString& description)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT investment_date, description, amount, investment_type, ")
        wxT("source FROM investments ")
        wxT("WHERE date(investment_date) = date('%s') AND description = '%s'"),
        DateUtils::FormatDateDB(investmentDate),
        StringUtils::EscapeSQL(description)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<Investment>(RowToInvestment(result->rows[0]));
    }

    return nullptr;
}

double InvestmentDAO::GetTotalByMonth(int month, int year)
{
    wxDateTime startDate = DateUtils::GetFirstDayOfMonth(month, year);
    wxDateTime endDate = DateUtils::GetLastDayOfMonth(month, year);

    return GetTotalByDateRange(startDate, endDate);
}

double InvestmentDAO::GetTotalByDateRange(const wxDateTime& startDate, const wxDateTime& endDate)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT SUM(amount) FROM investments ")
        wxT("WHERE investment_date >= '%s' AND investment_date <= '%s'"),
        DateUtils::FormatDateDB(startDate),
        DateUtils::FormatDateDB(endDate)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        if (!result->rows[0][0].IsEmpty())
        {
            return MoneyUtils::ParseMoney(result->rows[0][0]);
        }
    }

    return 0.0;
}

double InvestmentDAO::GetTotalByType(const wxString& investmentType)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT SUM(amount) FROM investments WHERE investment_type = '%s'"),
        StringUtils::EscapeSQL(investmentType)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        if (!result->rows[0][0].IsEmpty())
        {
            return MoneyUtils::ParseMoney(result->rows[0][0]);
        }
    }

    return 0.0;
}

Investment InvestmentDAO::RowToInvestment(const wxArrayString& row)
{
    Investment investment;

    if (row.GetCount() >= 5)
    {
        investment.investmentDate = DateUtils::ParseDateDB(row[0]);
        investment.description = row[1];
        investment.amount = MoneyUtils::ParseMoney(row[2]);
        investment.investmentType = row[3];
        investment.source = row[4];
    }

    return investment;
}
