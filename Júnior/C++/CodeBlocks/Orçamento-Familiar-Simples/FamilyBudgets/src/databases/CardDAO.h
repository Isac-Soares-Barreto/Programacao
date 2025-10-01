#ifndef CARDDAO_H
#define CARDDAO_H

#include <wx/wx.h>
#include <vector>
#include <memory>
#include "../models/Card.h"

// Classe para acesso a dados de cartões
class CardDAO
{
public:
    CardDAO();
    ~CardDAO();

    // Operações CRUD
    bool Insert(const Card& card);
    bool Update(const wxString& oldName, const Card& newCard);
    bool Delete(const wxString& name);

    // Consultas
    std::vector<Card> GetAll();
    std::unique_ptr<Card> GetByName(const wxString& name);

    // Verificar se cartão existe
    bool Exists(const wxString& name);

    // Obter total de limites de todos os cartões
    double GetTotalLimits();

private:
    // Converter linha do banco para objeto Card
    Card RowToCard(const wxArrayString& row);
};

#endif // CARDDAO_H
