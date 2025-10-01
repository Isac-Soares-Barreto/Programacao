#include "PaymentCategoryDAO.h"
#include "Database.h"
#include "../utils/StringUtils.h"

PaymentCategoryDAO::PaymentCategoryDAO()
{
}

PaymentCategoryDAO::~PaymentCategoryDAO()
{
}

bool PaymentCategoryDAO::Insert(const PaymentCategory& category)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("INSERT INTO payment_categories (payment_method_name, category_name, description) ")
        wxT("VALUES ('%s', '%s', '%s')"),
        StringUtils::EscapeSQL(category.paymentMethodName),
        StringUtils::EscapeSQL(category.categoryName),
        StringUtils::EscapeSQL(category.description)
    );
    
    return db.Execute(sql);
}

bool PaymentCategoryDAO::Update(const wxString& paymentMethodName, const wxString& oldCategoryName, const PaymentCategory& newCategory)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("UPDATE payment_categories SET ")
        wxT("category_name = '%s', description = '%s' ")
        wxT("WHERE payment_method_name = '%s' AND category_name = '%s'"),
        StringUtils::EscapeSQL(newCategory.categoryName),
        StringUtils::EscapeSQL(newCategory.description),
        StringUtils::EscapeSQL(paymentMethodName),
        StringUtils::EscapeSQL(oldCategoryName)
    );
    
    return db.Execute(sql);
}

bool PaymentCategoryDAO::Delete(const wxString& paymentMethodName, const wxString& categoryName)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("DELETE FROM payment_categories WHERE payment_method_name = '%s' AND category_name = '%s'"),
        StringUtils::EscapeSQL(paymentMethodName),
        StringUtils::EscapeSQL(categoryName)
    );
    
    return db.Execute(sql);
}

bool PaymentCategoryDAO::DeleteAllByPaymentMethod(const wxString& paymentMethodName)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("DELETE FROM payment_categories WHERE payment_method_name = '%s'"),
        StringUtils::EscapeSQL(paymentMethodName)
    );
    
    return db.Execute(sql);
}

std::vector<PaymentCategory> PaymentCategoryDAO::GetByPaymentMethod(const wxString& paymentMethodName)
{
    std::vector<PaymentCategory> categories;
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT payment_method_name, category_name, description ")
        wxT("FROM payment_categories WHERE payment_method_name = '%s' ORDER BY category_name"),
        StringUtils::EscapeSQL(paymentMethodName)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            categories.push_back(RowToPaymentCategory(row));
        }
    }
    
    return categories;
}

std::unique_ptr<PaymentCategory> PaymentCategoryDAO::GetByKey(const wxString& paymentMethodName, const wxString& categoryName)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT payment_method_name, category_name, description ")
        wxT("FROM payment_categories WHERE payment_method_name = '%s' AND category_name = '%s'"),
        StringUtils::EscapeSQL(paymentMethodName),
        StringUtils::EscapeSQL(categoryName)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<PaymentCategory>(RowToPaymentCategory(result->rows[0]));
    }
    
    return nullptr;
}

bool PaymentCategoryDAO::Exists(const wxString& paymentMethodName, const wxString& categoryName)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT COUNT(*) FROM payment_categories ")
        wxT("WHERE payment_method_name = '%s' AND category_name = '%s'"),
        StringUtils::EscapeSQL(paymentMethodName),
        StringUtils::EscapeSQL(categoryName)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        int count = wxAtoi(result->rows[0][0]);
        return count > 0;
    }
    
    return false;
}

int PaymentCategoryDAO::CountByPaymentMethod(const wxString& paymentMethodName)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT COUNT(*) FROM payment_categories WHERE payment_method_name = '%s'"),
        StringUtils::EscapeSQL(paymentMethodName)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return wxAtoi(result->rows[0][0]);
    }
    
    return 0;
}

PaymentCategory PaymentCategoryDAO::RowToPaymentCategory(const wxArrayString& row)
{
    PaymentCategory category;
    
    if (row.GetCount() >= 3)
    {
        category.paymentMethodName = row[0];
        category.categoryName = row[1];
        category.description = row[2];
    }
    
    return category;
}