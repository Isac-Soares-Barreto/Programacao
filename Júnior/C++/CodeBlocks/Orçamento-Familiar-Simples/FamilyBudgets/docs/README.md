# 📚 Documentação Técnica - Kaiser Family Budgets

<div align="center">

![Documentation](https://img.shields.io/badge/Documentação-v1.0-blue?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completa-success?style=for-the-badge)

**Central de documentação técnica e arquitetural do sistema**

</div>

---

## 📋 Índice

- [Visão Geral](#-visão-geral)
- [Arquitetura](#-arquitetura)
- [Banco de Dados](#-banco-de-dados)
- [Módulos](#-módulos)
- [API Interna](#-api-interna)
- [Fluxos Principais](#-fluxos-principais)
- [Guia de Desenvolvimento](#-guia-de-desenvolvimento)
- [Licença e Contato](#-licença-e-contato)

---

## 🎯 Visão Geral

O **Kaiser Family Budgets** é um sistema de gestão financeira familiar desenvolvido em **C++17** usando **wxWidgets 3.3.0** para interface gráfica e **SQLite3** para persistência de dados. O projeto é construído com foco em modularidade, performance e manutenibilidade.

### Objetivos Técnicos

- ✅ **Interface Responsiva**: Utilização de componentes nativos do wxWidgets para uma experiência fluida.
- ✅ **Banco de Dados Local**: Armazenamento seguro e offline dos dados do usuário.
- ✅ **Código Modular**: Separação clara de responsabilidades (UI, Lógica, Dados).
- ✅ **Performance Otimizada**: Consultas eficientes e renderização de interface com double buffering.
- ✅ **Suporte a Caracteres Especiais**: Configuração para UTF-8 em todo o sistema.

---

## 🏗️ Arquitetura

### Padrão MVC (Model-View-Controller)

O sistema segue uma adaptação do padrão **Model-View-Controller** para organizar o código:

```
┌─────────────────────────────────────────┐
│            VIEW (Interface)             │
│   src/panels/, src/dialogs/             │
└──────────────┬──────────────────────────┘
               │ Eventos do Usuário (wxEVT)
               ▼
┌─────────────────────────────────────────┐
│         CONTROLLER (Lógica)             │
│   DAOs, Utils, Calculators              │
└──────────────┬──────────────────────────┘
               │ Consultas e Comandos (CRUD)
               ▼
┌─────────────────────────────────────────┐
│          MODEL (Dados)                  │
│   src/models/ (Structs e Enums)         │
└──────────────┬──────────────────────────┘
               │ Persistência
               ▼
┌─────────────────────────────────────────┐
│      DATABASE (SQLite3)                 │
│   familybudgets.db                      │
└─────────────────────────────────────────┘
```

### Fluxo de Dados

```
Input do Usuário → Panel/Dialog → Event Handler → DAO → Database
                                                          ↓
Retorno da Interface ← Panel/Dialog ← Dados ← DAO ← Query Result
```

---

## 🗄️ Banco de Dados

### Schema SQLite3

O banco de dados, definido em `resources/database/schema.sql`, é composto por **10 tabelas principais** e **5 tabelas auxiliares**:

#### Tabelas Principais

| Tabela | Descrição | Chave Primária |
|--------|-----------|----------------|
| `expenses` | Despesas avulsas e importadas | (due_date, description) |
| `fixed_expenses` | Modelo para despesas fixas | (due_day, description) |
| `installment_expenses` | Modelo para despesas parceladas | (purchase_date, description, card_name) |
| `incomes` | Receitas | (receipt_date, description) |
| `investments` | Investimentos | (investment_date, description) |
| `cards` | Cartões de crédito | name |
| `payment_methods` | Formas de pagamento | name |
| `payment_categories` | Categorias customizadas | (payment_method_name, category_name) |

#### Tabelas Auxiliares (Categorias)

- `expense_categories`: Tipos de despesa (Alimentação, Moradia, etc.)
- `income_categories`: Tipos de receita (Salário, Freelance, etc.)
- `income_destinations`: Destinos de receita (Conta Corrente, Poupança, etc.)
- `investment_types`: Tipos de investimento (CDB, Ações, etc.)

### Índices

Para otimizar as consultas, foram criados os seguintes índices:
```sql
CREATE INDEX idx_expenses_due_date ON expenses(due_date);
CREATE INDEX idx_expenses_status ON expenses(status);
CREATE INDEX idx_incomes_date ON incomes(receipt_date);
CREATE INDEX idx_investments_date ON investments(investment_date);
```

---

## 📦 Módulos

### 1. **Módulo de Interface (src/panels/ & src/dialogs/)**

Responsável por toda a interação com o usuário.

**Painéis Principais:**
- `MainFrame`: Janela principal com o `wxNotebook` que gerencia as abas.
- `DashboardPanel`: Contém 4 sub-abas com gráficos e indicadores.
- `ExpensesPanel`: Listagem, filtros, ordenação e botões de ação para despesas.
- `FixedExpensesPanel`, `InstallmentExpensesPanel`, `IncomePanel`, etc.

**Diálogos:**
- Cada entidade (Despesa, Receita, Cartão) possui um diálogo modal para adição e edição.
- Utilizam validadores e controles customizados, como o `DecimalTextCtrl`.

### 2. **Módulo de Persistência (src/databases/)**

Implementa o padrão **DAO (Data Access Object)** para abstrair o acesso ao banco de dados.

**Classe Singleton `Database`:**
Gerencia a conexão com o SQLite, executa queries e transações de forma segura.

**DAOs Especializados:**
- `ExpenseDAO`: CRUD + `ImportFixedExpenses` + `ImportInstallmentExpenses`.
- `IncomeDAO`: CRUD + `GetTotalByMonth`.
- `InvestmentDAO`: CRUD + `GetTotalByType`.
- `CardDAO`, `PaymentMethodDAO`, etc.

### 3. **Módulo de Modelos (src/models/)**

Define as estruturas de dados (`structs`) e enumerações (`enums`) que representam as entidades do sistema.

**Exemplo (`Expense.h`):**
```cpp
struct Expense {
    wxDateTime dueDate;
    wxString description;
    double amount;
    ExpenseStatus status;
    // ... outros campos
    
    void CalculateStatus(bool isPaidChecked);
};
```

### 4. **Módulo de Utilitários (src/utils/)**

Contém classes e funções auxiliares reutilizáveis em todo o projeto.

**Principais Utilitários:**
- `DateUtils`: Formatação e parsing de datas.
- `MoneyUtils`: Formatação monetária para o padrão brasileiro (R$).
- `ChartUtils`: Funções estáticas para desenhar gráficos de linha, pizza e barras.
- `InstallmentCalculator`: Lógica de cálculo de vencimentos de parcelas.
- `DecimalTextCtrl`: Controle customizado para entrada de valores decimais.

---

## 🔌 API Interna

### Padrão de Uso dos DAOs

```cpp
// 1. Criar uma instância do DAO
ExpenseDAO dao;

// 2. Criar e preencher um modelo
Expense novaDespesa;
novaDespesa.description = wxT("Supermercado");
novaDespesa.amount = 250.75;
// ...

// 3. Chamar o método do DAO
if (dao.Insert(novaDespesa)) {
    // Sucesso!
} else {
    // Erro!
}

// 4. Para consultas
auto despesasDoMes = dao.GetByMonth(5, 2025);
```

### Padrão de Uso dos Utilitários

Os utilitários são, em sua maioria, classes estáticas, facilitando o uso direto.

```cpp
// Formatar valor para exibição
wxString valorFormatado = MoneyUtils::FormatMoney(1234.56);
// Retorna: "R$ 1.234,56"

// Formatar data
wxString dataFormatada = DateUtils::FormatDate(wxDateTime::Now());
// Retorna: "31/12/2025"

// Desenhar um gráfico (dentro de um evento OnPaint)
std::vector<PieChartData> dados;
// ... preencher dados
ChartUtils::DrawPieChart(dc, rect, dados, wxT("Meu Gráfico"));
```

---

## 🔄 Fluxos Principais

### Fluxo de Adição de Despesa

`Usuário clica em "Adicionar"` → `ExpensesPanel` → `ExpenseDialog` → `Usuário preenche dados` → `Validação` → `ExpenseDialog::GetExpense()` → `ExpenseDAO::Insert()` → `ExpensesPanel::LoadExpenses()` → `Interface atualizada`.

### Fluxo de Importação de Despesas Fixas

`Usuário clica em "Importar"` → `Diálogo de seleção de Mês/Ano` → `ExpenseDAO::ImportFixedExpenses()` → `SELECT * FROM fixed_expenses WHERE active=1` → `Loop`: `Cálculo da data` → `Verificação de duplicidade` → `INSERT INTO expenses` → `Feedback ao usuário` → `ExpensesPanel::LoadExpenses()`.

### Fluxo de Geração de Gráfico no Dashboard

`Aba do Dashboard é exibida` → `Painel do gráfico (ex: BalanceLineChart) é criado` → `LoadData()` do painel é chamado → `DAO correspondente (ex: IncomeDAO, ExpenseDAO) é consultado` → `Dados são processados e armazenados` → `Evento OnPaint é disparado` → `DrawChart()` → `ChartUtils::DrawLineChart()` desenha o gráfico na tela.

---

## 👨‍💻 Guia de Desenvolvimento

### Adicionando uma Nova Funcionalidade

Para adicionar uma nova entidade ao sistema (ex: "Metas Financeiras"):

1. **Modelo**: Crie `src/models/Meta.h`.
2. **Banco**: Adicione a tabela `metas` em `resources/database/schema.sql`.
3. **DAO**: Crie `src/databases/MetaDAO.h` e `MetaDAO.cpp`.
4. **Diálogo**: Crie `src/dialogs/MetaDialog.h` e `MetaDialog.cpp` para adicionar/editar.
5. **Painel**: Crie `src/panels/MetasPanel.h` e `MetasPanel.cpp` para listar as metas.
6. **Integração**: Adicione o `MetasPanel` como uma nova aba no `MainFrame.cpp`.

### Padrões de Código

- **Nomenclatura**: `CamelCase` para classes, `camelCase` para métodos e variáveis.
- **Comentários**: Em português, usando `//` para explicar a lógica.
- **Strings**: Sempre usar `wxT("")` para compatibilidade com Unicode.
- **Ícones**: Adicionar novos ícones em `resources/icons/` e referenciá-los.

---

## 📄 Licença e Contato

Este projeto é regido por uma licença personalizada. Para detalhes completos, consulte o arquivo [**LICENSE.md**](../LICENSE.md).

Para dúvidas técnicas, sugestões ou contribuições, entre em contato com o autor:

**Isac Soares Barreto**
- 📧 Email: isacprofessional@gmail.com
- 🐱 GitHub: [github.com/Isac-Soares-Barreto/](https://github.com/Isac-Soares-Barreto/)

---

<div align="center">

**📚 Documentação Kaiser Family Budgets v1.0**

*Última atualização: Setembro 2025*

</div>