#include "PaymentMethodDAO.h"
#include "Database.h"
#include "../utils/StringUtils.h"

PaymentMethodDAO::PaymentMethodDAO()
{
}

PaymentMethodDAO::~PaymentMethodDAO()
{
}

bool PaymentMethodDAO::Insert(const PaymentMethod& method)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("INSERT INTO payment_methods (name, description) ")
        wxT("VALUES ('%s', '%s')"),
        StringUtils::EscapeSQL(method.name),
        StringUtils::EscapeSQL(method.description)
    );
    
    return db.Execute(sql);
}

bool PaymentMethodDAO::Update(const wxString& oldName, const PaymentMethod& newMethod)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("UPDATE payment_methods SET ")
        wxT("name = '%s', description = '%s' ")
        wxT("WHERE name = '%s'"),
        StringUtils::EscapeSQL(newMethod.name),
        StringUtils::EscapeSQL(newMethod.description),
        StringUtils::EscapeSQL(oldName)
    );
    
    return db.Execute(sql);
}

bool PaymentMethodDAO::Delete(const wxString& name)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("DELETE FROM payment_methods WHERE name = '%s'"),
        StringUtils::EscapeSQL(name)
    );
    
    return db.Execute(sql);
}

std::vector<PaymentMethod> PaymentMethodDAO::GetAll()
{
    std::vector<PaymentMethod> methods;
    Database& db = Database::GetInstance();
    
    wxString sql = wxT("SELECT name, description FROM payment_methods ORDER BY name");
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            methods.push_back(RowToPaymentMethod(row));
        }
    }
    
    return methods;
}

std::unique_ptr<PaymentMethod> PaymentMethodDAO::GetByName(const wxString& name)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT name, description FROM payment_methods WHERE name = '%s'"),
        StringUtils::EscapeSQL(name)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<PaymentMethod>(RowToPaymentMethod(result->rows[0]));
    }
    
    return nullptr;
}

bool PaymentMethodDAO::Exists(const wxString& name)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT COUNT(*) FROM payment_methods WHERE name = '%s'"),
        StringUtils::EscapeSQL(name)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        int count = wxAtoi(result->rows[0][0]);
        return count > 0;
    }
    
    return false;
}

int PaymentMethodDAO::Count()
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxT("SELECT COUNT(*) FROM payment_methods");
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return wxAtoi(result->rows[0][0]);
    }
    
    return 0;
}

PaymentMethod PaymentMethodDAO::RowToPaymentMethod(const wxArrayString& row)
{
    PaymentMethod method;
    
    if (row.GetCount() >= 2)
    {
        method.name = row[0];
        method.description = row[1];
    }
    
    return method;
}