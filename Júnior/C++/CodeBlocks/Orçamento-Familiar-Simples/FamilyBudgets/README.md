# 💰 Kaiser Family Budgets

<div align="center">

![Kaiser Family Budgets](https://img.shields.io/badge/Kaiser%20Family%20Budgets-v1.0-blue?style=for-the-badge)
![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![wxWidgets](https://img.shields.io/badge/wxWidgets-3.3.0-red?style=for-the-badge)
![SQLite](https://img.shields.io/badge/SQLite-3-003B57?style=for-the-badge&logo=sqlite&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D4?style=for-the-badge&logo=windows&logoColor=white)
![License](https://img.shields.io/badge/License-Custom-orange?style=for-the-badge)

**Sistema completo de gestão financeira familiar desenvolvido em C++ com interface gráfica moderna**

[🚀 Funcionalidades](#-funcionalidades) • [💻 Instalação](#-instalação) • [📱 Como Usar](#-como-usar) • [🏗️ Tecnologias](#️-tecnologias) • [📞 Contato](#-contato)

</div>

---

## 📖 Sobre o Projeto

O **Kaiser Family Budgets** é um sistema desktop robusto para gestão financeira familiar, desenvolvido em **C++17** com **wxWidgets 3.3.0**. Oferece controle completo de despesas (avulsas, fixas e parceladas), receitas, investimentos e cartões de crédito, com **dashboards interativos**, **gráficos dinâmicos** e **alertas inteligentes**.

### 🎯 Objetivo

Proporcionar às famílias brasileiras uma ferramenta **profissional, gratuita e offline** para:
- 💰 Controlar gastos mensais com categorização inteligente
- 📊 Visualizar dados financeiros em gráficos interativos
- 📅 Planejar despesas futuras com importação automática
- 💳 Gerenciar cartões de crédito e formas de pagamento
- 🏦 Acompanhar investimentos e evolução patrimonial
- ⚠️ Receber alertas de vencimentos e contas atrasadas

---

## ✨ Funcionalidades

### 🏠 Dashboard Interativo
- 📈 **4 Dashboards Especializados**: Resumo Geral, Despesas, Receitas e Investimentos
- 📊 **Gráficos Dinâmicos**: Linha, pizza e barras com dados em tempo real
- 💡 **Indicadores Inteligentes**: Taxa de comprometimento de renda, evolução patrimonial
- ⚠️ **Alertas Automáticos**: Despesas vencendo nos próximos 7 dias, atrasadas e em dívida
- 🎛️ **Filtros Avançados**: Por mês, ano, tipo, forma de pagamento e status

### 💸 Gestão de Despesas
- ✅ **CRUD Completo**: Criar, editar, excluir e visualizar despesas
- 🔄 **Despesas Fixas**: Importação automática mensal de contas recorrentes
- 💳 **Despesas Parceladas**: Cálculo inteligente de vencimentos baseado no ciclo do cartão
- 🎨 **Status Colorido**: A pagar (azul), Atrasado (amarelo), Em dívida (vermelho), Pago (verde)
- 💰 **Valor Reajustado**: Controle de multas e juros em contas atrasadas
- 🔍 **Filtros Múltiplos**: Mês, ano, tipo, forma de pagamento, status
- ⬆️⬇️ **Ordenação Inteligente**: Por qualquer coluna com indicadores visuais (▲▼)

### 💚 Controle de Receitas
- 📝 **Cadastro Completo**: Data, descrição, valor, tipo e destino
- 📊 **Categorização**: Salário, Freelance, Investimentos, Vendas, Outros
- 🎯 **Destinos**: Conta corrente, poupança, investimento, dinheiro
- 📈 **Estatísticas**: Totais por período, tipo e destino
- 🔍 **Filtros Avançados**: Por mês, ano, tipo e destino

### 📈 Gerenciamento de Investimentos
- 💼 **Tipos Diversos**: Poupança, CDB, LCI/LCA, Tesouro Direto, Ações, Fundos
- 💰 **Controle de Origem**: Rastreamento da fonte dos recursos investidos
- 📊 **Análise**: Totais por tipo, período e fonte
- 📅 **Histórico Completo**: Registro de todas as aplicações com datas e valores

### 💳 Sistema de Cartões
- 🏦 **Cadastro Completo**: Nome, limite, dia de fechamento e vencimento
- 🔄 **Ciclo Inteligente**: Cálculo automático de vencimentos das parcelas
- 💡 **Integração Total**: Com despesas parceladas e formas de pagamento
- ✅ **Validações Rigorosas**: Garantia de dados consistentes

### 💎 Formas de Pagamento
- 🎛️ **Gerenciamento Completo**: Dinheiro, PIX, Boleto, Cartão, Transferência
- 🛡️ **Proteção de Padrões**: Formas essenciais não podem ser excluídas
- 🎨 **Categorias Customizadas**: Personalização para cada forma de pagamento
- 🔗 **Integração Automática**: Sistema inteligente com despesas

---

## 🎨 Interface

### 🖥️ Tela Principal
- 📑 **Navegação por Abas**: Interface organizada e intuitiva
- 🎯 **Botões Padronizados**: Ícones + texto para melhor usabilidade
- 🎨 **Design Responsivo**: Colunas que se ajustam automaticamente
- 🌟 **Experiência Moderna**: Interface limpa e profissional

### 📊 Dashboard com 4 Abas Especializadas

**🏠 Aba Resumo Geral:**
- 💳 Cards: Receita Total, Despesa Total, Investimento Total, Saldo Total
- 📈 Gráficos: Saldo Mensal, Receita Mensal, Despesa Mensal, Investimento Mensal
- ⚠️ Alertas: Despesas vencendo, atrasadas e em dívida
- 💡 Indicadores: % Despesas pagas, Comprometimento da renda, Evolução patrimonial
- 🎛️ Filtros: Ano (para gráficos) e Mês (para indicadores)

**💸 Aba Despesas:**
- 💳 Cards: Despesa Total, Despesa a Pagar, Contas a Pagar, Contas Pagas, Contas Atrasadas, Contas em Dívida
- 📊 Gráficos: Despesas Mensais, Por Tipo, Por Forma de Pagamento, Por Situação
- 🎛️ Filtros: Mês e Ano (gráfico mensal não é afetado)

**💚 Aba Receitas:**
- 💳 Cards: Receita Total, Quantidade de Receitas
- 📈 Gráficos: Receitas Mensais, Por Tipo, Por Destinos
- 🎛️ Filtros: Mês e Ano (gráfico mensal não é afetado)

**📈 Aba Investimentos:**
- 💳 Cards: Investimento Total, Quantidade de Investimentos
- 📊 Gráficos: Investimentos Mensais, Por Tipo, Por Fontes
- 🎛️ Filtros: Mês e Ano (gráfico mensal não é afetado)

---

## 💻 Instalação

### 📋 Requisitos Mínimos

- 🖥️ **Sistema Operacional**: Windows 7 SP1 ou superior (64-bit)
- 🔧 **Processador**: Intel Core i3 ou equivalente
- 🧠 **Memória RAM**: 2 GB
- 💾 **Espaço em Disco**: 100 MB
- 🖼️ **Resolução**: 1280x720

### 📋 Requisitos para Desenvolvimento

- 💻 **MSYS2**: Sistema de build e gerenciador de pacotes
- 🔨 **MinGW-w64 GCC**: Compilador C++17
- 🎨 **wxWidgets 3.3.0**: Framework de interface gráfica
- 🗄️ **SQLite3**: Sistema de banco de dados
- 📝 **Code::Blocks**: IDE (Integrated Development Environment)

### 🛠️ Configuração do Ambiente de Desenvolvimento

```bash
# 1. Instalar MSYS2
# Baixar de: https://www.msys2.org/

# 2. Abrir MSYS2 MinGW 64-bit e executar:
pacman -Syu
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-sqlite3

# 3. Instalar wxWidgets 3.3.0 (Manual)
# Baixar de: https://www.wxwidgets.org/downloads/
# Extrair em C:\wxWidgets-3.3.0 e compilar
# Ou usar os binários pré-compilados
```

### 📦 Compilação

```bash
# 1. Clonar o repositório
git clone https://github.com/Isac-Soares-Barreto/Programacao.git
cd Programacao/Júnior/C++/CodeBlocks/Orçamento-Familiar-Simples

# 2. Abrir no Code::Blocks
# File → Open → FamilyBudgets.cbp

# 3. Compilar
# Build → Build (Ctrl+F9)

# 4. Executar script de deploy
# Navegue até a pasta 'scripts'
cd scripts
deploy.bat

# 5. Executar programa
# O programa com todas as DLLs estará em 'bin\Release' ou 'bin\Debug'
cd ../bin/Release
FamilyBudgets.exe
```

---

## 📱 Como Usar

### 🚀 Primeiros Passos

1. **📦 Execute o programa** pela primeira vez
2. **💳 Configure cartões** (opcional): Menu Cartões → Adicionar
3. **💰 Cadastre despesas fixas** (opcional): Despesas Fixas → Adicionar
4. **💸 Lance suas primeiras despesas**: Despesas → Adicionar

### 💡 Fluxo de Trabalho Recomendado

```
1️⃣ Configure cartões e formas de pagamento
2️⃣ Cadastre despesas fixas mensais
3️⃣ Registre suas receitas do mês
4️⃣ Lance despesas avulsas conforme ocorrem
5️⃣ Cadastre compras parceladas
6️⃣ Importe despesas fixas mensalmente
7️⃣ Importe parcelas de despesas parceladas
8️⃣ Acompanhe pelo Dashboard
```

---

## 🏗️ Tecnologias

### 🔧 Stack Principal
- **💻 Linguagem**: C++17
- **🖼️ Interface Gráfica**: wxWidgets 3.3.0
- **🗄️ Banco de Dados**: SQLite3
- **🔨 IDE**: Code::Blocks
- **⚙️ Compilador**: MinGW-w64 GCC

### 📐 Arquitetura
- **🏛️ Padrão**: MVC (Model-View-Controller)
- **📦 Estrutura**: Modular com separação de responsabilidades
- **🔒 Transações**: ACID garantidas
- **🎨 Interface**: Componentes wxWidgets nativos e customizados

### 🛠️ Componentes Principais
- **📊 ChartUtils**: Sistema próprio de gráficos (linha, pizza, barras)
- **💰 MoneyUtils**: Formatação monetária brasileira (R$)
- **📅 DateUtils**: Manipulação de datas (dd/mm/aaaa)
- **🔢 DecimalTextCtrl**: Controle customizado para valores decimais
- **🧮 InstallmentCalculator**: Cálculos automáticos de parcelas

---

## 📊 Status de Desenvolvimento

| Módulo | Status | Progresso |
|--------|--------|-----------|
| 💸 Despesas | ✅ Completo | 100% |
| 🔄 Despesas Fixas | ✅ Completo | 100% |
| 💳 Despesas Parceladas | ✅ Completo | 100% |
| 💚 Receitas | ✅ Completo | 100% |
| 📈 Investimentos | ✅ Completo | 100% |
| 💳 Cartões | ✅ Completo | 100% |
| 💎 Formas de Pagamento | ✅ Completo | 100% |
| 📊 Dashboard | ✅ Completo | 100% |
| 🗄️ Banco de Dados | ✅ Completo | 100% |
| 🛠️ Utilitários | ✅ Completo | 100% |

**🎉 Versão 1.0 - Sistema 100% Funcional!**

---

## 📁 Estrutura do Projeto

```
FamilyBudgets/
├── 📁 resources/          # Recursos (ícones, banco)
├── 📁 src/                # Código-fonte
│   ├── 📁 app/            # Aplicação principal
│   ├── 📁 panels/         # Painéis da interface
│   ├── 📁 dialogs/        # Diálogos modais
│   ├── 📁 databases/      # DAOs e banco
│   ├── 📁 models/         # Modelos de dados
│   └── 📁 utils/          # Utilitários
├── 📁 docs/               # Documentação
├── 📁 scripts/            # Scripts de deploy
├── FamilyBudgets.cbp      # Projeto Code::Blocks
└── README.md              # Este arquivo
```

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Sinta-se à vontade para:

1. 🍴 Fazer um fork do projeto
2. 🌿 Criar uma branch para sua feature (`git checkout -b feature/NovaFuncionalidade`)
3. 💾 Commit suas mudanças (`git commit -m 'Add: Nova funcionalidade'`)
4. 📤 Push para a branch (`git push origin feature/NovaFuncionalidade`)
5. 🔃 Abrir um Pull Request

---

## 🚀 Roadmap (Próximas Funcionalidades)

O Kaiser Family Budgets está em constante evolução! A versão 1.0 é totalmente funcional, mas já estamos planejando as próximas melhorias para tornar o sistema ainda mais poderoso.

### ⚙️ Módulo de Configurações (Em Desenvolvimento)

A próxima grande atualização será a implementação completa da tela de **Configurações**, permitindo que o usuário personalize sua experiência. As funcionalidades planejadas são:

- 💾 **Backup e Restauração**:
  - Backup automático diário/semanal.
  - Opção para criar e restaurar backups manualmente.
- 🎨 **Temas de Cores**:
  - Temas "Claro" (padrão) e "Escuro" (Dark Mode).
  - Opção para cores personalizadas.
- ⏰ **Alertas Configuráveis**:
  - Definir com quantos dias de antecedência os alertas de vencimento devem aparecer.
- 💰 **Formato de Moeda**:
  - Personalizar o símbolo monetário e o formato de exibição.

### 📝 Outras Melhorias Planejadas

- **Relatórios Avançados**: Geração de relatórios mensais/anuais em PDF e CSV.
- **Importação/Exportação**: Importar extratos bancários (OFX/CSV) e exportar dados.
- **Proteção por Senha**: Opção para proteger o acesso ao programa com uma senha.
- **Gráficos Comparativos**: Comparar despesas/receitas entre diferentes meses ou anos.

**Tem alguma sugestão?** Abra uma [Issue no GitHub](https://github.com/Isac-Soares-Barreto/Programacao/issues) e compartilhe sua ideia!

---

## 🙏 Agradecimentos

O desenvolvimento da interface gráfica e a experiência do usuário do **Kaiser Family Budgets** foram significativamente aprimorados graças aos recursos visuais de alta qualidade fornecidos por comunidades online.

### 🎨 Ícones

Um agradecimento especial ao site **[Icon-Icons](https://icon-icons.com/pt/)**.

Todos os ícones utilizados na interface do programa foram obtidos desta plataforma. Agradecemos imensamente por disponibilizar uma vasta biblioteca de ícones que podem ser utilizados gratuitamente, o que foi essencial para a criação de uma interface amigável, intuitiva e visualmente agradável para este projeto.

---

## 📞 Contato

**Isac Soares Barreto**
- 💼 Engenheiro Mecatrônico | Desenvolvedor de Software | Especialista em Excel
- 📧 Email: isacprofessional@gmail.com
- 💼 LinkedIn: [linkedin.com/in/isac-engenheiro-mecatronico/](https://linkedin.com/in/isac-engenheiro-mecatronico/)
- 🐱 GitHub: [github.com/Isac-Soares-Barreto/](https://github.com/Isac-Soares-Barreto/)

---

## 📄 Licença

Este projeto está sob licença personalizada de uso não comercial. Consulte o arquivo [LICENSE.md](LICENSE.md) para mais detalhes.

**© 2025 Isac Soares Barreto - Todos os direitos reservados**

---

<div align="center">

**⭐ Se este projeto foi útil para você, não esqueça de deixar uma estrela! ⭐**

**🚀 Kaiser Family Budgets - Transformando a gestão financeira familiar! 🚀**

![Desenvolvido com ❤️](https://img.shields.io/badge/Desenvolvido%20com-❤️-red?style=for-the-badge)
![C++](https://img.shields.io/badge/C++-blue?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![wxWidgets](https://img.shields.io/badge/wxWidgets-red?style=for-the-badge)

</div>