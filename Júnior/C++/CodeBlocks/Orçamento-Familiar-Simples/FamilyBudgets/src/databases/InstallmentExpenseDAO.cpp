#include "InstallmentExpenseDAO.h"
#include "Database.h"
#include "../utils/DateUtils.h"
#include "../utils/StringUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/InstallmentCalculator.h"

InstallmentExpenseDAO::InstallmentExpenseDAO()
{
}

InstallmentExpenseDAO::~InstallmentExpenseDAO()
{
}

bool InstallmentExpenseDAO::Insert(const InstallmentExpense& expense)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("INSERT INTO installment_expenses (purchase_date, description, total_amount, ")
        wxT("installments, expense_type, card_name, active) ")
        wxT("VALUES ('%s', '%s', %s, %d, '%s', '%s', %d)"),
        DateUtils::FormatDateDB(expense.purchaseDate),
        StringUtils::EscapeSQL(expense.description),
        StringUtils::FormatDoubleForSQL(expense.totalAmount),
        expense.installments,
        StringUtils::EscapeSQL(expense.expenseType),
        StringUtils::EscapeSQL(expense.cardName),
        expense.active ? 1 : 0
    );

    return db.Execute(sql);
}

bool InstallmentExpenseDAO::Update(const InstallmentExpense& oldExpense, const InstallmentExpense& newExpense)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("UPDATE installment_expenses SET ")
        wxT("purchase_date = '%s', description = '%s', total_amount = %s, ")
        wxT("installments = %d, expense_type = '%s', card_name = '%s', active = %d ")
        wxT("WHERE purchase_date = '%s' AND description = '%s' AND card_name = '%s'"),
        DateUtils::FormatDateDB(newExpense.purchaseDate),
        StringUtils::EscapeSQL(newExpense.description),
        StringUtils::FormatDoubleForSQL(newExpense.totalAmount),
        newExpense.installments,
        StringUtils::EscapeSQL(newExpense.expenseType),
        StringUtils::EscapeSQL(newExpense.cardName),
        newExpense.active ? 1 : 0,
        DateUtils::FormatDateDB(oldExpense.purchaseDate),
        StringUtils::EscapeSQL(oldExpense.description),
        StringUtils::EscapeSQL(oldExpense.cardName)
    );

    return db.Execute(sql);
}

bool InstallmentExpenseDAO::Delete(const wxDateTime& purchaseDate, const wxString& description, const wxString& cardName)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("DELETE FROM installment_expenses ")
        wxT("WHERE purchase_date = '%s' AND description = '%s' AND card_name = '%s'"),
        DateUtils::FormatDateDB(purchaseDate),
        StringUtils::EscapeSQL(description),
        StringUtils::EscapeSQL(cardName)
    );

    return db.Execute(sql);
}

std::vector<InstallmentExpense> InstallmentExpenseDAO::GetAll()
{
    std::vector<InstallmentExpense> expenses;
    Database& db = Database::GetInstance();

    wxString sql = wxT("SELECT purchase_date, description, total_amount, installments, ")
                   wxT("expense_type, card_name, active ")
                   wxT("FROM installment_expenses ORDER BY purchase_date DESC, description");

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            InstallmentExpense expense = RowToInstallmentExpense(row);

            // Calcular data de vencimento e valor da parcela
            int closingDay, dueDay;
            if (GetCardInfo(expense.cardName, closingDay, dueDay))
            {
                expense.dueDate = InstallmentCalculator::CalculateDueDate(
                    expense.purchaseDate, 1, closingDay, dueDay);
                expense.installmentAmount = InstallmentCalculator::CalculateInstallmentAmount(
                    expense.totalAmount, expense.installments);
            }

            expenses.push_back(expense);
        }
    }

    return expenses;
}

std::vector<InstallmentExpense> InstallmentExpenseDAO::GetActive()
{
    std::vector<InstallmentExpense> expenses;
    Database& db = Database::GetInstance();

    wxString sql = wxT("SELECT purchase_date, description, total_amount, installments, ")
                   wxT("expense_type, card_name, active ")
                   wxT("FROM installment_expenses WHERE active = 1 ")
                   wxT("ORDER BY purchase_date DESC, description");

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            InstallmentExpense expense = RowToInstallmentExpense(row);

            // Calcular data de vencimento e valor da parcela
            int closingDay, dueDay;
            if (GetCardInfo(expense.cardName, closingDay, dueDay))
            {
                expense.dueDate = InstallmentCalculator::CalculateDueDate(
                    expense.purchaseDate, 1, closingDay, dueDay);
                expense.installmentAmount = InstallmentCalculator::CalculateInstallmentAmount(
                    expense.totalAmount, expense.installments);
            }

            expenses.push_back(expense);
        }
    }

    return expenses;
}

std::unique_ptr<InstallmentExpense> InstallmentExpenseDAO::GetByKey(const wxDateTime& purchaseDate,
                                                                     const wxString& description,
                                                                     const wxString& cardName)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT purchase_date, description, total_amount, installments, ")
        wxT("expense_type, card_name, active ")
        wxT("FROM installment_expenses ")
        wxT("WHERE date(purchase_date) = date('%s') AND description = '%s' AND card_name = '%s'"),
        DateUtils::FormatDateDB(purchaseDate),
        StringUtils::EscapeSQL(description),
        StringUtils::EscapeSQL(cardName)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        auto expense = std::make_unique<InstallmentExpense>(RowToInstallmentExpense(result->rows[0]));

        // Calcular data de vencimento e valor da parcela
        int closingDay, dueDay;
        if (GetCardInfo(expense->cardName, closingDay, dueDay))
        {
            expense->dueDate = InstallmentCalculator::CalculateDueDate(
                expense->purchaseDate, 1, closingDay, dueDay);
            expense->installmentAmount = InstallmentCalculator::CalculateInstallmentAmount(
                expense->totalAmount, expense->installments);
        }

        return expense;
    }

    return nullptr;
}

bool InstallmentExpenseDAO::SetActive(const wxDateTime& purchaseDate, const wxString& description,
                                      const wxString& cardName, bool active)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("UPDATE installment_expenses SET active = %d ")
        wxT("WHERE purchase_date = '%s' AND description = '%s' AND card_name = '%s'"),
        active ? 1 : 0,
        DateUtils::FormatDateDB(purchaseDate),
        StringUtils::EscapeSQL(description),
        StringUtils::EscapeSQL(cardName)
    );

    return db.Execute(sql);
}

bool InstallmentExpenseDAO::GetCardInfo(const wxString& cardName, int& closingDay, int& dueDay)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT closing_day, due_day FROM cards WHERE name = '%s'"),
        StringUtils::EscapeSQL(cardName)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        closingDay = wxAtoi(result->rows[0][0]);
        dueDay = wxAtoi(result->rows[0][1]);
        return true;
    }

    return false;
}

InstallmentExpense InstallmentExpenseDAO::RowToInstallmentExpense(const wxArrayString& row)
{
    InstallmentExpense expense;

    if (row.GetCount() >= 7)
    {
        expense.purchaseDate = DateUtils::ParseDateDB(row[0]);
        expense.description = row[1];
        expense.totalAmount = MoneyUtils::ParseMoney(row[2]);
        expense.installments = wxAtoi(row[3]);
        expense.expenseType = row[4];
        expense.cardName = row[5];
        expense.active = wxAtoi(row[6]) == 1;
    }

    return expense;
}
