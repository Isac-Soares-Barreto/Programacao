#include "ExpenseDAO.h"
#include "Database.h"
#include "../utils/DateUtils.h"
#include "../utils/StringUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/InstallmentCalculator.h"
#include <wx/msgdlg.h>

ExpenseDAO::ExpenseDAO()
{
}

ExpenseDAO::~ExpenseDAO()
{
}

bool ExpenseDAO::Insert(const Expense& expense)
{
    Database& db = Database::GetInstance();

    // Verificar se o banco está aberto
    if (!db.IsOpen())
    {
        wxLogError(wxT("Banco de dados não está aberto!"));
        return false;
    }

    // Formatar valores corretamente
    wxString dueDateStr = DateUtils::FormatDateDB(expense.dueDate);
    wxString paymentDateStr = expense.paymentDate.IsValid() ?
        wxT("'") + DateUtils::FormatDateDB(expense.paymentDate) + wxT("'") : wxT("NULL");
    wxString adjustedAmountStr = expense.adjustedAmount > 0 ?
        StringUtils::FormatDoubleForSQL(expense.adjustedAmount) : wxT("NULL");

    wxString sql = wxString::Format(
        wxT("INSERT INTO expenses (due_date, description, amount, expense_type, ")
        wxT("payment_method, payment_category, payment_date, status, adjusted_amount) ")
        wxT("VALUES ('%s', '%s', %s, '%s', '%s', '%s', %s, '%s', %s)"),
        dueDateStr,
        StringUtils::EscapeSQL(expense.description),
        StringUtils::FormatDoubleForSQL(expense.amount),
        StringUtils::EscapeSQL(expense.expenseType),
        StringUtils::EscapeSQL(expense.paymentMethod),
        StringUtils::EscapeSQL(expense.paymentCategory),
        paymentDateStr,
        ExpenseStatusToString(expense.status),
        adjustedAmountStr
    );

    // Debug: mostrar a query
    wxLogDebug(wxT("SQL Insert: %s"), sql);

    bool result = db.Execute(sql);

    if (!result)
    {
        wxString error = db.GetLastError();
        wxLogError(wxT("Erro ao inserir despesa: %s"), error);
    }

    return result;
}

bool ExpenseDAO::Update(const Expense& oldExpense, const Expense& newExpense)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("UPDATE expenses SET ")
        wxT("due_date = '%s', description = '%s', amount = %s, ")
        wxT("expense_type = '%s', payment_method = '%s', payment_category = '%s', ")
        wxT("payment_date = %s, status = '%s', adjusted_amount = %s ")
        wxT("WHERE due_date = '%s' AND description = '%s'"),
        DateUtils::FormatDateDB(newExpense.dueDate),
        StringUtils::EscapeSQL(newExpense.description),
        StringUtils::FormatDoubleForSQL(newExpense.amount),
        StringUtils::EscapeSQL(newExpense.expenseType),
        StringUtils::EscapeSQL(newExpense.paymentMethod),
        StringUtils::EscapeSQL(newExpense.paymentCategory),
        newExpense.paymentDate.IsValid() ?
            wxT("'") + DateUtils::FormatDateDB(newExpense.paymentDate) + wxT("'") : wxT("NULL"),
        ExpenseStatusToString(newExpense.status),
        newExpense.adjustedAmount > 0 ?
            StringUtils::FormatDoubleForSQL(newExpense.adjustedAmount) : wxT("NULL"),
        DateUtils::FormatDateDB(oldExpense.dueDate),
        StringUtils::EscapeSQL(oldExpense.description)
    );

    return db.Execute(sql);
}

bool ExpenseDAO::Delete(const wxDateTime& dueDate, const wxString& description)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("DELETE FROM expenses WHERE due_date = '%s' AND description = '%s'"),
        DateUtils::FormatDateDB(dueDate),
        StringUtils::EscapeSQL(description)
    );

    return db.Execute(sql);
}

std::vector<Expense> ExpenseDAO::GetAll()
{
    std::vector<Expense> expenses;
    Database& db = Database::GetInstance();

    wxString sql = wxT("SELECT due_date, description, amount, expense_type, ")
                   wxT("payment_method, payment_category, payment_date, status, ")
                   wxT("adjusted_amount FROM expenses ORDER BY due_date DESC");

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            expenses.push_back(RowToExpense(row));
        }
    }

    return expenses;
}

std::vector<Expense> ExpenseDAO::GetByMonth(int month, int year)
{
    wxDateTime startDate = DateUtils::GetFirstDayOfMonth(month, year);
    wxDateTime endDate = DateUtils::GetLastDayOfMonth(month, year);

    return GetByDateRange(startDate, endDate);
}

std::vector<Expense> ExpenseDAO::GetByDateRange(const wxDateTime& startDate, const wxDateTime& endDate)
{
    std::vector<Expense> expenses;
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT due_date, description, amount, expense_type, ")
        wxT("payment_method, payment_category, payment_date, status, ")
        wxT("adjusted_amount FROM expenses ")
        wxT("WHERE due_date >= '%s' AND due_date <= '%s' ")
        wxT("ORDER BY due_date"),
        DateUtils::FormatDateDB(startDate),
        DateUtils::FormatDateDB(endDate)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            expenses.push_back(RowToExpense(row));
        }
    }

    return expenses;
}

std::unique_ptr<Expense> ExpenseDAO::GetByKey(const wxDateTime& dueDate, const wxString& description)
{
    Database& db = Database::GetInstance();

    wxString sql = wxString::Format(
        wxT("SELECT due_date, description, amount, expense_type, ")
        wxT("payment_method, payment_category, payment_date, status, ")
        wxT("adjusted_amount FROM expenses ")
        wxT("WHERE date(due_date) = date('%s') AND description = '%s'"),  // Usar date() para comparação
        DateUtils::FormatDateDB(dueDate),
        StringUtils::EscapeSQL(description)
    );

    // Debug
    wxLogDebug(wxT("GetByKey SQL: %s"), sql);

    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<Expense>(RowToExpense(result->rows[0]));
    }

    return nullptr;
}

bool ExpenseDAO::ImportFixedExpenses(int month, int year)
{
    Database& db = Database::GetInstance();

    // Buscar despesas fixas ativas
    wxString sql = wxT("SELECT due_day, description, amount, expense_type, ")
                   wxT("payment_method, payment_category FROM fixed_expenses ")
                   wxT("WHERE active = 1 ORDER BY due_day");

    auto result = db.Query(sql);
    if (!result || result->IsEmpty())
    {
        wxMessageBox(wxT("Não há despesas fixas ativas para importar!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING);
        return false;
    }

    // Contadores para feedback
    int totalFixed = result->rows.size();
    int imported = 0;
    int skipped = 0;
    wxArrayString skippedDescriptions;
    wxArrayString importedDescriptions;
    wxArrayString errorDescriptions;

    // Log para debug
    wxLogDebug(wxT("Importando %d despesas fixas para %02d/%04d"), totalFixed, month, year);

    db.BeginTransaction();

    for (const auto& row : result->rows)
    {
        Expense expense;

        // Calcular data de vencimento
        int dueDay = wxAtoi(row[0]);

        // Garantir que o dia seja válido para o mês
        wxDateTime lastDayOfMonth = DateUtils::GetLastDayOfMonth(month, year);
        int maxDay = lastDayOfMonth.GetDay();
        int actualDay = wxMin(dueDay, maxDay);

        // Criar a data de vencimento corretamente
        expense.dueDate = wxDateTime(actualDay, (wxDateTime::Month)(month - 1), year);

        expense.description = row[1];
        expense.amount = MoneyUtils::ParseMoney(row[2]);
        expense.expenseType = row[3];
        expense.paymentMethod = row[4];
        expense.paymentCategory = row[5];
        expense.status = ExpenseStatus::TO_PAY;
        expense.adjustedAmount = 0.0;

        // Log para debug
        wxLogDebug(wxT("Tentando importar: %s - Vencimento: %s"),
                   expense.description,
                   DateUtils::FormatDate(expense.dueDate));

        // Verificar se já existe
        auto existing = GetByKey(expense.dueDate, expense.description);

        if (!existing)
        {
            // Não existe, pode inserir
            if (Insert(expense))
            {
                imported++;
                importedDescriptions.Add(expense.description);
                wxLogDebug(wxT("Importada com sucesso: %s"), expense.description);
            }
            else
            {
                errorDescriptions.Add(expense.description + wxT(" - Erro: ") + db.GetLastError());
                wxLogDebug(wxT("Erro ao importar: %s - %s"), expense.description, db.GetLastError());
            }
        }
        else
        {
            // Já existe, pular
            skipped++;
            skippedDescriptions.Add(expense.description);
            wxLogDebug(wxT("Já existe: %s"), expense.description);
        }
    }

    bool success = db.CommitTransaction();

    if (!success)
    {
        wxMessageBox(wxT("Erro ao confirmar transação:\n") + db.GetLastError(),
                     wxT("Erro"), wxOK | wxICON_ERROR);
        return false;
    }

    // Montar mensagem de feedback detalhada
    wxString message;

    if (imported > 0 && skipped == 0 && errorDescriptions.IsEmpty())
    {
        message = wxString::Format(
            wxT("Importação concluída com sucesso!\n\n")
            wxT("Mês/Ano: %02d/%04d\n")
            wxT("Total de despesas fixas ativas: %d\n")
            wxT("Despesas importadas: %d\n\n")
            wxT("Despesas importadas:\n"),
            month, year, totalFixed, imported
        );

        for (const wxString& desc : importedDescriptions)
        {
            message += wxT("✓ ") + desc + wxT("\n");
        }
    }
    else if (imported == 0 && skipped > 0)
    {
        message = wxString::Format(
            wxT("Nenhuma despesa foi importada!\n\n")
            wxT("Mês/Ano: %02d/%04d\n")
            wxT("Total de despesas fixas ativas: %d\n")
            wxT("Despesas já existentes: %d\n\n")
            wxT("As seguintes despesas já existem para este mês:\n"),
            month, year, totalFixed, skipped
        );

        int count = 0;
        for (const wxString& desc : skippedDescriptions)
        {
            if (count < 10)
            {
                message += wxT("• ") + desc + wxT("\n");
                count++;
            }
        }
        if (skippedDescriptions.size() > 10)
        {
            message += wxString::Format(wxT("... e mais %d despesas"),
                                      (int)(skippedDescriptions.size() - 10));
        }
    }
    else
    {
        // Situação mista
        message = wxString::Format(
            wxT("Importação concluída!\n\n")
            wxT("Mês/Ano: %02d/%04d\n")
            wxT("Total de despesas fixas ativas: %d\n")
            wxT("Despesas importadas: %d\n")
            wxT("Despesas já existentes: %d\n"),
            month, year, totalFixed, imported, skipped
        );

        if (imported > 0)
        {
            message += wxT("\nImportadas:\n");
            for (const wxString& desc : importedDescriptions)
            {
                message += wxT("✓ ") + desc + wxT("\n");
            }
        }

        if (!errorDescriptions.IsEmpty())
        {
            message += wxT("\nErros:\n");
            for (const wxString& error : errorDescriptions)
            {
                message += wxT("✗ ") + error + wxT("\n");
            }
        }
    }

    wxMessageBox(message,
                imported > 0 ? wxT("Sucesso") : wxT("Aviso"),
                imported > 0 ? wxOK | wxICON_INFORMATION : wxOK | wxICON_WARNING);

    return imported > 0;
}

bool ExpenseDAO::ImportInstallmentExpenses(int month, int year)
{
    Database& db = Database::GetInstance();

    // Buscar despesas parceladas ativas com informações do cartão
    wxString sql = wxT("SELECT ie.purchase_date, ie.description, ie.total_amount, ")
                   wxT("ie.installments, ie.expense_type, ie.card_name, ")
                   wxT("c.closing_day, c.due_day ")
                   wxT("FROM installment_expenses ie ")
                   wxT("INNER JOIN cards c ON ie.card_name = c.name ")
                   wxT("WHERE ie.active = 1");

    wxLogDebug(wxT("Importando despesas parceladas para %02d/%04d"), month, year);

    auto result = db.Query(sql);
    if (!result || result->IsEmpty())
    {
        wxMessageBox(wxT("Não há despesas parceladas ativas para importar!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING);
        return false;
    }

    int totalImported = 0;
    int totalSkipped = 0;
    wxArrayString importedDescriptions;
    wxArrayString skippedDescriptions;

    db.BeginTransaction();

    // Processar cada despesa parcelada
    for (const auto& row : result->rows)
    {
        wxDateTime purchaseDate = DateUtils::ParseDateDB(row[0]);
        wxString description = row[1];
        double totalAmount = MoneyUtils::ParseMoney(row[2]);
        int installments = wxAtoi(row[3]);
        wxString expenseType = row[4];
        wxString cardName = row[5];
        int closingDay = wxAtoi(row[6]);
        int dueDay = wxAtoi(row[7]);

        wxLogDebug(wxT("Processando: %s - %d parcelas"), description, installments);

        // Calcular todas as parcelas usando InstallmentCalculator
        std::vector<CalculatedInstallment> calculatedInstallments =
            InstallmentCalculator::CalculateAllInstallments(
                purchaseDate,
                description,
                totalAmount,
                installments,
                closingDay,
                dueDay
            );

        // Filtrar e importar apenas as parcelas do mês/ano solicitado
        for (const auto& calcInstallment : calculatedInstallments)
        {
            // Verificar se a parcela vence no mês/ano solicitado
            if (calcInstallment.dueDate.GetMonth() + 1 == month &&
                calcInstallment.dueDate.GetYear() == year)
            {
                wxLogDebug(wxT("Parcela %d vence em %s"),
                          calcInstallment.installmentNumber,
                          DateUtils::FormatDate(calcInstallment.dueDate));

                // Verificar se já existe
                auto existing = GetByKey(calcInstallment.dueDate, calcInstallment.description);

                if (!existing)
                {
                    // Criar despesa
                    Expense expense;
                    expense.dueDate = calcInstallment.dueDate;
                    expense.description = calcInstallment.description;
                    expense.amount = calcInstallment.amount;
                    expense.expenseType = expenseType;
                    expense.paymentMethod = wxT("Cartão");
                    expense.paymentCategory = cardName;
                    expense.status = ExpenseStatus::TO_PAY;
                    expense.adjustedAmount = 0.0;

                    if (Insert(expense))
                    {
                        totalImported++;
                        importedDescriptions.Add(calcInstallment.description);
                        wxLogDebug(wxT("Parcela importada: %s"), calcInstallment.description);
                    }
                    else
                    {
                        wxLogError(wxT("Erro ao inserir parcela: %s"), calcInstallment.description);
                    }
                }
                else
                {
                    totalSkipped++;
                    skippedDescriptions.Add(calcInstallment.description);
                    wxLogDebug(wxT("Parcela já existe: %s"), calcInstallment.description);
                }
            }
        }
    }

    bool success = db.CommitTransaction();

    if (!success)
    {
        wxMessageBox(wxT("Erro ao confirmar transação:\n") + db.GetLastError(),
                     wxT("Erro"), wxOK | wxICON_ERROR);
        return false;
    }

    // Montar mensagem de feedback detalhada
    wxString message;
    wxString monthName = wxDateTime::GetMonthName((wxDateTime::Month)(month - 1));

    if (totalImported > 0 && totalSkipped == 0)
    {
        message = wxString::Format(
            wxT("Importação concluída com sucesso!\n\n")
            wxT("Mês/Ano: %s/%d\n")
            wxT("Parcelas importadas: %d\n\n")
            wxT("Parcelas importadas:\n"),
            monthName, year, totalImported
        );

        int count = 0;
        for (const wxString& desc : importedDescriptions)
        {
            if (count < 10)
            {
                message += wxT("✓ ") + desc + wxT("\n");
                count++;
            }
        }
        if (importedDescriptions.size() > 10)
        {
            message += wxString::Format(wxT("... e mais %d parcelas"),
                                      (int)(importedDescriptions.size() - 10));
        }
    }
    else if (totalImported == 0 && totalSkipped > 0)
    {
        message = wxString::Format(
            wxT("Nenhuma parcela foi importada!\n\n")
            wxT("Mês/Ano: %s/%d\n")
            wxT("Parcelas já existentes: %d\n\n")
            wxT("As seguintes parcelas já existem para este mês:\n"),
            monthName, year, totalSkipped
        );

        int count = 0;
        for (const wxString& desc : skippedDescriptions)
        {
            if (count < 10)
            {
                message += wxT("• ") + desc + wxT("\n");
                count++;
            }
        }
        if (skippedDescriptions.size() > 10)
        {
            message += wxString::Format(wxT("... e mais %d parcelas"),
                                      (int)(skippedDescriptions.size() - 10));
        }
    }
    else if (totalImported == 0 && totalSkipped == 0)
    {
        message = wxString::Format(
            wxT("Nenhuma parcela encontrada!\n\n")
            wxT("Mês/Ano: %s/%d\n\n")
            wxT("Não há parcelas de despesas parceladas ativas\n")
            wxT("que vencem neste mês."),
            monthName, year
        );
    }
    else
    {
        // Situação mista
        message = wxString::Format(
            wxT("Importação concluída!\n\n")
            wxT("Mês/Ano: %s/%d\n")
            wxT("Parcelas importadas: %d\n")
            wxT("Parcelas já existentes: %d\n\n"),
            monthName, year, totalImported, totalSkipped
        );

        if (totalImported > 0)
        {
            message += wxT("Importadas:\n");
            int count = 0;
            for (const wxString& desc : importedDescriptions)
            {
                if (count < 5)
                {
                    message += wxT("✓ ") + desc + wxT("\n");
                    count++;
                }
            }
            if (importedDescriptions.size() > 5)
            {
                message += wxString::Format(wxT("... e mais %d\n"),
                                          (int)(importedDescriptions.size() - 5));
            }
        }

        if (totalSkipped > 0)
        {
            message += wxT("\nJá existentes:\n");
            int count = 0;
            for (const wxString& desc : skippedDescriptions)
            {
                if (count < 5)
                {
                    message += wxT("• ") + desc + wxT("\n");
                    count++;
                }
            }
            if (skippedDescriptions.size() > 5)
            {
                message += wxString::Format(wxT("... e mais %d"),
                                          (int)(skippedDescriptions.size() - 5));
            }
        }
    }

    wxMessageBox(message,
                totalImported > 0 ? wxT("Sucesso") : wxT("Aviso"),
                totalImported > 0 ? wxOK | wxICON_INFORMATION : wxOK | wxICON_WARNING);

    return totalImported > 0;
}

Expense ExpenseDAO::RowToExpense(const wxArrayString& row)
{
    Expense expense;

    if (row.GetCount() >= 9)
    {
        expense.dueDate = DateUtils::ParseDateDB(row[0]);
        expense.description = row[1];
        expense.amount = MoneyUtils::ParseMoney(row[2]);
        expense.expenseType = row[3];
        expense.paymentMethod = row[4];
        expense.paymentCategory = row[5];

        if (!row[6].IsEmpty())
        {
            expense.paymentDate = DateUtils::ParseDateDB(row[6]);
        }

        expense.status = StringToExpenseStatus(row[7]);

        if (!row[8].IsEmpty())
        {
            expense.adjustedAmount = MoneyUtils::ParseMoney(row[8]);
        }
    }

    return expense;
}

std::vector<wxString> ExpenseDAO::GetExistingDescriptionsForMonth(int month, int year)
{
    std::vector<wxString> descriptions;
    Database& db = Database::GetInstance();

    wxDateTime startDate = DateUtils::GetFirstDayOfMonth(month, year);
    wxDateTime endDate = DateUtils::GetLastDayOfMonth(month, year);

    wxString sql = wxString::Format(
        wxT("SELECT DISTINCT description FROM expenses ")
        wxT("WHERE due_date >= '%s' AND due_date <= '%s' ")
        wxT("ORDER BY description"),
        DateUtils::FormatDateDB(startDate),
        DateUtils::FormatDateDB(endDate)
    );

    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            descriptions.push_back(row[0]);
        }
    }

    return descriptions;
}
