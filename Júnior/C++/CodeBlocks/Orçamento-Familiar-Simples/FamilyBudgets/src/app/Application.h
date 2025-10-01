#ifndef APPLICATION_H
#define APPLICATION_H

#include <wx/wx.h>

// Classe principal da aplicação
class Application : public wxApp
{
public:
    Application();
    virtual ~Application();

    // Inicialização da aplicação
    virtual bool OnInit() override;
    
    // Finalização da aplicação
    virtual int OnExit() override;

private:
    // Configurar locale para português brasileiro
    void SetupLocale();
    
    // Inicializar banco de dados
    bool InitializeDatabase();
    
    // Verificar e copiar recursos se necessário
    void SetupResources();
    
    wxLocale* m_locale;
};

#endif // APPLICATION_H