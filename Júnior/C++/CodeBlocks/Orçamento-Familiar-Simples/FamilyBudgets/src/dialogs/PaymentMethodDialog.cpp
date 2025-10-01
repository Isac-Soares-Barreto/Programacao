#include "PaymentMethodDialog.h"
#include "../databases/PaymentMethodDAO.h"
#include "../databases/PaymentCategoryDAO.h"
#include "../utils/ButtonUtils.h"
#include <wx/statline.h>

PaymentMethodDialog::PaymentMethodDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxT("Nova Forma de Pagamento"),
               wxDefaultPosition, wxSize(650, 650))
    , m_nameText(nullptr)
    , m_descriptionText(nullptr)
    , m_categoriesListView(nullptr)
    , m_addCategoryButton(nullptr)
    , m_editCategoryButton(nullptr)
    , m_deleteCategoryButton(nullptr)
    , m_categoriesBox(nullptr)
    , m_method()
    , m_isEdit(false)
{
    CreateInterface();
    Centre();
}

PaymentMethodDialog::PaymentMethodDialog(wxWindow* parent, const PaymentMethod& method)
    : wxDialog(parent, wxID_ANY, wxT("Editar Forma de Pagamento"),
               wxDefaultPosition, wxSize(650, 650))
    , m_nameText(nullptr)
    , m_descriptionText(nullptr)
    , m_categoriesListView(nullptr)
    , m_addCategoryButton(nullptr)
    , m_editCategoryButton(nullptr)
    , m_deleteCategoryButton(nullptr)
    , m_categoriesBox(nullptr)
    , m_method(method)
    , m_isEdit(true)
{
    CreateInterface();
    LoadData();
    Centre();
}

PaymentMethodDialog::~PaymentMethodDialog()
{
}

void PaymentMethodDialog::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Criar ScrolledWindow para o conteúdo principal
    wxScrolledWindow* scrolledWindow = new wxScrolledWindow(this, wxID_ANY,
                                                             wxDefaultPosition, wxDefaultSize,
                                                             wxVSCROLL);
    scrolledWindow->SetScrollRate(0, 10); // Rolagem vertical de 10 pixels por vez

    wxBoxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);

    // Grid para os campos
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 10);
    gridSizer->AddGrowableCol(1);

    // Nome da Forma de Pagamento
    wxStaticText* nameLabel = new wxStaticText(scrolledWindow, wxID_ANY, wxT("Nome:"));
    m_nameText = new wxTextCtrl(scrolledWindow, wxID_ANY);
    m_nameText->SetMaxLength(100);

    if (m_isEdit && m_method.IsDefault())
    {
        // Formas de pagamento padrão não podem ter o nome alterado
        m_nameText->Enable(false);
    }

    gridSizer->Add(nameLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    gridSizer->Add(m_nameText, 1, wxEXPAND);

    // Descrição/Observações
    wxStaticText* descLabel = new wxStaticText(scrolledWindow, wxID_ANY, wxT("Descrição/Observações:"));
    m_descriptionText = new wxTextCtrl(scrolledWindow, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxSize(-1, 60),
                                       wxTE_MULTILINE);
    m_descriptionText->SetMaxLength(500);
    gridSizer->Add(descLabel, 0, wxALIGN_TOP | wxRIGHT, 5);
    gridSizer->Add(m_descriptionText, 1, wxEXPAND);

    scrollSizer->Add(gridSizer, 0, wxALL | wxEXPAND, 10);

    // Linha separadora
    scrollSizer->Add(new wxStaticLine(scrolledWindow), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Seção de categorias (só para formas customizadas)
    m_categoriesBox = new wxStaticBoxSizer(wxVERTICAL, scrolledWindow, wxT("Categorias desta Forma de Pagamento"));

    wxStaticText* catInfo = new wxStaticText(scrolledWindow, wxID_ANY,
        wxT("Adicione categorias específicas para esta forma de pagamento.\n")
        wxT("Exemplo: Para 'Vale-Alimentação' você pode ter 'Vale Empresa', 'Vale Pessoal', etc."));
    wxFont catInfoFont = catInfo->GetFont();
    catInfoFont.SetPointSize(catInfoFont.GetPointSize() - 1);
    catInfo->SetFont(catInfoFont);
    catInfo->SetForegroundColour(wxColour(100, 100, 100));

    m_categoriesBox->Add(catInfo, 0, wxALL | wxEXPAND, 5);

    // Barra de ferramentas das categorias
    wxBoxSizer* catToolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    m_addCategoryButton = ButtonUtils::CreateButton(scrolledWindow, wxT("Adicionar"), wxT("add.png"));
    m_addCategoryButton->SetId(ID_AddCategory);
    m_editCategoryButton = ButtonUtils::CreateButton(scrolledWindow, wxT("Editar"), wxT("edit.png"));
    m_editCategoryButton->SetId(ID_EditCategory);
    m_deleteCategoryButton = ButtonUtils::CreateButton(scrolledWindow, wxT("Excluir"), wxT("delete.png"));
    m_deleteCategoryButton->SetId(ID_DeleteCategory);

    catToolbarSizer->Add(m_addCategoryButton, 0, wxALL, 3);
    catToolbarSizer->Add(m_editCategoryButton, 0, wxALL, 3);
    catToolbarSizer->Add(m_deleteCategoryButton, 0, wxALL, 3);

    m_categoriesBox->Add(catToolbarSizer, 0, wxALL, 5);

    // Lista de categorias
    m_categoriesListView = new wxListCtrl(scrolledWindow, ID_CategoriesList, wxDefaultPosition, wxSize(-1, 150),
                                          wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    m_categoriesListView->AppendColumn(wxT("Categoria"), wxLIST_FORMAT_LEFT, 200);
    m_categoriesListView->AppendColumn(wxT("Descrição"), wxLIST_FORMAT_LEFT, 350);

    m_categoriesBox->Add(m_categoriesListView, 1, wxALL | wxEXPAND, 5);

    scrollSizer->Add(m_categoriesBox, 0, wxALL | wxEXPAND, 10);

    // Ocultar seção de categorias se for forma padrão
    if (m_isEdit && m_method.IsDefault())
    {
        m_categoriesBox->GetStaticBox()->Hide();
        m_categoriesListView->Hide();
        m_addCategoryButton->Hide();
        m_editCategoryButton->Hide();
        m_deleteCategoryButton->Hide();
        catInfo->Hide();
    }

    // Linha separadora
    scrollSizer->Add(new wxStaticLine(scrolledWindow), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Informações sobre formas de pagamento
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL, scrolledWindow, wxT("Informações"));

    // Sempre mostrar informações gerais primeiro
    wxString infoTextContent =
        wxT("Formas de pagamento são utilizadas para categorizar suas despesas.\n\n")
        wxT("Formas de pagamento padrão do sistema:\n")
        wxT("• Dinheiro: Pagamento em espécie\n")
        wxT("• PIX: Transferência instantânea\n")
        wxT("• Boleto: Boleto bancário\n")
        wxT("• Cartão: Cartão de crédito (requer seleção do cartão)\n")
        wxT("• Transferência: Transferência bancária\n\n");

    // Se for edição de forma padrão, adicionar aviso
    if (m_isEdit && m_method.IsDefault())
    {
        infoTextContent +=
            wxT("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n")
            wxT("⚠ ATENÇÃO: Você está editando uma forma de pagamento PADRÃO\n\n")
            wxT("Restrições:\n")
            wxT("• O nome não pode ser alterado\n")
            wxT("• A forma não pode ser excluída\n")
            wxT("• Não possui categorias customizadas\n")
            wxT("• Apenas a descrição pode ser editada");
    }
    else if (!m_isEdit)
    {
        // Texto adicional para nova forma
        infoTextContent +=
            wxT("Você pode adicionar formas de pagamento personalizadas conforme\n")
            wxT("sua necessidade (ex: Vale-alimentação, Cheque, etc).\n\n")
            wxT("Após salvar, você poderá adicionar categorias específicas para\n")
            wxT("esta forma de pagamento personalizada.");
    }

    wxStaticText* infoText = new wxStaticText(scrolledWindow, wxID_ANY, infoTextContent);

    wxFont infoFont = infoText->GetFont();
    infoFont.SetPointSize(infoFont.GetPointSize() - 1);
    infoText->SetFont(infoFont);
    infoText->SetForegroundColour(wxColour(60, 60, 60));

    infoBox->Add(infoText, 1, wxALL | wxEXPAND, 10);

    scrollSizer->Add(infoBox, 0, wxALL | wxEXPAND, 10);

    // Aplicar o sizer ao scrolled window
    scrolledWindow->SetSizer(scrollSizer);
    scrolledWindow->FitInside(); // Ajustar o tamanho virtual do scrolled window

    // Adicionar scrolled window ao sizer principal
    mainSizer->Add(scrolledWindow, 1, wxEXPAND);

    // Linha separadora antes dos botões
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

    // Botões (FORA do scrolled window para ficarem sempre visíveis)
    wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer();
    wxButton* okButton = new wxButton(this, wxID_OK, wxT("Salvar"));
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancelar"));
    buttonSizer->AddButton(okButton);
    buttonSizer->AddButton(cancelButton);
    buttonSizer->Realize();

    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 10);

    SetSizer(mainSizer);

    // Bind eventos
    Bind(wxEVT_BUTTON, &PaymentMethodDialog::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &PaymentMethodDialog::OnAddCategory, this, ID_AddCategory);
    Bind(wxEVT_BUTTON, &PaymentMethodDialog::OnEditCategory, this, ID_EditCategory);
    Bind(wxEVT_BUTTON, &PaymentMethodDialog::OnDeleteCategory, this, ID_DeleteCategory);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &PaymentMethodDialog::OnCategoryItemActivated, this, ID_CategoriesList);

    okButton->SetDefault();
}

void PaymentMethodDialog::LoadData()
{
    // Preencher campos com dados da forma de pagamento
    m_nameText->SetValue(m_method.name);
    m_descriptionText->SetValue(m_method.description);

    // Carregar categorias se for forma customizada
    if (!m_method.IsDefault())
    {
        LoadCategories();
    }
}

void PaymentMethodDialog::LoadCategories()
{
    if (!m_categoriesListView) return;

    m_categoriesListView->DeleteAllItems();
    m_categories.clear();

    if (m_isEdit && !m_method.name.IsEmpty())
    {
        PaymentCategoryDAO dao;
        m_categories = dao.GetByPaymentMethod(m_method.name);

        for (size_t i = 0; i < m_categories.size(); i++)
        {
            const PaymentCategory& cat = m_categories[i];

            long itemIndex = m_categoriesListView->InsertItem(i, cat.categoryName);
            m_categoriesListView->SetItem(itemIndex, 1, cat.description);
        }
    }
}

void PaymentMethodDialog::OnAddCategory(wxCommandEvent& event)
{
    // Verificar se é uma nova forma de pagamento que ainda não foi salva
    if (!m_isEdit)
    {
        wxMessageBox(
            wxT("Você precisa primeiro salvar a forma de pagamento\n")
            wxT("antes de adicionar categorias.\n\n")
            wxT("Clique em 'Salvar', depois edite novamente esta forma\n")
            wxT("de pagamento para adicionar categorias."),
            wxT("Aviso"),
            wxOK | wxICON_INFORMATION,
            this
        );
        return;
    }

    // Dialog simples para adicionar categoria
    wxTextEntryDialog catNameDialog(this,
        wxT("Digite o nome da categoria:"),
        wxT("Nova Categoria"),
        wxEmptyString);

    if (catNameDialog.ShowModal() == wxID_OK)
    {
        wxString catName = catNameDialog.GetValue().Trim();

        if (catName.IsEmpty())
        {
            wxMessageBox(wxT("Nome da categoria não pode ser vazio!"),
                        wxT("Aviso"), wxOK | wxICON_WARNING, this);
            return;
        }

        // Verificar se já existe
        PaymentCategoryDAO dao;
        if (dao.Exists(m_method.name, catName))
        {
            wxMessageBox(wxT("Já existe uma categoria com este nome!"),
                        wxT("Aviso"), wxOK | wxICON_WARNING, this);
            return;
        }

        // Pedir descrição
        wxTextEntryDialog descDialog(this,
            wxT("Digite uma descrição (opcional):"),
            wxT("Descrição da Categoria"),
            wxEmptyString);

        wxString description;
        if (descDialog.ShowModal() == wxID_OK)
        {
            description = descDialog.GetValue().Trim();
        }

        // Criar categoria
        PaymentCategory category(m_method.name, catName, description);

        if (dao.Insert(category))
        {
            LoadCategories();
            wxMessageBox(wxT("Categoria adicionada com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao adicionar categoria!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void PaymentMethodDialog::OnEditCategory(wxCommandEvent& event)
{
    long selectedIndex = m_categoriesListView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma categoria para editar!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    PaymentCategory& oldCategory = m_categories[selectedIndex];

    // Dialog para editar nome
    wxTextEntryDialog nameDialog(this,
        wxT("Edite o nome da categoria:"),
        wxT("Editar Categoria"),
        oldCategory.categoryName);

    if (nameDialog.ShowModal() == wxID_OK)
    {
        wxString newName = nameDialog.GetValue().Trim();

        if (newName.IsEmpty())
        {
            wxMessageBox(wxT("Nome da categoria não pode ser vazio!"),
                        wxT("Aviso"), wxOK | wxICON_WARNING, this);
            return;
        }

        // Verificar duplicidade (se o nome mudou)
        if (newName != oldCategory.categoryName)
        {
            PaymentCategoryDAO dao;
            if (dao.Exists(m_method.name, newName))
            {
                wxMessageBox(wxT("Já existe uma categoria com este nome!"),
                            wxT("Aviso"), wxOK | wxICON_WARNING, this);
                return;
            }
        }

        // Dialog para editar descrição
        wxTextEntryDialog descDialog(this,
            wxT("Edite a descrição (opcional):"),
            wxT("Descrição da Categoria"),
            oldCategory.description);

        wxString newDesc;
        if (descDialog.ShowModal() == wxID_OK)
        {
            newDesc = descDialog.GetValue().Trim();
        }

        // Atualizar categoria
        PaymentCategory newCategory(m_method.name, newName, newDesc);
        PaymentCategoryDAO dao;

        if (dao.Update(m_method.name, oldCategory.categoryName, newCategory))
        {
            LoadCategories();
            wxMessageBox(wxT("Categoria atualizada com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao atualizar categoria!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void PaymentMethodDialog::OnDeleteCategory(wxCommandEvent& event)
{
    long selectedIndex = m_categoriesListView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (selectedIndex == -1)
    {
        wxMessageBox(wxT("Selecione uma categoria para excluir!"),
                    wxT("Aviso"), wxOK | wxICON_WARNING, this);
        return;
    }

    PaymentCategory& category = m_categories[selectedIndex];

    wxString message = wxString::Format(
        wxT("Confirma a exclusão da categoria:\n\n%s\n\n")
        wxT("Esta ação não pode ser desfeita!"),
        category.categoryName
    );

    if (wxMessageBox(message, wxT("Confirmar Exclusão"),
                     wxYES_NO | wxICON_QUESTION, this) == wxYES)
    {
        PaymentCategoryDAO dao;

        if (dao.Delete(m_method.name, category.categoryName))
        {
            LoadCategories();
            wxMessageBox(wxT("Categoria excluída com sucesso!"),
                        wxT("Sucesso"), wxOK | wxICON_INFORMATION, this);
        }
        else
        {
            wxMessageBox(wxT("Erro ao excluir categoria!"),
                        wxT("Erro"), wxOK | wxICON_ERROR, this);
        }
    }
}

void PaymentMethodDialog::OnCategoryItemActivated(wxListEvent& event)
{
    wxCommandEvent evt(wxEVT_BUTTON, ID_EditCategory);
    OnEditCategory(evt);
}

void PaymentMethodDialog::OnOK(wxCommandEvent& event)
{
    // Validar campos obrigatórios
    if (m_nameText->GetValue().Trim().IsEmpty())
    {
        wxMessageBox(wxT("Nome da forma de pagamento é obrigatório!"),
                     wxT("Aviso"), wxOK | wxICON_WARNING, this);
        m_nameText->SetFocus();
        return;
    }

    // Verificar se o nome já existe (apenas ao adicionar novo ou alterar nome)
    wxString newName = m_nameText->GetValue().Trim();
    if (!m_isEdit || (m_isEdit && !m_method.IsDefault() && newName != m_method.name))
    {
        PaymentMethodDAO dao;
        if (dao.Exists(newName))
        {
            wxMessageBox(wxT("Já existe uma forma de pagamento com este nome!\n\nEscolha outro nome."),
                        wxT("Aviso"), wxOK | wxICON_WARNING, this);
            m_nameText->SetFocus();
            return;
        }
    }

    event.Skip();
}

PaymentMethod PaymentMethodDialog::GetPaymentMethod() const
{
    PaymentMethod method;

    method.name = m_nameText->GetValue().Trim();
    method.description = m_descriptionText->GetValue().Trim();

    return method;
}
