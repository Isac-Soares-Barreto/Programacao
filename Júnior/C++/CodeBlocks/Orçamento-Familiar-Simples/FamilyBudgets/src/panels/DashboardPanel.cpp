#include "DashboardPanel.h"
#include "../utils/PathUtils.h"
#include "../utils/DateUtils.h"
#include "../utils/MoneyUtils.h"
#include "../utils/ChartUtils.h"
#include "../databases/ExpenseDAO.h"
#include "../databases/IncomeDAO.h"
#include "../databases/InvestmentDAO.h"
#include "../databases/Database.h"
#include <wx/dcbuffer.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <algorithm>
#include <map>

// ============================================================================
// CLASSE BASE PARA PAINÉIS DE GRÁFICOS (COM PROTEÇÃO)
// ============================================================================

class ChartPanel : public wxPanel
{
public:
    ChartPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &ChartPanel::OnPaint, this);
        SetMinSize(wxSize(300, 200)); // Tamanho mínimo seguro
    }

    virtual ~ChartPanel() {}

protected:
    virtual void DrawChart(wxDC& dc) = 0;

private:
    void OnPaint(wxPaintEvent& event)
    {
        try
        {
            wxBufferedPaintDC dc(this);

            // Limpar o fundo primeiro
            dc.SetBackground(wxBrush(*wxWHITE));
            dc.Clear();

            // Verificar se o painel tem tamanho válido
            wxSize size = GetClientSize();
            if (size.GetWidth() > 10 && size.GetHeight() > 10)
            {
                DrawChart(dc);
            }
        }
        catch (const std::exception& e)
        {
            wxLogError(wxT("Erro ao desenhar gráfico: %s"), e.what());
        }
        catch (...)
        {
            wxLogError(wxT("Erro desconhecido ao desenhar gráfico"));
        }
    }
};

// ============================================================================
// PAINEL DE CARD INFORMATIVO
// ============================================================================

class InfoCard : public wxPanel
{
public:
    InfoCard(wxWindow* parent, const wxString& title, const wxString& value,
             const wxColour& color = wxColour(52, 152, 219))
        : wxPanel(parent, wxID_ANY)
        , m_title(title)
        , m_value(value)
        , m_color(color)
    {
        SetBackgroundColour(color);
        SetMinSize(wxSize(150, 80));

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        m_titleText = new wxStaticText(this, wxID_ANY, m_title);
        wxFont titleFont = m_titleText->GetFont();
        titleFont.SetPointSize(9);
        titleFont.SetWeight(wxFONTWEIGHT_NORMAL);
        m_titleText->SetFont(titleFont);
        m_titleText->SetForegroundColour(*wxWHITE);

        m_valueText = new wxStaticText(this, wxID_ANY, m_value);
        wxFont valueFont = m_valueText->GetFont();
        valueFont.SetPointSize(14);
        valueFont.SetWeight(wxFONTWEIGHT_BOLD);
        m_valueText->SetFont(valueFont);
        m_valueText->SetForegroundColour(*wxWHITE);

        sizer->AddStretchSpacer();
        sizer->Add(m_titleText, 0, wxALL | wxALIGN_CENTER, 5);
        sizer->Add(m_valueText, 0, wxALL | wxALIGN_CENTER, 5);
        sizer->AddStretchSpacer();

        SetSizer(sizer);
    }

    void UpdateValue(const wxString& value)
    {
        if (m_valueText && !value.IsEmpty())
        {
            m_valueText->SetLabel(value);
            Layout();
            Refresh();
        }
    }

private:
    wxString m_title;
    wxString m_value;
    wxColour m_color;
    wxStaticText* m_titleText;
    wxStaticText* m_valueText;
};

// ============================================================================
// GRÁFICOS DE LINHA PARA RESUMO GERAL
// ============================================================================

class MonthlyLineChart : public ChartPanel
{
public:
    MonthlyLineChart(wxWindow* parent, const wxString& title, const wxColour& color, int year)
        : ChartPanel(parent)
        , m_title(title)
        , m_color(color)
        , m_year(year)
    {
        SetMinSize(wxSize(400, 250));
    }

    void SetYear(int year)
    {
        m_year = year;
        LoadData();
        Refresh();
    }

    void SetData(const std::vector<double>& values)
    {
        m_values = values;
        // Garantir que sempre temos 12 valores
        m_values.resize(12, 0.0);
        Refresh();
    }

protected:
    virtual void LoadData() = 0;

    void DrawChart(wxDC& dc) override
    {
        try
        {
            wxRect rect = GetClientRect();

            // Validar dados
            if (m_values.empty())
            {
                m_values.resize(12, 0.0);
            }

            std::vector<LineChartData> series;
            LineChartData data;
            data.label = m_title;
            data.color = m_color;
            data.values = m_values;
            series.push_back(data);

            wxArrayString labels;
            labels.Add(wxT("Jan")); labels.Add(wxT("Fev")); labels.Add(wxT("Mar"));
            labels.Add(wxT("Abr")); labels.Add(wxT("Mai")); labels.Add(wxT("Jun"));
            labels.Add(wxT("Jul")); labels.Add(wxT("Ago")); labels.Add(wxT("Set"));
            labels.Add(wxT("Out")); labels.Add(wxT("Nov")); labels.Add(wxT("Dez"));

            ChartUtils::DrawLineChart(dc, rect, series, labels, m_title);
        }
        catch (...)
        {
            // Em caso de erro, apenas não desenha
        }
    }

protected:
    wxString m_title;
    wxColour m_color;
    int m_year;
    std::vector<double> m_values;
};

// Implementações específicas para cada tipo
class BalanceLineChart : public MonthlyLineChart
{
public:
    BalanceLineChart(wxWindow* parent, int year)
        : MonthlyLineChart(parent, wxT("Saldo Mensal"), wxColour(52, 152, 219), year)
    {
        LoadData();
    }

protected:
    void LoadData() override
    {
        m_values.clear();
        m_values.resize(12, 0.0);

        try
        {
            IncomeDAO incomeDAO;
            ExpenseDAO expenseDAO;

            for (int month = 1; month <= 12; month++)
            {
                double income = incomeDAO.GetTotalByMonth(month, m_year);
                double expense = 0;

                auto expenses = expenseDAO.GetByMonth(month, m_year);
                for (const auto& exp : expenses)
                {
                    double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                    expense += value;
                }

                m_values[month - 1] = income - expense;
            }
        }
        catch (...) {}
    }
};

class IncomeLineChart : public MonthlyLineChart
{
public:
    IncomeLineChart(wxWindow* parent, int year)
        : MonthlyLineChart(parent, wxT("Receita Mensal"), wxColour(46, 204, 113), year)
    {
        LoadData();
    }

protected:
    void LoadData() override
    {
        m_values.clear();
        m_values.resize(12, 0.0);

        try
        {
            IncomeDAO dao;
            for (int month = 1; month <= 12; month++)
            {
                m_values[month - 1] = dao.GetTotalByMonth(month, m_year);
            }
        }
        catch (...) {}
    }
};

class ExpenseLineChart : public MonthlyLineChart
{
public:
    ExpenseLineChart(wxWindow* parent, int year)
        : MonthlyLineChart(parent, wxT("Despesa Mensal"), wxColour(231, 76, 60), year)
    {
        LoadData();
    }

protected:
    void LoadData() override
    {
        m_values.clear();
        m_values.resize(12, 0.0);

        try
        {
            ExpenseDAO dao;
            for (int month = 1; month <= 12; month++)
            {
                double total = 0;
                auto expenses = dao.GetByMonth(month, m_year);

                for (const auto& exp : expenses)
                {
                    double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                    total += value;
                }

                m_values[month - 1] = total;
            }
        }
        catch (...) {}
    }
};

class InvestmentLineChart : public MonthlyLineChart
{
public:
    InvestmentLineChart(wxWindow* parent, int year)
        : MonthlyLineChart(parent, wxT("Investimento Mensal"), wxColour(155, 89, 182), year)
    {
        LoadData();
    }

protected:
    void LoadData() override
    {
        m_values.clear();
        m_values.resize(12, 0.0);

        try
        {
            InvestmentDAO dao;
            for (int month = 1; month <= 12; month++)
            {
                m_values[month - 1] = dao.GetTotalByMonth(month, m_year);
            }
        }
        catch (...) {}
    }
};

// ============================================================================
// GRÁFICOS DE PIZZA E BARRA GENÉRICOS
// ============================================================================

class PieChartPanel : public ChartPanel
{
public:
    PieChartPanel(wxWindow* parent, const wxString& title)
        : ChartPanel(parent)
        , m_title(title)
    {
        SetMinSize(wxSize(400, 300));
    }

    void SetData(const std::vector<PieChartData>& data)
    {
        m_data = data;
        Refresh();
    }

protected:
    void DrawChart(wxDC& dc) override
    {
        try
        {
            wxRect rect = GetClientRect();

            if (!m_data.empty())
            {
                ChartUtils::DrawPieChart(dc, rect, m_data, m_title);
            }
            else
            {
                // Desenhar mensagem de "sem dados"
                dc.SetTextForeground(wxColour(128, 128, 128));
                dc.DrawText(wxT("Sem dados para exibir"), rect.width/2 - 60, rect.height/2);
            }
        }
        catch (...)
        {
            // Em caso de erro, não desenha
        }
    }

private:
    wxString m_title;
    std::vector<PieChartData> m_data;
};

class BarChartPanel : public ChartPanel
{
public:
    BarChartPanel(wxWindow* parent, const wxString& title)
        : ChartPanel(parent)
        , m_title(title)
    {
        SetMinSize(wxSize(400, 300));
    }

    void SetData(const std::vector<BarChartData>& data)
    {
        m_data = data;
        Refresh();
    }

protected:
    void DrawChart(wxDC& dc) override
    {
        try
        {
            wxRect rect = GetClientRect();

            if (!m_data.empty())
            {
                ChartUtils::DrawBarChart(dc, rect, m_data, m_title);
            }
            else
            {
                // Desenhar mensagem de "sem dados"
                dc.SetTextForeground(wxColour(128, 128, 128));
                dc.DrawText(wxT("Sem dados para exibir"), rect.width/2 - 60, rect.height/2);
            }
        }
        catch (...)
        {
            // Em caso de erro, não desenha
        }
    }

private:
    wxString m_title;
    std::vector<BarChartData> m_data;
};

// ============================================================================
// PAINÉIS DE ALERTAS E INDICADORES
// ============================================================================

class AlertsPanel : public wxPanel
{
public:
    AlertsPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY)
    {
        SetBackgroundColour(wxColour(255, 243, 205));
        CreateInterface();
        LoadAlerts();
    }

    void LoadAlerts()
    {
        if (!m_alertsList) return;

        m_alertsList->Clear();

        try
        {
            ExpenseDAO dao;
            wxDateTime today = wxDateTime::Today();
            wxDateTime next7Days = today;
            next7Days.Add(wxDateSpan::Days(7));

            int countExpiring = 0, countLate = 0, countInDebt = 0;
            auto expenses = dao.GetAll();

            for (const auto& exp : expenses)
            {
                if (exp.status == ExpenseStatus::TO_PAY &&
                    exp.dueDate >= today && exp.dueDate <= next7Days)
                {
                    countExpiring++;
                    if (m_alertsList->GetCount() < 5) // Limitar para não poluir
                    {
                        m_alertsList->Append(wxString::Format(
                            wxT("⏰ Vence em %d dias: %s - %s"),
                            (exp.dueDate - today).GetDays(),
                            exp.description,
                            MoneyUtils::FormatMoney(exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount)));
                    }
                }
                else if (exp.status == ExpenseStatus::LATE)
                {
                    countLate++;
                    if (m_alertsList->GetCount() < 10) // Limitar quantidade
                    {
                        m_alertsList->Append(wxString::Format(
                            wxT("⚠️ Atrasada: %s - %s"),
                            exp.description,
                            MoneyUtils::FormatMoney(exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount)));
                    }
                }
                else if (exp.status == ExpenseStatus::IN_DEBT)
                {
                    countInDebt++;
                    if (m_alertsList->GetCount() < 10) // Limitar quantidade
                    {
                        m_alertsList->Append(wxString::Format(
                            wxT("🔴 Em dívida: %s - %s"),
                            exp.description,
                            MoneyUtils::FormatMoney(exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount)));
                    }
                }
            }

            if (m_summaryText)
            {
                m_summaryText->SetLabel(wxString::Format(
                    wxT("Total: %d alertas | Vencendo: %d | Atrasadas: %d | Em dívida: %d"),
                    countExpiring + countLate + countInDebt, countExpiring, countLate, countInDebt));
            }

            if (m_alertsList->GetCount() == 0)
            {
                m_alertsList->Append(wxT("✅ Nenhum alerta no momento!"));
            }
        }
        catch (...)
        {
            m_alertsList->Append(wxT("❌ Erro ao carregar alertas"));
        }
    }

private:
    void CreateInterface()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("⚠️ Alertas Financeiros"));
        wxFont titleFont = title->GetFont();
        titleFont.SetPointSize(10);
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);
        mainSizer->Add(title, 0, wxALL, 5);

        m_alertsList = new wxListBox(this, wxID_ANY);
        m_alertsList->SetMinSize(wxSize(-1, 100));
        mainSizer->Add(m_alertsList, 1, wxALL | wxEXPAND, 5);

        m_summaryText = new wxStaticText(this, wxID_ANY, wxT("Carregando..."));
        wxFont summaryFont = m_summaryText->GetFont();
        summaryFont.SetPointSize(8);
        m_summaryText->SetFont(summaryFont);
        mainSizer->Add(m_summaryText, 0, wxALL, 5);

        SetSizer(mainSizer);
    }

    wxListBox* m_alertsList;
    wxStaticText* m_summaryText;
};

class IndicatorsPanel : public wxPanel
{
public:
    IndicatorsPanel(wxWindow* parent, int year = wxDateTime::Now().GetYear())
        : wxPanel(parent, wxID_ANY)
        , m_selectedMonth(0) // 0 = Todos os meses
        , m_selectedYear(year)
    {
        SetBackgroundColour(wxColour(232, 245, 233));
        CreateInterface();
        LoadIndicators();
    }

    void SetYear(int year)
    {
        m_selectedYear = year;
        LoadIndicators();
    }

    void LoadIndicators()
    {
        try
        {
            ExpenseDAO expenseDAO;
            IncomeDAO incomeDAO;

            std::vector<Expense> expenses;
            double totalIncome = 0;

            // Determinar período baseado no filtro
            if (m_selectedMonth == 0)
            {
                // Todos os meses do ano
                for (int month = 1; month <= 12; month++)
                {
                    auto monthExpenses = expenseDAO.GetByMonth(month, m_selectedYear);
                    expenses.insert(expenses.end(), monthExpenses.begin(), monthExpenses.end());
                    totalIncome += incomeDAO.GetTotalByMonth(month, m_selectedYear);
                }
            }
            else
            {
                // Mês específico
                expenses = expenseDAO.GetByMonth(m_selectedMonth, m_selectedYear);
                totalIncome = incomeDAO.GetTotalByMonth(m_selectedMonth, m_selectedYear);
            }

            int totalExpenses = expenses.size();
            int paidExpenses = 0;
            double totalExpenseAmount = 0;

            for (const auto& exp : expenses)
            {
                if (exp.status == ExpenseStatus::PAID) paidExpenses++;
                double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                totalExpenseAmount += value;
            }

            // Percentual de despesas pagas
            double percentPaid = totalExpenses > 0 ? (paidExpenses * 100.0 / totalExpenses) : 0.0;

            if (m_percentPaidText)
            {
                m_percentPaidText->SetLabel(wxString::Format(
                    wxT("%.1f%% (%d de %d)"), percentPaid, paidExpenses, totalExpenses));

                // Colorir baseado no percentual
                if (percentPaid >= 80)
                    m_percentPaidText->SetForegroundColour(wxColour(39, 174, 96)); // Verde
                else if (percentPaid >= 60)
                    m_percentPaidText->SetForegroundColour(wxColour(241, 196, 15)); // Amarelo
                else
                    m_percentPaidText->SetForegroundColour(wxColour(192, 57, 43)); // Vermelho
            }

            // Taxa de comprometimento da renda
            double commitmentRate = totalIncome > 0 ? (totalExpenseAmount * 100.0 / totalIncome) : 0.0;

            if (m_commitmentRateText)
            {
                m_commitmentRateText->SetLabel(wxString::Format(wxT("%.1f%%"), commitmentRate));

                if (commitmentRate > 80)
                    m_commitmentRateText->SetForegroundColour(wxColour(192, 57, 43)); // Vermelho
                else if (commitmentRate > 60)
                    m_commitmentRateText->SetForegroundColour(wxColour(241, 196, 15)); // Amarelo
                else
                    m_commitmentRateText->SetForegroundColour(wxColour(39, 174, 96)); // Verde
            }

            // Evolução patrimonial (comparação com período anterior)
            double prevIncome = 0;
            double prevExpense = 0;

            if (m_selectedMonth == 0)
            {
                // Comparar com ano anterior
                for (int month = 1; month <= 12; month++)
                {
                    prevIncome += incomeDAO.GetTotalByMonth(month, m_selectedYear - 1);
                    auto prevExpenses = expenseDAO.GetByMonth(month, m_selectedYear - 1);
                    for (const auto& exp : prevExpenses)
                    {
                        double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                        prevExpense += value;
                    }
                }
            }
            else
            {
                // Comparar com mês anterior
                int prevMonth = m_selectedMonth - 1;
                int prevYear = m_selectedYear;
                if (prevMonth < 1)
                {
                    prevMonth = 12;
                    prevYear--;
                }

                prevIncome = incomeDAO.GetTotalByMonth(prevMonth, prevYear);
                auto prevExpenses = expenseDAO.GetByMonth(prevMonth, prevYear);
                for (const auto& exp : prevExpenses)
                {
                    double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                    prevExpense += value;
                }
            }

            double currentBalance = totalIncome - totalExpenseAmount;
            double previousBalance = prevIncome - prevExpense;
            double evolution = currentBalance - previousBalance;

            if (m_evolutionText)
            {
                m_evolutionText->SetLabel(MoneyUtils::FormatMoney(evolution));
                m_evolutionText->SetForegroundColour(evolution >= 0 ?
                    wxColour(39, 174, 96) : wxColour(192, 57, 43));
            }

            // Atualizar label do período
            if (m_periodLabel)
            {
                wxString periodText;
                if (m_selectedMonth == 0)
                {
                    periodText = wxString::Format(wxT("Ano %d"), m_selectedYear);
                }
                else
                {
                    wxString monthName = wxDateTime::GetMonthName((wxDateTime::Month)(m_selectedMonth - 1));
                    periodText = wxString::Format(wxT("%s/%d"), monthName, m_selectedYear);
                }
                m_periodLabel->SetLabel(wxT("Período: ") + periodText);
            }
        }
        catch (...)
        {
            // Em caso de erro, apenas não atualiza
        }
    }

private:
    void CreateInterface()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Título e filtro de mês
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("💡 Indicadores"));
        wxFont titleFont = title->GetFont();
        titleFont.SetPointSize(10);
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);
        headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

        headerSizer->AddStretchSpacer();

        // Filtro de mês para indicadores
        wxStaticText* monthLabel = new wxStaticText(this, wxID_ANY, wxT("Mês:"));
        headerSizer->Add(monthLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxArrayString months;
        months.Add(wxT("Todos"));
        for (int i = 1; i <= 12; i++)
        {
            months.Add(wxDateTime::GetMonthName((wxDateTime::Month)(i - 1), wxDateTime::Name_Abbr));
        }

        m_monthChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(80, -1), months);
        m_monthChoice->SetSelection(m_selectedMonth);
        m_monthChoice->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) {
            m_selectedMonth = m_monthChoice->GetSelection();
            LoadIndicators();
        });

        headerSizer->Add(m_monthChoice, 0, wxALIGN_CENTER_VERTICAL);

        mainSizer->Add(headerSizer, 0, wxALL | wxEXPAND, 5);

        // Label do período atual
        m_periodLabel = new wxStaticText(this, wxID_ANY, wxT("Período: Carregando..."));
        wxFont periodFont = m_periodLabel->GetFont();
        periodFont.SetPointSize(8);
        m_periodLabel->SetFont(periodFont);
        m_periodLabel->SetForegroundColour(wxColour(100, 100, 100));
        mainSizer->Add(m_periodLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);

        // Grid de indicadores
        wxFlexGridSizer* gridSizer = new wxFlexGridSizer(3, 2, 10, 10);
        gridSizer->AddGrowableCol(1);

        // Despesas Pagas
        wxStaticText* label1 = new wxStaticText(this, wxID_ANY, wxT("Despesas Pagas:"));
        m_percentPaidText = new wxStaticText(this, wxID_ANY, wxT("Calculando..."));
        gridSizer->Add(label1, 0, wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(m_percentPaidText, 0, wxEXPAND);

        // Comprometimento da Renda
        wxStaticText* label2 = new wxStaticText(this, wxID_ANY, wxT("Comprometimento:"));
        m_commitmentRateText = new wxStaticText(this, wxID_ANY, wxT("Calculando..."));
        gridSizer->Add(label2, 0, wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(m_commitmentRateText, 0, wxEXPAND);

        // Evolução Patrimonial
        wxStaticText* label3 = new wxStaticText(this, wxID_ANY, wxT("Evolução:"));
        m_evolutionText = new wxStaticText(this, wxID_ANY, wxT("Calculando..."));
        gridSizer->Add(label3, 0, wxALIGN_CENTER_VERTICAL);
        gridSizer->Add(m_evolutionText, 0, wxEXPAND);

        mainSizer->Add(gridSizer, 1, wxALL | wxEXPAND, 10);
        SetSizer(mainSizer);
    }

    int m_selectedMonth;
    int m_selectedYear;
    wxChoice* m_monthChoice;
    wxStaticText* m_periodLabel;
    wxStaticText* m_percentPaidText;
    wxStaticText* m_commitmentRateText;
    wxStaticText* m_evolutionText;
};

// ============================================================================
// ABA 1: RESUMO GERAL
// ============================================================================

class OverviewPanel : public wxScrolledWindow
{
public:
    OverviewPanel(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY)
        , m_selectedYear(wxDateTime::Now().GetYear())
    {
        SetScrollRate(5, 5);
        CreateInterface();
        LoadData();
    }

    void LoadData()
    {
        try
        {
            IncomeDAO incomeDAO;
            ExpenseDAO expenseDAO;
            InvestmentDAO investmentDAO;

            double totalIncome = 0, totalExpense = 0, totalInvestment = 0;

            for (int month = 1; month <= 12; month++)
            {
                totalIncome += incomeDAO.GetTotalByMonth(month, m_selectedYear);
                totalInvestment += investmentDAO.GetTotalByMonth(month, m_selectedYear);

                auto expenses = expenseDAO.GetByMonth(month, m_selectedYear);
                for (const auto& exp : expenses)
                {
                    double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                    totalExpense += value;
                }
            }

            // Atualizar cards
            if (m_cardIncome) m_cardIncome->UpdateValue(MoneyUtils::FormatMoney(totalIncome));
            if (m_cardExpense) m_cardExpense->UpdateValue(MoneyUtils::FormatMoney(totalExpense));
            if (m_cardInvestment) m_cardInvestment->UpdateValue(MoneyUtils::FormatMoney(totalInvestment));
            if (m_cardBalance) m_cardBalance->UpdateValue(MoneyUtils::FormatMoney(totalIncome - totalExpense));

            // Atualizar alertas e indicadores
            if (m_alertsPanel) m_alertsPanel->LoadAlerts();
            if (m_indicatorsPanel)
            {
                m_indicatorsPanel->SetYear(m_selectedYear); // Passar o ano selecionado
                m_indicatorsPanel->LoadIndicators();
            }

            Refresh();
        }
        catch (...)
        {
            wxLogError(wxT("Erro ao carregar dados do resumo"));
        }
    }

private:
    void CreateInterface()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Header com filtro de ano
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Resumo Geral"));
        wxFont titleFont = title->GetFont();
        titleFont.SetPointSize(14);
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);

        headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 20);
        headerSizer->AddStretchSpacer();
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Ano:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxSpinCtrl* yearSpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                              wxDefaultPosition, wxSize(80, -1),
                                              wxSP_ARROW_KEYS, 2000, 2100, m_selectedYear);

        yearSpin->Bind(wxEVT_SPINCTRL, [this, yearSpin](wxSpinEvent& event) {
            m_selectedYear = yearSpin->GetValue();

            // Atualizar gráficos
            if (m_chartBalance) ((BalanceLineChart*)m_chartBalance)->SetYear(m_selectedYear);
            if (m_chartIncome) ((IncomeLineChart*)m_chartIncome)->SetYear(m_selectedYear);
            if (m_chartExpense) ((ExpenseLineChart*)m_chartExpense)->SetYear(m_selectedYear);
            if (m_chartInvestment) ((InvestmentLineChart*)m_chartInvestment)->SetYear(m_selectedYear);

            // Atualizar indicadores com o novo ano
            if (m_indicatorsPanel)
            {
                m_indicatorsPanel->SetYear(m_selectedYear);
            }

            LoadData();
        });

        headerSizer->Add(yearSpin, 0, wxALIGN_CENTER_VERTICAL);
        mainSizer->Add(headerSizer, 0, wxALL | wxEXPAND, 10);

        // Alertas e Indicadores
        wxBoxSizer* infoSizer = new wxBoxSizer(wxHORIZONTAL);
        m_alertsPanel = new AlertsPanel(this);
        m_indicatorsPanel = new IndicatorsPanel(this, m_selectedYear); // Passar o ano inicial
        infoSizer->Add(m_alertsPanel, 1, wxALL | wxEXPAND, 5);
        infoSizer->Add(m_indicatorsPanel, 1, wxALL | wxEXPAND, 5);
        mainSizer->Add(infoSizer, 0, wxALL | wxEXPAND, 5);

        // Cards
        wxBoxSizer* cardsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_cardIncome = new InfoCard(this, wxT("Receita Total"), wxT("R$ 0,00"), wxColour(46, 204, 113));
        m_cardExpense = new InfoCard(this, wxT("Despesa Total"), wxT("R$ 0,00"), wxColour(231, 76, 60));
        m_cardInvestment = new InfoCard(this, wxT("Investimento Total"), wxT("R$ 0,00"), wxColour(155, 89, 182));
        m_cardBalance = new InfoCard(this, wxT("Saldo Total"), wxT("R$ 0,00"), wxColour(52, 152, 219));

        cardsSizer->Add(m_cardIncome, 1, wxALL | wxEXPAND, 5);
        cardsSizer->Add(m_cardExpense, 1, wxALL | wxEXPAND, 5);
        cardsSizer->Add(m_cardInvestment, 1, wxALL | wxEXPAND, 5);
        cardsSizer->Add(m_cardBalance, 1, wxALL | wxEXPAND, 5);
        mainSizer->Add(cardsSizer, 0, wxALL | wxEXPAND, 5);

        // Gráficos
        wxGridSizer* chartsSizer = new wxGridSizer(2, 2, 10, 10);
        m_chartBalance = new BalanceLineChart(this, m_selectedYear);
        m_chartIncome = new IncomeLineChart(this, m_selectedYear);
        m_chartExpense = new ExpenseLineChart(this, m_selectedYear);
        m_chartInvestment = new InvestmentLineChart(this, m_selectedYear);

        chartsSizer->Add(m_chartBalance, 1, wxEXPAND);
        chartsSizer->Add(m_chartIncome, 1, wxEXPAND);
        chartsSizer->Add(m_chartExpense, 1, wxEXPAND);
        chartsSizer->Add(m_chartInvestment, 1, wxEXPAND);
        mainSizer->Add(chartsSizer, 1, wxALL | wxEXPAND, 10);

        SetSizer(mainSizer);
    }

    int m_selectedYear;
    InfoCard *m_cardIncome, *m_cardExpense, *m_cardInvestment, *m_cardBalance;
    wxPanel *m_chartBalance, *m_chartIncome, *m_chartExpense, *m_chartInvestment;
    AlertsPanel* m_alertsPanel;
    IndicatorsPanel* m_indicatorsPanel;
};

// ============================================================================
// ABA 2: DASHBOARD DE DESPESAS
// ============================================================================

class ExpensesDashboardPanel : public wxScrolledWindow
{
public:
    ExpensesDashboardPanel(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY)
        , m_selectedMonth(0) // 0 = Todos
        , m_selectedYear(wxDateTime::Now().GetYear())
    {
        SetScrollRate(5, 5);
        CreateInterface();
        LoadData();
    }

    void LoadData()
    {
        try
        {
            ExpenseDAO dao;
            std::vector<Expense> expenses;

            // Carregar despesas baseado no filtro
            if (m_selectedMonth == 0)
            {
                // Todos os meses do ano
                for (int month = 1; month <= 12; month++)
                {
                    auto monthExpenses = dao.GetByMonth(month, m_selectedYear);
                    expenses.insert(expenses.end(), monthExpenses.begin(), monthExpenses.end());
                }
            }
            else
            {
                expenses = dao.GetByMonth(m_selectedMonth, m_selectedYear);
            }

            // Calcular totais para os cards
            double totalExpense = 0, totalToPay = 0;
            int countToPay = 0, countPaid = 0, countLate = 0, countInDebt = 0;

            // Maps para gráficos
            std::map<wxString, double> typeMap;
            std::map<wxString, double> paymentMap;
            std::map<wxString, int> statusMap;

            for (const auto& exp : expenses)
            {
                double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                totalExpense += value;

                // Por status
                switch (exp.status)
                {
                    case ExpenseStatus::TO_PAY:
                        totalToPay += value;
                        countToPay++;
                        statusMap[wxT("A pagar")]++;
                        break;
                    case ExpenseStatus::PAID:
                        countPaid++;
                        statusMap[wxT("Pago")]++;
                        break;
                    case ExpenseStatus::LATE:
                        countLate++;
                        totalToPay += value;
                        statusMap[wxT("Atrasado")]++;
                        break;
                    case ExpenseStatus::IN_DEBT:
                        totalToPay += value;
                        countInDebt++;
                        statusMap[wxT("Em dívida")]++;
                        break;
                }

                // Por tipo
                if (!exp.expenseType.IsEmpty())
                    typeMap[exp.expenseType] += value;

                // Por forma de pagamento
                if (!exp.paymentMethod.IsEmpty())
                    paymentMap[exp.paymentMethod] += value;
            }

            // Atualizar cards
            if (m_cardTotal) m_cardTotal->UpdateValue(MoneyUtils::FormatMoney(totalExpense));
            if (m_cardToPay) m_cardToPay->UpdateValue(MoneyUtils::FormatMoney(totalToPay));
            if (m_cardCountToPay) m_cardCountToPay->UpdateValue(wxString::Format(wxT("%d"), countToPay));
            if (m_cardPaid) m_cardPaid->UpdateValue(wxString::Format(wxT("%d"), countPaid));
            if (m_cardLate) m_cardLate->UpdateValue(wxString::Format(wxT("%d"), countLate));
            if (m_cardInDebt) m_cardInDebt->UpdateValue(wxString::Format(wxT("%d"), countInDebt));

            // Atualizar gráfico de despesas mensais (sempre mostra o ano todo)
            if (m_chartMonthly)
            {
                std::vector<double> monthlyValues(12, 0.0);
                for (int month = 1; month <= 12; month++)
                {
                    auto monthExpenses = dao.GetByMonth(month, m_selectedYear);
                    for (const auto& exp : monthExpenses)
                    {
                        double value = exp.adjustedAmount > 0 ? exp.adjustedAmount : exp.amount;
                        monthlyValues[month - 1] += value;
                    }
                }
                ((MonthlyLineChart*)m_chartMonthly)->SetData(monthlyValues);
            }

            // Atualizar gráfico por tipo
            if (m_chartByType)
            {
                std::vector<PieChartData> typeData;
                int colorIndex = 0;
                for (const auto& pair : typeMap)
                {
                    PieChartData data;
                    data.label = pair.first;
                    data.value = pair.second;
                    data.color = ChartUtils::GetChartColor(colorIndex++);
                    typeData.push_back(data);
                }
                ((PieChartPanel*)m_chartByType)->SetData(typeData);
            }

            // Atualizar gráfico por forma de pagamento
            if (m_chartByPayment)
            {
                std::vector<BarChartData> paymentData;
                int colorIndex = 0;
                for (const auto& pair : paymentMap)
                {
                    BarChartData data;
                    data.label = pair.first;
                    data.value = pair.second;
                    data.color = ChartUtils::GetChartColor(colorIndex++);
                    paymentData.push_back(data);
                }
                ((BarChartPanel*)m_chartByPayment)->SetData(paymentData);
            }

            // Atualizar gráfico por situação
            if (m_chartByStatus)
            {
                std::vector<PieChartData> statusData;

                if (statusMap[wxT("Pago")] > 0)
                {
                    PieChartData data;
                    data.label = wxT("Pago");
                    data.value = statusMap[wxT("Pago")];
                    data.color = wxColour(46, 204, 113);
                    statusData.push_back(data);
                }

                if (statusMap[wxT("A pagar")] > 0)
                {
                    PieChartData data;
                    data.label = wxT("A pagar");
                    data.value = statusMap[wxT("A pagar")];
                    data.color = wxColour(52, 152, 219);
                    statusData.push_back(data);
                }

                if (statusMap[wxT("Atrasado")] > 0)
                {
                    PieChartData data;
                    data.label = wxT("Atrasado");
                    data.value = statusMap[wxT("Atrasado")];
                    data.color = wxColour(241, 196, 15);
                    statusData.push_back(data);
                }

                if (statusMap[wxT("Em dívida")] > 0)
                {
                    PieChartData data;
                    data.label = wxT("Em dívida");
                    data.value = statusMap[wxT("Em dívida")];
                    data.color = wxColour(231, 76, 60);
                    statusData.push_back(data);
                }

                ((PieChartPanel*)m_chartByStatus)->SetData(statusData);
            }

            Refresh();
        }
        catch (...)
        {
            wxLogError(wxT("Erro ao carregar dados de despesas"));
        }
    }

private:
    void CreateInterface()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Header com filtros
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Dashboard de Despesas"));
        wxFont titleFont = title->GetFont();
        titleFont.SetPointSize(14);
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);

        headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 20);
        headerSizer->AddStretchSpacer();

        // Filtro de mês
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Mês:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxArrayString months;
        months.Add(wxT("Todos"));
        for (int i = 1; i <= 12; i++)
            months.Add(wxDateTime::GetMonthName((wxDateTime::Month)(i - 1)));

        wxChoice* monthChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition,
                                            wxSize(100, -1), months);
        monthChoice->SetSelection(m_selectedMonth);
        monthChoice->Bind(wxEVT_CHOICE, [this, monthChoice](wxCommandEvent&) {
            m_selectedMonth = monthChoice->GetSelection();
            LoadData();
        });

        headerSizer->Add(monthChoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

        // Filtro de ano
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Ano:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxSpinCtrl* yearSpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                              wxDefaultPosition, wxSize(80, -1),
                                              wxSP_ARROW_KEYS, 2000, 2100, m_selectedYear);
        yearSpin->Bind(wxEVT_SPINCTRL, [this, yearSpin](wxSpinEvent&) {
            m_selectedYear = yearSpin->GetValue();
            LoadData();
        });

        headerSizer->Add(yearSpin, 0, wxALIGN_CENTER_VERTICAL);
        mainSizer->Add(headerSizer, 0, wxALL | wxEXPAND, 10);

        // Cards - Linha 1
        wxBoxSizer* cardsRow1 = new wxBoxSizer(wxHORIZONTAL);
        m_cardTotal = new InfoCard(this, wxT("Despesa Total"), wxT("R$ 0,00"), wxColour(231, 76, 60));
        m_cardToPay = new InfoCard(this, wxT("Despesa a Pagar"), wxT("R$ 0,00"), wxColour(52, 152, 219));
        m_cardCountToPay = new InfoCard(this, wxT("Contas a Pagar"), wxT("0"), wxColour(52, 152, 219));

        cardsRow1->Add(m_cardTotal, 1, wxALL | wxEXPAND, 5);
        cardsRow1->Add(m_cardToPay, 1, wxALL | wxEXPAND, 5);
        cardsRow1->Add(m_cardCountToPay, 1, wxALL | wxEXPAND, 5);
        mainSizer->Add(cardsRow1, 0, wxALL | wxEXPAND, 5);

        // Cards - Linha 2
        wxBoxSizer* cardsRow2 = new wxBoxSizer(wxHORIZONTAL);
        m_cardPaid = new InfoCard(this, wxT("Contas Pagas"), wxT("0"), wxColour(46, 204, 113));
        m_cardLate = new InfoCard(this, wxT("Contas Atrasadas"), wxT("0"), wxColour(241, 196, 15));
        m_cardInDebt = new InfoCard(this, wxT("Contas em Dívida"), wxT("0"), wxColour(192, 57, 43));

        cardsRow2->Add(m_cardPaid, 1, wxALL | wxEXPAND, 5);
        cardsRow2->Add(m_cardLate, 1, wxALL | wxEXPAND, 5);
        cardsRow2->Add(m_cardInDebt, 1, wxALL | wxEXPAND, 5);
        mainSizer->Add(cardsRow2, 0, wxALL | wxEXPAND, 5);

        // Gráficos
        wxGridSizer* chartsSizer = new wxGridSizer(2, 2, 10, 10);

        m_chartMonthly = new ExpenseLineChart(this, m_selectedYear);
        m_chartByType = new PieChartPanel(this, wxT("Despesas por Tipo"));
        m_chartByPayment = new BarChartPanel(this, wxT("Despesas por Forma de Pagamento"));
        m_chartByStatus = new PieChartPanel(this, wxT("Despesas por Situação"));

        chartsSizer->Add(m_chartMonthly, 1, wxEXPAND);
        chartsSizer->Add(m_chartByType, 1, wxEXPAND);
        chartsSizer->Add(m_chartByPayment, 1, wxEXPAND);
        chartsSizer->Add(m_chartByStatus, 1, wxEXPAND);
        mainSizer->Add(chartsSizer, 1, wxALL | wxEXPAND, 10);

        SetSizer(mainSizer);
    }

    int m_selectedMonth, m_selectedYear;
    InfoCard *m_cardTotal, *m_cardToPay, *m_cardCountToPay, *m_cardPaid, *m_cardLate, *m_cardInDebt;
    wxPanel *m_chartMonthly, *m_chartByType, *m_chartByPayment, *m_chartByStatus;
};

// ============================================================================
// ABA 3: DASHBOARD DE RECEITAS
// ============================================================================

class IncomesDashboardPanel : public wxScrolledWindow
{
public:
    IncomesDashboardPanel(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY)
        , m_selectedMonth(0) // 0 = Todos
        , m_selectedYear(wxDateTime::Now().GetYear())
    {
        SetScrollRate(5, 5);
        CreateInterface();
        LoadData();
    }

    void LoadData()
    {
        try
        {
            IncomeDAO dao;
            std::vector<Income> incomes;

            // Carregar receitas baseado no filtro
            if (m_selectedMonth == 0)
            {
                for (int month = 1; month <= 12; month++)
                {
                    auto monthIncomes = dao.GetByMonth(month, m_selectedYear);
                    incomes.insert(incomes.end(), monthIncomes.begin(), monthIncomes.end());
                }
            }
            else
            {
                incomes = dao.GetByMonth(m_selectedMonth, m_selectedYear);
            }

            // Calcular totais
            double totalIncome = 0;
            std::map<wxString, double> typeMap;
            std::map<wxString, double> destinationMap;

            for (const auto& income : incomes)
            {
                totalIncome += income.amount;

                if (!income.incomeType.IsEmpty())
                    typeMap[income.incomeType] += income.amount;

                if (!income.destination.IsEmpty())
                    destinationMap[income.destination] += income.amount;
            }

            // Atualizar cards
            if (m_cardTotal) m_cardTotal->UpdateValue(MoneyUtils::FormatMoney(totalIncome));
            if (m_cardCount) m_cardCount->UpdateValue(wxString::Format(wxT("%d"), (int)incomes.size()));

            // Atualizar gráfico mensal (sempre mostra o ano todo)
            if (m_chartMonthly)
            {
                std::vector<double> monthlyValues(12, 0.0);
                for (int month = 1; month <= 12; month++)
                {
                    monthlyValues[month - 1] = dao.GetTotalByMonth(month, m_selectedYear);
                }
                ((MonthlyLineChart*)m_chartMonthly)->SetData(monthlyValues);
            }

            // Atualizar gráfico por tipo
            if (m_chartByType)
            {
                std::vector<PieChartData> typeData;
                int colorIndex = 0;
                for (const auto& pair : typeMap)
                {
                    PieChartData data;
                    data.label = pair.first;
                    data.value = pair.second;
                    data.color = ChartUtils::GetChartColor(colorIndex++);
                    typeData.push_back(data);
                }
                ((PieChartPanel*)m_chartByType)->SetData(typeData);
            }

            // Atualizar gráfico por destino
            if (m_chartByDestination)
            {
                std::vector<BarChartData> destData;
                int colorIndex = 0;
                for (const auto& pair : destinationMap)
                {
                    BarChartData data;
                    data.label = pair.first;
                    data.value = pair.second;
                    data.color = ChartUtils::GetChartColor(colorIndex++);
                    destData.push_back(data);
                }
                ((BarChartPanel*)m_chartByDestination)->SetData(destData);
            }

            Refresh();
        }
        catch (...)
        {
            wxLogError(wxT("Erro ao carregar dados de receitas"));
        }
    }

private:
    void CreateInterface()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Header com filtros
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Dashboard de Receitas"));
        wxFont titleFont = title->GetFont();
        titleFont.SetPointSize(14);
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);

        headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 20);
        headerSizer->AddStretchSpacer();

        // Filtro de mês
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Mês:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxArrayString months;
        months.Add(wxT("Todos"));
        for (int i = 1; i <= 12; i++)
            months.Add(wxDateTime::GetMonthName((wxDateTime::Month)(i - 1)));

        wxChoice* monthChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition,
                                            wxSize(100, -1), months);
        monthChoice->SetSelection(m_selectedMonth);
        monthChoice->Bind(wxEVT_CHOICE, [this, monthChoice](wxCommandEvent&) {
            m_selectedMonth = monthChoice->GetSelection();
            LoadData();
        });

        headerSizer->Add(monthChoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

        // Filtro de ano
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Ano:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxSpinCtrl* yearSpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                              wxDefaultPosition, wxSize(80, -1),
                                              wxSP_ARROW_KEYS, 2000, 2100, m_selectedYear);
        yearSpin->Bind(wxEVT_SPINCTRL, [this, yearSpin](wxSpinEvent&) {
            m_selectedYear = yearSpin->GetValue();
            LoadData();
        });

        headerSizer->Add(yearSpin, 0, wxALIGN_CENTER_VERTICAL);
        mainSizer->Add(headerSizer, 0, wxALL | wxEXPAND, 10);

        // Cards
        wxBoxSizer* cardsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_cardTotal = new InfoCard(this, wxT("Receita Total"), wxT("R$ 0,00"), wxColour(46, 204, 113));
        m_cardCount = new InfoCard(this, wxT("Qtde. Receitas"), wxT("0"), wxColour(52, 152, 219));

        cardsSizer->Add(m_cardTotal, 1, wxALL | wxEXPAND, 5);
        cardsSizer->Add(m_cardCount, 1, wxALL | wxEXPAND, 5);
        cardsSizer->AddStretchSpacer(2); // Espaço vazio para manter alinhamento
        mainSizer->Add(cardsSizer, 0, wxALL | wxEXPAND, 5);

        // Gráficos
        wxBoxSizer* chartsSizer = new wxBoxSizer(wxVERTICAL);

        // Linha 1: Gráfico de receitas mensais (ocupa toda a largura)
        m_chartMonthly = new IncomeLineChart(this, m_selectedYear);
        m_chartMonthly->SetMinSize(wxSize(-1, 250));
        chartsSizer->Add(m_chartMonthly, 0, wxALL | wxEXPAND, 10);

        // Linha 2: Gráficos de pizza e barra
        wxBoxSizer* chartsRow2 = new wxBoxSizer(wxHORIZONTAL);
        m_chartByType = new PieChartPanel(this, wxT("Receitas por Tipo"));
        m_chartByDestination = new BarChartPanel(this, wxT("Receitas por Destino"));

        chartsRow2->Add(m_chartByType, 1, wxALL | wxEXPAND, 5);
        chartsRow2->Add(m_chartByDestination, 1, wxALL | wxEXPAND, 5);
        chartsSizer->Add(chartsRow2, 1, wxEXPAND);

        mainSizer->Add(chartsSizer, 1, wxALL | wxEXPAND, 10);

        SetSizer(mainSizer);
    }

    int m_selectedMonth, m_selectedYear;
    InfoCard *m_cardTotal, *m_cardCount;
    wxPanel *m_chartMonthly, *m_chartByType, *m_chartByDestination;
};

// ============================================================================
// ABA 4: DASHBOARD DE INVESTIMENTOS
// ============================================================================

class InvestmentsDashboardPanel : public wxScrolledWindow
{
public:
    InvestmentsDashboardPanel(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY)
        , m_selectedMonth(0) // 0 = Todos
        , m_selectedYear(wxDateTime::Now().GetYear())
    {
        SetScrollRate(5, 5);
        CreateInterface();
        LoadData();
    }

    void LoadData()
    {
        try
        {
            InvestmentDAO dao;
            std::vector<Investment> investments;

            // Carregar investimentos baseado no filtro
            if (m_selectedMonth == 0)
            {
                for (int month = 1; month <= 12; month++)
                {
                    auto monthInvestments = dao.GetByMonth(month, m_selectedYear);
                    investments.insert(investments.end(), monthInvestments.begin(), monthInvestments.end());
                }
            }
            else
            {
                investments = dao.GetByMonth(m_selectedMonth, m_selectedYear);
            }

            // Calcular totais
            double totalInvestment = 0;
            std::map<wxString, double> typeMap;
            std::map<wxString, double> sourceMap;

            for (const auto& investment : investments)
            {
                totalInvestment += investment.amount;

                if (!investment.investmentType.IsEmpty())
                    typeMap[investment.investmentType] += investment.amount;

                if (!investment.source.IsEmpty())
                    sourceMap[investment.source] += investment.amount;
            }

            // Atualizar cards
            if (m_cardTotal) m_cardTotal->UpdateValue(MoneyUtils::FormatMoney(totalInvestment));
            if (m_cardCount) m_cardCount->UpdateValue(wxString::Format(wxT("%d"), (int)investments.size()));

            // Atualizar gráfico mensal (sempre mostra o ano todo)
            if (m_chartMonthly)
            {
                std::vector<double> monthlyValues(12, 0.0);
                for (int month = 1; month <= 12; month++)
                {
                    monthlyValues[month - 1] = dao.GetTotalByMonth(month, m_selectedYear);
                }
                ((MonthlyLineChart*)m_chartMonthly)->SetData(monthlyValues);
            }

            // Atualizar gráfico por tipo
            if (m_chartByType)
            {
                std::vector<PieChartData> typeData;
                int colorIndex = 0;
                for (const auto& pair : typeMap)
                {
                    PieChartData data;
                    data.label = pair.first;
                    data.value = pair.second;
                    data.color = ChartUtils::GetChartColor(colorIndex++);
                    typeData.push_back(data);
                }
                ((PieChartPanel*)m_chartByType)->SetData(typeData);
            }

            // Atualizar gráfico por fonte
            if (m_chartBySource)
            {
                std::vector<BarChartData> sourceData;
                int colorIndex = 0;
                for (const auto& pair : sourceMap)
                {
                    BarChartData data;
                    data.label = pair.first;
                    data.value = pair.second;
                    data.color = ChartUtils::GetChartColor(colorIndex++);
                    sourceData.push_back(data);
                }
                ((BarChartPanel*)m_chartBySource)->SetData(sourceData);
            }

            Refresh();
        }
        catch (...)
        {
            wxLogError(wxT("Erro ao carregar dados de investimentos"));
        }
    }

private:
    void CreateInterface()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Header com filtros
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("Dashboard de Investimentos"));
        wxFont titleFont = title->GetFont();
        titleFont.SetPointSize(14);
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);

        headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 20);
        headerSizer->AddStretchSpacer();

        // Filtro de mês
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Mês:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxArrayString months;
        months.Add(wxT("Todos"));
        for (int i = 1; i <= 12; i++)
            months.Add(wxDateTime::GetMonthName((wxDateTime::Month)(i - 1)));

        wxChoice* monthChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition,
                                            wxSize(100, -1), months);
        monthChoice->SetSelection(m_selectedMonth);
        monthChoice->Bind(wxEVT_CHOICE, [this, monthChoice](wxCommandEvent&) {
            m_selectedMonth = monthChoice->GetSelection();
            LoadData();
        });

        headerSizer->Add(monthChoice, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

        // Filtro de ano
        headerSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Ano:")),
                        0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxSpinCtrl* yearSpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString,
                                              wxDefaultPosition, wxSize(80, -1),
                                              wxSP_ARROW_KEYS, 2000, 2100, m_selectedYear);
        yearSpin->Bind(wxEVT_SPINCTRL, [this, yearSpin](wxSpinEvent&) {
            m_selectedYear = yearSpin->GetValue();
            LoadData();
        });

        headerSizer->Add(yearSpin, 0, wxALIGN_CENTER_VERTICAL);
        mainSizer->Add(headerSizer, 0, wxALL | wxEXPAND, 10);

        // Cards
        wxBoxSizer* cardsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_cardTotal = new InfoCard(this, wxT("Investimento Total"), wxT("R$ 0,00"), wxColour(155, 89, 182));
        m_cardCount = new InfoCard(this, wxT("Qtde. Investimentos"), wxT("0"), wxColour(52, 152, 219));

        cardsSizer->Add(m_cardTotal, 1, wxALL | wxEXPAND, 5);
        cardsSizer->Add(m_cardCount, 1, wxALL | wxEXPAND, 5);
        cardsSizer->AddStretchSpacer(2); // Espaço vazio para manter alinhamento
        mainSizer->Add(cardsSizer, 0, wxALL | wxEXPAND, 5);

        // Gráficos
        wxBoxSizer* chartsSizer = new wxBoxSizer(wxVERTICAL);

        // Linha 1: Gráfico de investimentos mensais (ocupa toda a largura)
        m_chartMonthly = new InvestmentLineChart(this, m_selectedYear);
        m_chartMonthly->SetMinSize(wxSize(-1, 250));
        chartsSizer->Add(m_chartMonthly, 0, wxALL | wxEXPAND, 10);

        // Linha 2: Gráficos de pizza e barra
        wxBoxSizer* chartsRow2 = new wxBoxSizer(wxHORIZONTAL);
        m_chartByType = new PieChartPanel(this, wxT("Investimentos por Tipo"));
        m_chartBySource = new BarChartPanel(this, wxT("Investimentos por Fonte"));

        chartsRow2->Add(m_chartByType, 1, wxALL | wxEXPAND, 5);
        chartsRow2->Add(m_chartBySource, 1, wxALL | wxEXPAND, 5);
        chartsSizer->Add(chartsRow2, 1, wxEXPAND);

        mainSizer->Add(chartsSizer, 1, wxALL | wxEXPAND, 10);

        SetSizer(mainSizer);
    }

    int m_selectedMonth, m_selectedYear;
    InfoCard *m_cardTotal, *m_cardCount;
    wxPanel *m_chartMonthly, *m_chartByType, *m_chartBySource;
};

// ============================================================================
// IMPLEMENTAÇÃO DO DASHBOARDPANEL PRINCIPAL
// ============================================================================

DashboardPanel::DashboardPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY)
{
    CreateInterface();
}

DashboardPanel::~DashboardPanel()
{
}

void DashboardPanel::CreateInterface()
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Título principal
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("📊 Dashboard Financeiro"));
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(16);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);

    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    // Criar notebook com as 4 abas
    m_notebook = new wxNotebook(this, wxID_ANY);

    try
    {
        // Adicionar as 4 abas
        m_notebook->AddPage(new OverviewPanel(m_notebook), wxT("Resumo Geral"));
        m_notebook->AddPage(new ExpensesDashboardPanel(m_notebook), wxT("Despesas"));
        m_notebook->AddPage(new IncomesDashboardPanel(m_notebook), wxT("Receitas"));
        m_notebook->AddPage(new InvestmentsDashboardPanel(m_notebook), wxT("Investimentos"));

        m_notebook->SetSelection(0);
    }
    catch (const std::exception& e)
    {
        wxLogError(wxT("Erro ao criar painéis do dashboard: %s"), e.what());
    }
    catch (...)
    {
        wxLogError(wxT("Erro desconhecido ao criar painéis do dashboard"));
    }

    mainSizer->Add(m_notebook, 1, wxALL | wxEXPAND, 5);
    SetSizer(mainSizer);
}
