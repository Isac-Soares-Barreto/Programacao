#ifndef BUTTONUTILS_H
#define BUTTONUTILS_H

#include <wx/wx.h>
#include <wx/button.h>
#include "PathUtils.h"

// Classe utilitária para criar botoes padronizados
class ButtonUtils
{
public:
    // Criar botao com icone padronizado (16x16)
    static wxButton* CreateButton(wxWindow* parent, const wxString& label, const wxString& iconName)
    {
        wxButton* button = new wxButton(parent, wxID_ANY, label);

        // So tentar carregar o icone se o arquivo existir
        wxString iconPath = PathUtils::GetIconsDir() + iconName;
        if (wxFileExists(iconPath))
        {
            wxImage image;
            if (image.LoadFile(iconPath, wxBITMAP_TYPE_PNG))
            {
                // Redimensionar para 16x16 se necessario
                if (image.GetWidth() != 16 || image.GetHeight() != 16)
                {
                    image.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
                }
                wxBitmap bmp(image);
                if (bmp.IsOk())
                {
                    button->SetBitmap(bmp);
                }
            }
        }

        return button;
    }
};

#endif // BUTTONUTILS_H
