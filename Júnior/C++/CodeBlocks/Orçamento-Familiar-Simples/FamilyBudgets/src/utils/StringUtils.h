#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <wx/wx.h>
#include <sstream>
#include <iomanip>

class StringUtils
{
public:
    // Formatar double para SQL (com ponto decimal)
    static wxString FormatDoubleForSQL(double value)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value;
        return wxString(ss.str());
    }
    
    // Escapar string para SQL (prevenir SQL injection)
    static wxString EscapeSQL(const wxString& str)
    {
        wxString escaped = str;
        escaped.Replace(wxT("'"), wxT("''"));  // Duplicar aspas simples
        return escaped;
    }
};

#endif // STRINGUTILS_H