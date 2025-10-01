#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <wx/wx.h>
#include <wx/hyperlink.h>

class AboutDialog : public wxDialog
{
public:
    AboutDialog(wxWindow* parent);
    virtual ~AboutDialog();

private:
    void CreateInterface();

    // Event handler para o link
    void OnLinkedInClick(wxHyperlinkEvent& event);
};

#endif // ABOUTDIALOG_H
