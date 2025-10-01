#include "CardDAO.h"
#include "Database.h"
#include "../utils/StringUtils.h"
#include "../utils/MoneyUtils.h"

CardDAO::CardDAO()
{
}

CardDAO::~CardDAO()
{
}

bool CardDAO::Insert(const Card& card)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("INSERT INTO cards (name, closing_day, due_day, card_limit, description) ")
        wxT("VALUES ('%s', %d, %d, %s, '%s')"),
        StringUtils::EscapeSQL(card.name),
        card.closingDay,
        card.dueDay,
        StringUtils::FormatDoubleForSQL(card.cardLimit),
        StringUtils::EscapeSQL(card.description)
    );
    
    return db.Execute(sql);
}

bool CardDAO::Update(const wxString& oldName, const Card& newCard)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("UPDATE cards SET ")
        wxT("name = '%s', closing_day = %d, due_day = %d, ")
        wxT("card_limit = %s, description = '%s' ")
        wxT("WHERE name = '%s'"),
        StringUtils::EscapeSQL(newCard.name),
        newCard.closingDay,
        newCard.dueDay,
        StringUtils::FormatDoubleForSQL(newCard.cardLimit),
        StringUtils::EscapeSQL(newCard.description),
        StringUtils::EscapeSQL(oldName)
    );
    
    return db.Execute(sql);
}

bool CardDAO::Delete(const wxString& name)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("DELETE FROM cards WHERE name = '%s'"),
        StringUtils::EscapeSQL(name)
    );
    
    return db.Execute(sql);
}

std::vector<Card> CardDAO::GetAll()
{
    std::vector<Card> cards;
    Database& db = Database::GetInstance();
    
    wxString sql = wxT("SELECT name, closing_day, due_day, card_limit, description ")
                   wxT("FROM cards ORDER BY name");
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty())
    {
        for (const auto& row : result->rows)
        {
            cards.push_back(RowToCard(row));
        }
    }
    
    return cards;
}

std::unique_ptr<Card> CardDAO::GetByName(const wxString& name)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT name, closing_day, due_day, card_limit, description ")
        wxT("FROM cards WHERE name = '%s'"),
        StringUtils::EscapeSQL(name)
    );
    
    auto result = db.Query(sql);
    if (result && !result->IsEmpty() && !result->rows.empty())
    {
        return std::make_unique<Card>(RowToCard(result->rows[0]));
    }
    
    return nullptr;
}

bool CardDAO::Exists(const wxString& name)
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxString::Format(
        wxT("SELECT COUNT(*) FROM cards WHERE name = '%s'"),
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

double CardDAO::GetTotalLimits()
{
    Database& db = Database::GetInstance();
    
    wxString sql = wxT("SELECT SUM(card_limit) FROM cards");
    
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

Card CardDAO::RowToCard(const wxArrayString& row)
{
    Card card;
    
    if (row.GetCount() >= 5)
    {
        card.name = row[0];
        card.closingDay = wxAtoi(row[1]);
        card.dueDay = wxAtoi(row[2]);
        card.cardLimit = MoneyUtils::ParseMoney(row[3]);
        card.description = row[4];
    }
    
    return card;
}