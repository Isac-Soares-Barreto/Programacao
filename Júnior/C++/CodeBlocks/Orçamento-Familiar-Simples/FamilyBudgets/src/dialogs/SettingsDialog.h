#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <wx/wx.h>

class SettingsDialog : public wxDialog
{
public:
    SettingsDialog(wxWindow* parent);
    virtual ~SettingsDialog();
    
private:
    void CreateInterface();
};

#endif // SETTINGSDIALOG_H