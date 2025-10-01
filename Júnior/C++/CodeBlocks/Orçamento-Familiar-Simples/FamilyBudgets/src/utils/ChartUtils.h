#ifndef CHARTUTILS_H
#define CHARTUTILS_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <map>
#include <cmath>

// Definir M_PI se não estiver disponível (comum no Windows)
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Estrutura para dados de gráfico de linha
struct LineChartData
{
    wxString label;
    std::vector<double> values;
    wxColour color;
};

// Estrutura para dados de gráfico de pizza
struct PieChartData
{
    wxString label;
    double value;
    wxColour color;
};

// Estrutura para dados de gráfico de barra
struct BarChartData
{
    wxString label;
    double value;
    wxColour color;
};

// Classe utilitária para desenhar gráficos
class ChartUtils
{
public:
    // Desenhar gráfico de linha
    static void DrawLineChart(wxDC& dc, const wxRect& rect,
                             const std::vector<LineChartData>& series,
                             const wxArrayString& labels,
                             const wxString& title = wxEmptyString)
    {
        // Validações de segurança
        if (series.empty() || labels.IsEmpty()) return;
        if (rect.width <= 0 || rect.height <= 0) return;

        // Cores de fundo
        dc.SetBrush(wxBrush(*wxWHITE));
        dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
        dc.DrawRectangle(rect);

        // Margens
        int marginLeft = 60;
        int marginRight = 20;
        int marginTop = title.IsEmpty() ? 20 : 50;
        int marginBottom = 40;

        // Verificar se há espaço suficiente
        if (rect.width <= (marginLeft + marginRight) ||
            rect.height <= (marginTop + marginBottom)) return;

        wxRect chartRect(rect.x + marginLeft, rect.y + marginTop,
                        rect.width - marginLeft - marginRight,
                        rect.height - marginTop - marginBottom);

        // Título
        if (!title.IsEmpty())
        {
            dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
            dc.SetTextForeground(*wxBLACK);
            wxSize titleSize = dc.GetTextExtent(title);
            dc.DrawText(title, rect.x + (rect.width - titleSize.x) / 2, rect.y + 10);
        }

        // Encontrar valores mínimo e máximo
        double minVal = 0;
        double maxVal = 0;
        bool hasData = false;

        for (const auto& serie : series)
        {
            for (double val : serie.values)
            {
                if (!hasData)
                {
                    minVal = val;
                    maxVal = val;
                    hasData = true;
                }
                else
                {
                    if (val > maxVal) maxVal = val;
                    if (val < minVal) minVal = val;
                }
            }
        }

        // Se não houver dados, desenhar apenas eixos
        if (!hasData)
        {
            maxVal = 100;
            minVal = 0;
        }

        // Adicionar margem nos valores
        double range = maxVal - minVal;
        if (range < 0.001) range = 1.0; // Evitar divisão por zero
        maxVal += range * 0.1;
        minVal -= range * 0.1;

        // Desenhar grade
        dc.SetPen(wxPen(wxColour(230, 230, 230), 1));
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.SetTextForeground(wxColour(100, 100, 100));

        int gridLines = 5;
        for (int i = 0; i <= gridLines; i++)
        {
            int y = chartRect.y + (chartRect.height * i / gridLines);
            dc.DrawLine(chartRect.x, y, chartRect.x + chartRect.width, y);

            double value = maxVal - ((maxVal - minVal) * i / gridLines);
            wxString label = wxString::Format(wxT("%.0f"), value);
            dc.DrawText(label, chartRect.x - 50, y - 7);
        }

        // Desenhar linhas dos dados
        int pointCount = labels.GetCount();
        if (pointCount < 2) return; // Precisa de pelo menos 2 pontos

        double xStep = (double)chartRect.width / wxMax(1, (pointCount - 1));

        for (const auto& serie : series)
        {
            if (serie.values.empty()) continue;

            dc.SetPen(wxPen(serie.color, 2));
            dc.SetBrush(wxBrush(serie.color));

            std::vector<wxPoint> points;

            for (size_t i = 0; i < wxMin(serie.values.size(), labels.GetCount()); i++)
            {
                int x = chartRect.x + (int)(i * xStep);

                double normalizedValue = 0;
                if (maxVal != minVal) // Evitar divisão por zero
                {
                    normalizedValue = (serie.values[i] - minVal) / (maxVal - minVal);
                }

                int y = chartRect.y + chartRect.height -
                       (int)(normalizedValue * chartRect.height);

                points.push_back(wxPoint(x, y));

                // Desenhar ponto
                dc.DrawCircle(x, y, 4);
            }

            // Desenhar linhas conectando os pontos
            if (points.size() >= 2)
            {
                for (size_t i = 0; i < points.size() - 1; i++)
                {
                    dc.DrawLine(points[i], points[i + 1]);
                }
            }
        }

        // Desenhar labels do eixo X
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.SetTextForeground(wxColour(100, 100, 100));

        for (size_t i = 0; i < labels.GetCount(); i++)
        {
            int x = chartRect.x + (int)(i * xStep);
            wxSize labelSize = dc.GetTextExtent(labels[i]);
            dc.DrawText(labels[i], x - labelSize.x / 2, chartRect.y + chartRect.height + 10);
        }

        // Legenda (se houver múltiplas séries)
        if (series.size() > 1)
        {
            int legendX = chartRect.x + chartRect.width - 100;
            int legendY = chartRect.y + 10;

            for (const auto& serie : series)
            {
                dc.SetBrush(wxBrush(serie.color));
                dc.SetPen(wxPen(serie.color, 1));
                dc.DrawRectangle(legendX, legendY, 15, 15);

                dc.SetTextForeground(*wxBLACK);
                dc.DrawText(serie.label, legendX + 20, legendY);

                legendY += 20;
            }
        }
    }

    // Desenhar gráfico de pizza
    static void DrawPieChart(wxDC& dc, const wxRect& rect,
                            const std::vector<PieChartData>& data,
                            const wxString& title = wxEmptyString)
    {
        // Validações de segurança
        if (data.empty()) return;
        if (rect.width <= 0 || rect.height <= 0) return;

        // Fundo
        dc.SetBrush(wxBrush(*wxWHITE));
        dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
        dc.DrawRectangle(rect);

        // Título
        int marginTop = 20;
        if (!title.IsEmpty())
        {
            dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
            dc.SetTextForeground(*wxBLACK);
            wxSize titleSize = dc.GetTextExtent(title);
            dc.DrawText(title, rect.x + (rect.width - titleSize.x) / 2, rect.y + 10);
            marginTop = 50;
        }

        // Calcular total
        double total = 0;
        for (const auto& item : data)
        {
            total += wxMax(0.0, item.value); // Garantir valores não negativos
        }

        if (total <= 0) return; // Evitar divisão por zero

        // Tamanho e posição da pizza
        int pieSize = wxMin(rect.width - 200, rect.height - marginTop - 40);
        if (pieSize <= 0) return;

        int centerX = rect.x + 100 + pieSize / 2;
        int centerY = rect.y + marginTop + pieSize / 2;
        int radius = pieSize / 2;

        // Desenhar fatias usando DrawEllipticArc (mais seguro que polígonos)
        double startAngle = 0;

        for (const auto& item : data)
        {
            if (item.value <= 0) continue; // Pular valores negativos ou zero

            double sweepAngle = (item.value / total) * 360.0;

            dc.SetBrush(wxBrush(item.color));
            dc.SetPen(wxPen(*wxWHITE, 2));

            // DrawEllipticArc é mais seguro que criar polígonos manualmente
            dc.DrawEllipticArc(centerX - radius, centerY - radius,
                              radius * 2, radius * 2,
                              startAngle, startAngle + sweepAngle);

            startAngle += sweepAngle;
        }

        // Legenda
        dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        int legendX = centerX + radius + 30;
        int legendY = rect.y + marginTop;
        int lineHeight = 25;

        for (const auto& item : data)
        {
            if (item.value <= 0) continue;

            double percentage = (item.value / total) * 100.0;

            // Cor
            dc.SetBrush(wxBrush(item.color));
            dc.SetPen(wxPen(item.color, 1));
            dc.DrawRectangle(legendX, legendY, 15, 15);

            // Texto
            dc.SetTextForeground(*wxBLACK);
            wxString label = wxString::Format(wxT("%s (%.1f%%)"), item.label, percentage);
            dc.DrawText(label, legendX + 20, legendY);

            legendY += lineHeight;
        }
    }

    // Desenhar gráfico de barras
    static void DrawBarChart(wxDC& dc, const wxRect& rect,
                            const std::vector<BarChartData>& data,
                            const wxString& title = wxEmptyString)
    {
        // Validações de segurança
        if (data.empty()) return;
        if (rect.width <= 0 || rect.height <= 0) return;

        // Fundo
        dc.SetBrush(wxBrush(*wxWHITE));
        dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
        dc.DrawRectangle(rect);

        // Margens
        int marginLeft = 60;
        int marginRight = 20;
        int marginTop = title.IsEmpty() ? 20 : 50;
        int marginBottom = 60;

        // Verificar se há espaço suficiente
        if (rect.width <= (marginLeft + marginRight) ||
            rect.height <= (marginTop + marginBottom)) return;

        wxRect chartRect(rect.x + marginLeft, rect.y + marginTop,
                        rect.width - marginLeft - marginRight,
                        rect.height - marginTop - marginBottom);

        // Título
        if (!title.IsEmpty())
        {
            dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
            dc.SetTextForeground(*wxBLACK);
            wxSize titleSize = dc.GetTextExtent(title);
            dc.DrawText(title, rect.x + (rect.width - titleSize.x) / 2, rect.y + 10);
        }

        // Encontrar valor máximo
        double maxVal = 0;
        for (const auto& item : data)
        {
            if (item.value > maxVal) maxVal = item.value;
        }

        if (maxVal <= 0) maxVal = 100; // Valor padrão se não houver dados
        maxVal *= 1.1; // Adicionar 10% de margem

        // Desenhar grade
        dc.SetPen(wxPen(wxColour(230, 230, 230), 1));
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.SetTextForeground(wxColour(100, 100, 100));

        int gridLines = 5;
        for (int i = 0; i <= gridLines; i++)
        {
            int y = chartRect.y + (chartRect.height * i / gridLines);
            dc.DrawLine(chartRect.x, y, chartRect.x + chartRect.width, y);

            double value = maxVal - (maxVal * i / gridLines);
            wxString label = wxString::Format(wxT("%.0f"), value);
            dc.DrawText(label, chartRect.x - 50, y - 7);
        }

        // Desenhar barras
        int barCount = data.size();
        if (barCount == 0) return;

        int barSpacing = 10;
        int totalSpacing = barSpacing * (barCount + 1);
        int availableWidth = chartRect.width - totalSpacing;

        if (availableWidth <= 0) return; // Não há espaço para desenhar

        int barWidth = availableWidth / barCount;
        barWidth = wxMin(barWidth, 100); // Limitar largura máxima

        int x = chartRect.x + barSpacing;

        for (const auto& item : data)
        {
            if (maxVal > 0) // Evitar divisão por zero
            {
                int barHeight = (int)((item.value / maxVal) * chartRect.height);
                int barY = chartRect.y + chartRect.height - barHeight;

                // Desenhar barra
                dc.SetBrush(wxBrush(item.color));
                dc.SetPen(wxPen(item.color, 1));
                dc.DrawRectangle(x, barY, barWidth, barHeight);

                // Label
                dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                dc.SetTextForeground(wxColour(100, 100, 100));

                wxSize labelSize = dc.GetTextExtent(item.label);

                // Se o texto for muito longo, abreviar
                wxString displayLabel = item.label;
                if (labelSize.x > barWidth + barSpacing)
                {
                    displayLabel = item.label.Left(10) + wxT("...");
                    labelSize = dc.GetTextExtent(displayLabel);
                }

                dc.DrawText(displayLabel, x + (barWidth - labelSize.x) / 2,
                           chartRect.y + chartRect.height + 10);
            }

            x += barWidth + barSpacing;
        }
    }

    // Cores padrão para gráficos
    static wxColour GetChartColor(int index)
    {
        static const wxColour colors[] = {
            wxColour(52, 152, 219),   // Azul
            wxColour(46, 204, 113),   // Verde
            wxColour(231, 76, 60),    // Vermelho
            wxColour(241, 196, 15),   // Amarelo
            wxColour(155, 89, 182),   // Roxo
            wxColour(230, 126, 34),   // Laranja
            wxColour(149, 165, 166),  // Cinza
            wxColour(26, 188, 156),   // Turquesa
            wxColour(192, 57, 43),    // Vermelho escuro
            wxColour(142, 68, 173)    // Roxo escuro
        };

        return colors[index % 10];
    }
};

#endif // CHARTUTILS_H
