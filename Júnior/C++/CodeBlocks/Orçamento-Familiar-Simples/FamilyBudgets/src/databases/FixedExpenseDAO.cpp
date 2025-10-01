#include "FixedExpenseDAO.h"
#include "Database.h"
#include "../utils/StringUtils.h"
#include "../utils/MoneyUtils.h"

FixedExpenseDAO::FixedExpenseDAO()
{
}

FixedExpenseDAO::~FixedExpenseDAO()
{
}

bool FixedExpenseDAO::Insert(const FixedExpense& expense)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("INSERT INTO fixed_expenses (due_day, description, amount, expense_type, ")
        wxT("payment_method, payment_category, active) ")
        wxT("VALUES (%d, '%s', %s, '%s', '%s', '%s', %d)"),
        expense.dueDay,
        StringUtils::EscapeSQL(expense.description),
        StringUtils::FormatDoubleForSQL(expense.amount),
        StringUtils::EscapeSQL(expense.expenseType),
        StringUtils::EscapeSQL(expense.paymentMethod),
        StringUtils::EscapeSQL(expense.paymentCategory),
        expense.active ? 1 : 0
    );
    
    return db.Execute(sql);
}

bool FixedExpenseDAO::Update(const FixedExpense& oldExpense, const FixedExpense& newExpense)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("UPDATE fixed_expenses SET ")
        wxT("due_day = %d, description = '%s', amount = %s, ")
        wxT("expense_type = '%s', payment_method = '%s', payment_category = '%s', ")
        wxT("active = %d ")
        wxT("WHERE due_day = %d AND description = '%s'"),
        newExpense.dueDay,
        StringUtils::EscapeSQL(newExpense.description),
        StringUtils::FormatDoubleForSQL(newExpense.amount),
        StringUtils::EscapeSQL(newExpense.expenseType),
        StringUtils::EscapeSQL(newExpense.paymentMethod),
        StringUtils::EscapeSQL(newExpense.paymentCategory),
        newExpense.active ? 1 : 0,
        oldExpense.dueDay,
        StringUtils::EscapeSQL(oldExpense.description)
    );
    
    return db.Execute(sql);
}

bool FixedExpenseDAO::Delete(int dueDay, const wxString& description)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("DELETE FROM fixed_expenses WHERE due_day = %d AND description = '%s'"),
        dueDay,
        StringUtils::EscapeSQL(description)
    );
    
    return db.Execute(sql);
}

std::vector<FixedExpense> FixedExpenseDAO::GetAll()
{
    std::vector<FixedExpense> expenses;
    Database& db = Database::GetInstance();
    
    wxString sql = wxT("SELECT due_day, description, amount, expense_type, ")
                   wxT("payment_method, payment_category, active ")
                   wxT("FROM fixed_expenses ORDER BY due_day, description");
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            expenses.push_back(RowToFixedExpense(row));
        }
    }
    
    return expenses;
}

std::vector<FixedExpense> FixedExpenseDAO::GetActive()
{
    std::vector<FixedExpense> expenses;
    Database& db = Database::GetInstance();
    
    wxString sql = wxT("SELECT due_day, description, amount, expense_type, ")
                   wxT("payment_method, payment_category, active ")
                   wxT("FROM fixed_expenses WHERE active = 1 ORDER BY due_day, description");
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            expenses.push_back(RowToFixedExpense(row));
        }
    }
    
    return expenses;
}

std::unique_ptr<FixedExpense> FixedExpenseDAO::GetByKey(int dueDay, const wxString& description)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT due_day, description, amount, expense_type, ")
        wxT("payment_method, payment_category, active ")
        wxT("FROM fixed_expenses ")
        wxT("WHERE due_day = %d AND description = '%s'"),
        dueDay,
        StringUtils::EscapeSQL(description)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<FixedExpense>(RowToFixedExpense(result->rows[0]));
    }
    
    return nullptr;
}

bool FixedExpenseDAO::SetActive(int dueDay, const wxString& description, bool active)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("UPDATE fixed_expenses SET active = %d ")
        wxT("WHERE due_day = %d AND description = '%s'"),
        active ? 1 : 0,
        dueDay,
        StringUtils::EscapeSQL(description)
    );
    
    return db.Execute(sql);
}

FixedExpense FixedExpenseDAO::RowToFixedExpense(const wxArrayString& row)
{
    FixedExpense expense;
    
    if (row.GetCount() >= 7)
    {
        expense.dueDay = wxAtoi(row[0]);
        expense.description = row[1];
        expense.amount = MoneyUtils::ParseMoney(row[2]);
        expense.expenseType = row[3];
        expense.paymentMethod = row[4];
        expense.paymentCategory = row[5];
        expense.active = wxAtoi(row[6]) == 1;
    }
    
    return expense;
}