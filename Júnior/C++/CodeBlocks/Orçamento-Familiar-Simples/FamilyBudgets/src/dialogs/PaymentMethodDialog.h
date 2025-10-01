#ifndef PAYMENTMETHODDIALOG_H
#define PAYMENTMETHODDIALOG_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include "../models/PaymentMethod.h"
#include "../models/PaymentCategory.h"

class PaymentMethodDialog : public wxDialog
{
public:
    // Construtor para nova forma de pagamento
    PaymentMethodDialog(wxWindow* parent);

    // Construtor para editar forma de pagamento existente
    PaymentMethodDialog(wxWindow* parent, const PaymentMethod& method);

    virtual ~PaymentMethodDialog();

    // Obter forma de pagamento preenchida
    PaymentMethod GetPaymentMethod() const;

private:
    void CreateInterface();
    void LoadData();
    void LoadCategories();

    // Event handlers
    void OnOK(wxCommandEvent& event);
    void OnAddCategory(wxCommandEvent& event);
    void OnEditCategory(wxCommandEvent& event);
    void OnDeleteCategory(wxCommandEvent& event);
    void OnCategoryItemActivated(wxListEvent& event);

    // Controles
    wxTextCtrl* m_nameText;
    wxTextCtrl* m_descriptionText;
    wxListCtrl* m_categoriesListView;
    wxButton* m_addCategoryButton;
    wxButton* m_editCategoryButton;
    wxButton* m_deleteCategoryButton;
    wxStaticBoxSizer* m_categoriesBox;

    // Dados
    PaymentMethod m_method;
    bool m_isEdit;
    std::vector<PaymentCategory> m_categories;

    // IDs
    enum
    {
        ID_CategoriesList = wxID_HIGHEST + 500,
        ID_AddCategory,
        ID_EditCategory,
        ID_DeleteCategory
    };
};

#endif // PAYMENTMETHODDIALOG_H
