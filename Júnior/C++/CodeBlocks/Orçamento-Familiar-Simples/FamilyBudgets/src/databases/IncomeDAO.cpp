#include "IncomeDAO.h"
#include "Database.h"
#include "../utils/DateUtils.h"
#include "../utils/StringUtils.h"
#include "../utils/MoneyUtils.h"

IncomeDAO::IncomeDAO()
{
}

IncomeDAO::~IncomeDAO()
{
}

bool IncomeDAO::Insert(const Income& income)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("INSERT INTO incomes (receipt_date, description, amount, income_type, destination) ")
        wxT("VALUES ('%s', '%s', %s, '%s', '%s')"),
        DateUtils::FormatDateDB(income.receiptDate),
        StringUtils::EscapeSQL(income.description),
        StringUtils::FormatDoubleForSQL(income.amount),
        StringUtils::EscapeSQL(income.incomeType),
        StringUtils::EscapeSQL(income.destination)
    );

    return db.Execute(sql);
}

bool IncomeDAO::Update(const Income& oldIncome, const Income& newIncome)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("UPDATE incomes SET ")
        wxT("receipt_date = '%s', description = '%s', amount = %s, ")
        wxT("income_type = '%s', destination = '%s' ")
        wxT("WHERE receipt_date = '%s' AND description = '%s'"),
        DateUtils::FormatDateDB(newIncome.receiptDate),
        StringUtils::EscapeSQL(newIncome.description),
        StringUtils::FormatDoubleForSQL(newIncome.amount),
        StringUtils::EscapeSQL(newIncome.incomeType),
        StringUtils::EscapeSQL(newIncome.destination),
        DateUtils::FormatDateDB(oldIncome.receiptDate),
        StringUtils::EscapeSQL(oldIncome.description)
    );

    return db.Execute(sql);
}

bool IncomeDAO::Delete(const wxDateTime& receiptDate, const wxString& description)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("DELETE FROM incomes WHERE receipt_date = '%s' AND description = '%s'"),
        DateUtils::FormatDateDB(receiptDate),
        StringUtils::EscapeSQL(description)
    );

    return db.Execute(sql);
}

std::vector<Income> IncomeDAO::GetAll()
{
    std::vector<Income> incomes;
    Database& db = Database::GetInstance();

    wxString sql = wxT("SELECT receipt_date, description, amount, income_type, ")
                   wxT("destination FROM incomes ORDER BY receipt_date DESC");

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            incomes.push_back(RowToIncome(row));
        }
    }

    return incomes;
}

std::vector<Income> IncomeDAO::GetByMonth(int month, int year)
{
    wxDateTime startDate = DateUtils::GetFirstDayOfMonth(month, year);
    wxDateTime endDate = DateUtils::GetLastDayOfMonth(month, year);

    return GetByDateRange(startDate, endDate);
}

std::vector<Income> IncomeDAO::GetByDateRange(const wxDateTime& startDate, const wxDateTime& endDate)
{
    std::vector<Income> incomes;
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT receipt_date, description, amount, income_type, ")
        wxT("destination FROM incomes ")
        wxT("WHERE receipt_date >= '%s' AND receipt_date <= '%s' ")
        wxT("ORDER BY receipt_date DESC"),
        DateUtils::FormatDateDB(startDate),
        DateUtils::FormatDateDB(endDate)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            incomes.push_back(RowToIncome(row));
        }
    }

    return incomes;
}

std::unique_ptr<Income> IncomeDAO::GetByKey(const wxDateTime& receiptDate, const wxString& description)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT receipt_date, description, amount, income_type, ")
        wxT("destination FROM incomes ")
        wxT("WHERE date(receipt_date) = date('%s') AND description = '%s'"),
        DateUtils::FormatDateDB(receiptDate),
        StringUtils::EscapeSQL(description)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<Income>(RowToIncome(result->rows[0]));
    }

    return nullptr;
}

double IncomeDAO::GetTotalByMonth(int month, int year)
{
    wxDateTime startDate = DateUtils::GetFirstDayOfMonth(month, year);
    wxDateTime endDate = DateUtils::GetLastDayOfMonth(month, year);

    return GetTotalByDateRange(startDate, endDate);
}

double IncomeDAO::GetTotalByDateRange(const wxDateTime& startDate, const wxDateTime& endDate)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT SUM(amount) FROM incomes ")
        wxT("WHERE receipt_date >= '%s' AND receipt_date <= '%s'"),
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

Income IncomeDAO::RowToIncome(const wxArrayString& row)
{
    Income income;

    if (row.GetCount() >= 5)
    {
        income.receiptDate = DateUtils::ParseDateDB(row[0]);
        income.description = row[1];
        income.amount = MoneyUtils::ParseMoney(row[2]);
        income.incomeType = row[3];
        income.destination = row[4];
    }

    return income;
}
