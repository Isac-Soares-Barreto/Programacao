-- Schema do banco de dados Kaiser Family Budgets
-- Versão 1.0

-- Tabela de formas de pagamento
CREATE TABLE IF NOT EXISTS payment_methods (
    name TEXT PRIMARY KEY,
    description TEXT
);

-- Inserir formas de pagamento padrão
INSERT OR IGNORE INTO payment_methods (name, description) VALUES 
    ('Dinheiro', 'Pagamento em espécie'),
    ('PIX', 'Transferência instantânea'),
    ('Boleto', 'Boleto bancário'),
    ('Cartão', 'Cartão de crédito/débito'),
    ('Transferência', 'Transferência bancária');

-- Tabela de cartões
CREATE TABLE IF NOT EXISTS cards (
    name TEXT PRIMARY KEY,
    closing_day INTEGER NOT NULL CHECK(closing_day >= 1 AND closing_day <= 31),
    due_day INTEGER NOT NULL CHECK(due_day >= 1 AND due_day <= 31),
    card_limit REAL DEFAULT 0,
    description TEXT
);

-- Tabela de categorias de despesas
CREATE TABLE IF NOT EXISTS expense_categories (
    name TEXT PRIMARY KEY,
    description TEXT
);

-- Inserir categorias padrão
INSERT OR IGNORE INTO expense_categories (name, description) VALUES 
    ('Alimentação', 'Supermercado, restaurantes, lanches'),
    ('Moradia', 'Aluguel, condomínio, IPTU'),
    ('Transporte', 'Combustível, passagens, manutenção'),
    ('Saúde', 'Planos, consultas, medicamentos'),
    ('Educação', 'Mensalidades, cursos, material escolar'),
    ('Lazer', 'Cinema, viagens, entretenimento'),
    ('Vestuário', 'Roupas, calçados, acessórios'),
    ('Serviços', 'Internet, telefone, água, luz'),
    ('Outros', 'Despesas diversas');

-- Tabela de despesas
CREATE TABLE IF NOT EXISTS expenses (
    due_date DATE NOT NULL,
    description TEXT NOT NULL,
    amount REAL NOT NULL CHECK(amount > 0),
    expense_type TEXT,
    payment_method TEXT,
    payment_category TEXT,
    payment_date DATE,
    status TEXT CHECK(status IN ('A pagar', 'Atrasado', 'Em dívida', 'Pago')),
    adjusted_amount REAL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (due_date, description)
);

-- Tabela de despesas fixas
CREATE TABLE IF NOT EXISTS fixed_expenses (
    due_day INTEGER NOT NULL CHECK(due_day >= 1 AND due_day <= 31),
    description TEXT NOT NULL,
    amount REAL NOT NULL CHECK(amount > 0),
    expense_type TEXT,
    payment_method TEXT,
    payment_category TEXT,
    active INTEGER DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (due_day, description)
);

-- Tabela de despesas parceladas
CREATE TABLE IF NOT EXISTS installment_expenses (
    purchase_date DATE NOT NULL,
    description TEXT NOT NULL,
    total_amount REAL NOT NULL CHECK(total_amount > 0),
    installments INTEGER NOT NULL CHECK(installments > 0),
    expense_type TEXT,
    card_name TEXT NOT NULL,
    active INTEGER DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (purchase_date, description, card_name)
);

-- Tabela de categorias de receitas
CREATE TABLE IF NOT EXISTS income_categories (
    name TEXT PRIMARY KEY,
    description TEXT
);

-- Inserir categorias de receita padrão
INSERT OR IGNORE INTO income_categories (name, description) VALUES 
    ('Salário', 'Salário mensal'),
    ('Freelance', 'Trabalhos extras'),
    ('Investimentos', 'Rendimentos de investimentos'),
    ('Vendas', 'Vendas de produtos/serviços'),
    ('Outros', 'Outras receitas');

-- Tabela de destinos de receita
CREATE TABLE IF NOT EXISTS income_destinations (
    name TEXT PRIMARY KEY,
    description TEXT
);

-- Inserir destinos padrão
INSERT OR IGNORE INTO income_destinations (name, description) VALUES 
    ('Conta Corrente', 'Depósito em conta corrente'),
    ('Poupança', 'Depósito em poupança'),
    ('Investimento', 'Aplicação em investimentos'),
    ('Dinheiro', 'Recebimento em espécie');

-- Tabela de receitas
CREATE TABLE IF NOT EXISTS incomes (
    receipt_date DATE NOT NULL,
    description TEXT NOT NULL,
    amount REAL NOT NULL CHECK(amount > 0),
    income_type TEXT,
    destination TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (receipt_date, description)
);

-- Tabela de tipos de investimento
CREATE TABLE IF NOT EXISTS investment_types (
    name TEXT PRIMARY KEY,
    description TEXT
);

-- Inserir tipos de investimento padrão
INSERT OR IGNORE INTO investment_types (name, description) VALUES 
    ('Poupança', 'Caderneta de poupança'),
    ('CDB', 'Certificado de Depósito Bancário'),
    ('LCI/LCA', 'Letra de Crédito Imobiliário/Agronegócio'),
    ('Tesouro Direto', 'Títulos públicos'),
    ('Ações', 'Mercado de ações'),
    ('Fundos', 'Fundos de investimento'),
    ('Outros', 'Outros investimentos');

-- Tabela de investimentos
CREATE TABLE IF NOT EXISTS investments (
    investment_date DATE NOT NULL,
    description TEXT NOT NULL,
    amount REAL NOT NULL CHECK(amount > 0),
    investment_type TEXT,
    source TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (investment_date, description)
);

-- Índices para melhorar performance
CREATE INDEX IF NOT EXISTS idx_expenses_due_date ON expenses(due_date);
CREATE INDEX IF NOT EXISTS idx_expenses_status ON expenses(status);
CREATE INDEX IF NOT EXISTS idx_incomes_date ON incomes(receipt_date);
CREATE INDEX IF NOT EXISTS idx_investments_date ON investments(investment_date);

-- Tabela de categorias customizadas para formas de pagamento
CREATE TABLE IF NOT EXISTS payment_categories (
    payment_method_name TEXT NOT NULL,
    category_name TEXT NOT NULL,
    description TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (payment_method_name, category_name),
    FOREIGN KEY (payment_method_name) REFERENCES payment_methods(name) ON DELETE CASCADE
);

-- Índice para melhorar performance
CREATE INDEX IF NOT EXISTS idx_payment_categories_method ON payment_categories(payment_method_name);