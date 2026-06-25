/*
=============================================================================
  SISTEMA EMPRESARIAL DE CONTROLE DE PRESENCA
  Linguagem  : C++  |  Paradigma: POO
  Plataforma : CxxDroid / CodeBlocks / Dev-C++ / VSCode
  Autor      : Kevin  |  Versao: 1.0
=============================================================================
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <limits>

using namespace std;

// ============================================================
//  UTILITARIOS GLOBAIS
// ============================================================

/** Limpa o ecra (compativel com CxxDroid / Linux / Windows) */
void limparEcra() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/** Pausa ate o utilizador premir Enter */
void pausar() {
    cout << "\n  Pressione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

/** Retorna a data/hora actual no formato DD/MM/AAAA HH:MM */
string dataHoraActual() {
    time_t agora = time(nullptr);
    tm* t = localtime(&agora);
    char buf[20];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", t);
    return string(buf);
}

/** Retorna so a data actual DD/MM/AAAA */
string dataActual() {
    return dataHoraActual().substr(0, 10);
}

/** Retorna so a hora actual HH:MM */
string horaActual() {
    return dataHoraActual().substr(11, 5);
}

/** Converte HH:MM em minutos desde meia-noite */
int horaParaMinutos(const string& hora) {
    if (hora.size() < 5) return 0;
    int h = stoi(hora.substr(0, 2));
    int m = stoi(hora.substr(3, 2));
    return h * 60 + m;
}

/** Verifica se string e numero */
bool ehNumero(const string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit(c)) return false;
    return true;
}

/** Verifica email basico */
bool emailValido(const string& e) {
    size_t a = e.find('@');
    size_t p = e.rfind('.');
    return (a != string::npos && p != string::npos && p > a + 1 && p < e.size() - 1);
}

/** Verifica se nome contem digitos */
bool nomeValido(const string& n) {
    for (char c : n) if (isdigit(c)) return false;
    return !n.empty();
}

/** Linha decorativa */
void linha(char c = '=', int tam = 55) {
    cout << "  ";
    for (int i = 0; i < tam; i++) cout << c;
    cout << "\n";
}

/** Cabecalho do sistema */
void cabecalho(const string& titulo) {
    limparEcra();
    linha('=');
    cout << "  " << setw(55) << left << "" << "\n";
    // Centralizar titulo
    int esp = (55 - (int)titulo.size()) / 2;
    cout << "  " << string(esp, ' ') << titulo << "\n";
    cout << "  " << setw(55) << left << "" << "\n";
    linha('=');
    cout << "\n";
}

// ============================================================
//  CLASSE BASE: PESSOA
// ============================================================
class Pessoa {
protected:
    int    id;
    string nome;
    string email;
    string telefone;

public:
    Pessoa() : id(0) {}
    Pessoa(int id, const string& nome, const string& email, const string& telefone)
        : id(id), nome(nome), email(email), telefone(telefone) {}

    virtual ~Pessoa() {}

    int    getId()       const { return id; }
    string getNome()     const { return nome; }
    string getEmail()    const { return email; }
    string getTelefone() const { return telefone; }

    void setNome(const string& n)     { nome = n; }
    void setEmail(const string& e)    { email = e; }
    void setTelefone(const string& t) { telefone = t; }

    virtual void exibir() const = 0;
};

// ============================================================
//  CLASSE: UTILIZADOR
// ============================================================
class Utilizador : public Pessoa {
protected:
    string username;
    string senha;
    string tipo;   // "admin" ou "funcionario"
    bool   activo;

public:
    Utilizador() : Pessoa(), activo(true) {}
    Utilizador(int id, const string& nome, const string& email,
               const string& telefone, const string& username,
               const string& senha, const string& tipo)
        : Pessoa(id, nome, email, telefone),
          username(username), senha(senha), tipo(tipo), activo(true) {}

    string getUsername() const { return username; }
    string getSenha()    const { return senha; }
    string getTipo()     const { return tipo; }
    bool   isActivo()    const { return activo; }

    void setUsername(const string& u) { username = u; }
    void setSenha(const string& s)    { senha = s; }
    void setActivo(bool a)            { activo = a; }

    bool verificarSenha(const string& s) const { return senha == s; }

    void exibir() const override {
        cout << "  ID: " << id << " | Nome: " << nome
             << " | User: " << username << " | Tipo: " << tipo
             << " | Estado: " << (activo ? "Activo" : "Inactivo") << "\n";
    }

    /** Serializa para linha TXT */
    string serializar() const {
        return to_string(id) + "|" + nome + "|" + email + "|" + telefone + "|" +
               username + "|" + senha + "|" + tipo + "|" + (activo ? "1" : "0");
    }

    /** Desserializa de linha TXT */
    static Utilizador desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 8) return Utilizador();
        Utilizador u;
        u.id       = stoi(campos[0]);
        u.nome     = campos[1];
        u.email    = campos[2];
        u.telefone = campos[3];
        u.username = campos[4];
        u.senha    = campos[5];
        u.tipo     = campos[6];
        u.activo   = (campos[7] == "1");
        return u;
    }
};

// ============================================================
//  CLASSE: FUNCIONARIO (herda Utilizador)
// ============================================================
class Funcionario : public Utilizador {
private:
    string bi;
    string cargo;
    string departamento;
    string dataAdmissao;
    double salarioBase;
    double bonus;
    double aumentos;
    double descontos;

public:
    Funcionario() : Utilizador(), salarioBase(0), bonus(0), aumentos(0), descontos(0) {}

    Funcionario(int id, const string& nome, const string& email,
                const string& telefone, const string& username,
                const string& senha, const string& bi,
                const string& cargo, const string& departamento,
                const string& dataAdmissao, double salarioBase)
        : Utilizador(id, nome, email, telefone, username, senha, "funcionario"),
          bi(bi), cargo(cargo), departamento(departamento),
          dataAdmissao(dataAdmissao), salarioBase(salarioBase),
          bonus(0), aumentos(0), descontos(0) {}

    string getBI()            const { return bi; }
    string getCargo()         const { return cargo; }
    string getDepartamento()  const { return departamento; }
    string getDataAdmissao()  const { return dataAdmissao; }
    double getSalarioBase()   const { return salarioBase; }
    double getBonus()         const { return bonus; }
    double getAumentos()      const { return aumentos; }
    double getDescontos()     const { return descontos; }

    void setCargo(const string& c)        { cargo = c; }
    void setDepartamento(const string& d) { departamento = d; }
    void setSalarioBase(double s)         { salarioBase = s; }
    void adicionarBonus(double b)         { bonus += b; }
    void adicionarAumento(double a)       { salarioBase += a; aumentos += a; }
    void adicionarDesconto(double d)      { descontos += d; }

    double getSalarioFinal() const {
        return salarioBase + bonus + aumentos - descontos;
    }

    void exibir() const override {
        cout << "  \n";
        cout << "   ID: " << setw(3) << id << "  Nome: " << setw(30) << left << nome << "\n";
        cout << "   Cargo: " << setw(20) << cargo << " Depto: " << setw(15) << departamento << "\n";
        cout << "   Salario Base: MZN " << fixed << setprecision(2) << setw(10) << salarioBase
             << "  Estado: " << (activo ? "Activo  " : "Inactivo") << "\n";
        cout << "  \n";
    }

    string serializar() const {
        return to_string(id) + "|" + nome + "|" + email + "|" + telefone + "|" +
               username + "|" + senha + "|" + bi + "|" + cargo + "|" +
               departamento + "|" + dataAdmissao + "|" +
               to_string(salarioBase) + "|" + to_string(bonus) + "|" +
               to_string(aumentos) + "|" + to_string(descontos) + "|" +
               (activo ? "1" : "0");
    }

    static Funcionario desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 15) return Funcionario();
        Funcionario f;
        f.id            = stoi(campos[0]);
        f.nome          = campos[1];
        f.email         = campos[2];
        f.telefone      = campos[3];
        f.username      = campos[4];
        f.senha         = campos[5];
        f.bi            = campos[6];
        f.cargo         = campos[7];
        f.departamento  = campos[8];
        f.dataAdmissao  = campos[9];
        f.salarioBase   = stod(campos[10]);
        f.bonus         = stod(campos[11]);
        f.aumentos      = stod(campos[12]);
        f.descontos     = stod(campos[13]);
        f.activo        = (campos[14] == "1");
        f.tipo          = "funcionario";
        return f;
    }
};

// ============================================================
//  CLASSE: PRESENCA
// ============================================================
class Presenca {
private:
    int    idFuncionario;
    string nomeFuncionario;
    string data;
    string hora;
    string tipo; // ENTRADA, SAIDA, SAIDA_ALMOCO, RETORNO_ALMOCO

public:
    Presenca() : idFuncionario(0) {}
    Presenca(int id, const string& nome, const string& data,
             const string& hora, const string& tipo)
        : idFuncionario(id), nomeFuncionario(nome),
          data(data), hora(hora), tipo(tipo) {}

    int    getIdFuncionario()   const { return idFuncionario; }
    string getNomeFuncionario() const { return nomeFuncionario; }
    string getData()            const { return data; }
    string getHora()            const { return hora; }
    string getTipo()            const { return tipo; }

    void exibir() const {
        cout << "  | " << setw(4) << idFuncionario
             << " | " << setw(22) << left << nomeFuncionario
             << " | " << data << " | " << hora
             << " | " << setw(16) << tipo << " |\n";
    }

    string serializar() const {
        return to_string(idFuncionario) + "|" + nomeFuncionario + "|" +
               data + "|" + hora + "|" + tipo;
    }

    static Presenca desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 5) return Presenca();
        return Presenca(stoi(campos[0]), campos[1], campos[2], campos[3], campos[4]);
    }
};

// ============================================================
//  CLASSE: FALTA
// ============================================================
class Falta {
private:
    int    idFuncionario;
    string nomeFuncionario;
    string data;
    string tipo;       // JUSTIFICADA / NAO_JUSTIFICADA
    string observacao;

public:
    Falta() : idFuncionario(0) {}
    Falta(int id, const string& nome, const string& data,
          const string& tipo, const string& obs = "")
        : idFuncionario(id), nomeFuncionario(nome),
          data(data), tipo(tipo), observacao(obs) {}

    int    getIdFuncionario()   const { return idFuncionario; }
    string getNomeFuncionario() const { return nomeFuncionario; }
    string getData()            const { return data; }
    string getTipo()            const { return tipo; }
    string getObservacao()      const { return observacao; }

    void setTipo(const string& t) { tipo = t; }

    void exibir() const {
        cout << "  | " << setw(4) << idFuncionario
             << " | " << setw(22) << left << nomeFuncionario
             << " | " << data
             << " | " << setw(16) << tipo << " |\n";
    }

    string serializar() const {
        return to_string(idFuncionario) + "|" + nomeFuncionario + "|" +
               data + "|" + tipo + "|" + observacao;
    }

    static Falta desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 4) return Falta();
        string obs = (campos.size() >= 5) ? campos[4] : "";
        return Falta(stoi(campos[0]), campos[1], campos[2], campos[3], obs);
    }
};

// ============================================================
//  CLASSE: JUSTIFICACAO
// ============================================================
class Justificacao {
private:
    int    id;
    int    idFuncionario;
    string nomeFuncionario;
    string dataFalta;
    string motivo;
    string documento;
    string estado; // PENDENTE / APROVADA / REJEITADA
    string dataSubmissao;

public:
    Justificacao() : id(0), idFuncionario(0) {}
    Justificacao(int id, int idFunc, const string& nome,
                 const string& dataFalta, const string& motivo,
                 const string& doc)
        : id(id), idFuncionario(idFunc), nomeFuncionario(nome),
          dataFalta(dataFalta), motivo(motivo), documento(doc),
          estado("PENDENTE"), dataSubmissao(dataActual()) {}

    int    getId()              const { return id; }
    int    getIdFuncionario()   const { return idFuncionario; }
    string getNomeFuncionario() const { return nomeFuncionario; }
    string getDataFalta()       const { return dataFalta; }
    string getMotivo()          const { return motivo; }
    string getDocumento()       const { return documento; }
    string getEstado()          const { return estado; }

    void aprovar()  { estado = "APROVADA"; }
    void rejeitar() { estado = "REJEITADA"; }

    void exibir() const {
        cout << "   Justificacao #" << id << " \n";
        cout << "   Funcionario : " << nomeFuncionario << "\n";
        cout << "   Data Falta  : " << dataFalta << "\n";
        cout << "   Motivo      : " << motivo << "\n";
        cout << "   Documento   : " << documento << "\n";
        cout << "   Estado      : " << estado << "\n";
        cout << "   Submetido   : " << dataSubmissao << "\n";
        cout << "  \n";
    }

    string serializar() const {
        return to_string(id) + "|" + to_string(idFuncionario) + "|" +
               nomeFuncionario + "|" + dataFalta + "|" + motivo + "|" +
               documento + "|" + estado + "|" + dataSubmissao;
    }

    static Justificacao desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 8) return Justificacao();
        Justificacao j;
        j.id              = stoi(campos[0]);
        j.idFuncionario   = stoi(campos[1]);
        j.nomeFuncionario = campos[2];
        j.dataFalta       = campos[3];
        j.motivo          = campos[4];
        j.documento       = campos[5];
        j.estado          = campos[6];
        j.dataSubmissao   = campos[7];
        return j;
    }
};

// ============================================================
//  CLASSE: HORA EXTRA
// ============================================================
class HoraExtra {
private:
    int    idFuncionario;
    string nomeFuncionario;
    string data;
    double horas;   // em horas decimais
    string observacao;

public:
    HoraExtra() : idFuncionario(0), horas(0) {}
    HoraExtra(int id, const string& nome, const string& data, double horas, const string& obs = "")
        : idFuncionario(id), nomeFuncionario(nome),
          data(data), horas(horas), observacao(obs) {}

    int    getIdFuncionario()   const { return idFuncionario; }
    string getNomeFuncionario() const { return nomeFuncionario; }
    string getData()            const { return data; }
    double getHoras()           const { return horas; }

    void exibir() const {
        cout << "  | " << setw(4) << idFuncionario
             << " | " << setw(22) << left << nomeFuncionario
             << " | " << data
             << " | " << fixed << setprecision(1) << setw(5) << horas << " h |\n";
    }

    string serializar() const {
        return to_string(idFuncionario) + "|" + nomeFuncionario + "|" +
               data + "|" + to_string(horas) + "|" + observacao;
    }

    static HoraExtra desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 4) return HoraExtra();
        string obs = (campos.size() >= 5) ? campos[4] : "";
        return HoraExtra(stoi(campos[0]), campos[1], campos[2], stod(campos[3]), obs);
    }
};

// ============================================================
//  CLASSE: FERIAS
// ============================================================
class Ferias {
private:
    int    id;
    int    idFuncionario;
    string nomeFuncionario;
    string dataInicio;
    string dataFim;
    int    dias;
    string estado; // PENDENTE / APROVADA / REJEITADA
    string dataPedido;

public:
    Ferias() : id(0), idFuncionario(0), dias(0) {}
    Ferias(int id, int idFunc, const string& nome,
           const string& ini, const string& fim, int dias)
        : id(id), idFuncionario(idFunc), nomeFuncionario(nome),
          dataInicio(ini), dataFim(fim), dias(dias),
          estado("PENDENTE"), dataPedido(dataActual()) {}

    int    getId()              const { return id; }
    int    getIdFuncionario()   const { return idFuncionario; }
    string getNomeFuncionario() const { return nomeFuncionario; }
    string getEstado()          const { return estado; }
    string getDataInicio()      const { return dataInicio; }
    string getDataFim()         const { return dataFim; }
    int    getDias()            const { return dias; }

    void aprovar()  { estado = "APROVADA"; }
    void rejeitar() { estado = "REJEITADA"; }

    void exibir() const {
        cout << "   Pedido de Ferias #" << id << " \n";
        cout << "   Funcionario : " << nomeFuncionario << "\n";
        cout << "   Periodo     : " << dataInicio << " a " << dataFim << "\n";
        cout << "   Dias        : " << dias << "\n";
        cout << "   Estado      : " << estado << "\n";
        cout << "   Pedido em   : " << dataPedido << "\n";
        cout << "  \n";
    }

    string serializar() const {
        return to_string(id) + "|" + to_string(idFuncionario) + "|" +
               nomeFuncionario + "|" + dataInicio + "|" + dataFim + "|" +
               to_string(dias) + "|" + estado + "|" + dataPedido;
    }

    static Ferias desserializar(const string& linha) {
        stringstream ss(linha);
        string tok;
        vector<string> campos;
        while (getline(ss, tok, '|')) campos.push_back(tok);
        if (campos.size() < 8) return Ferias();
        Ferias f;
        f.id              = stoi(campos[0]);
        f.idFuncionario   = stoi(campos[1]);
        f.nomeFuncionario = campos[2];
        f.dataInicio      = campos[3];
        f.dataFim         = campos[4];
        f.dias            = stoi(campos[5]);
        f.estado          = campos[6];
        f.dataPedido      = campos[7];
        return f;
    }
};

// ============================================================
//  CLASSE: RELATORIO (polimorfismo)
// ============================================================
class Relatorio {
protected:
    string titulo;
    string dataGeracao;

public:
    Relatorio(const string& t) : titulo(t), dataGeracao(dataHoraActual()) {}
    virtual ~Relatorio() {}
    virtual void gerar() const = 0;

    void cabecalhoRelatorio() const {
        linha('=');
        int esp = (53 - (int)titulo.size()) / 2;
        cout << "  " << string(esp, ' ') << titulo << "\n";
        cout << "  Gerado em: " << dataGeracao << "\n";
        linha('=');
    }
};

// ============================================================
//  GESTAO DE FICHEIROS (funcoes auxiliares)
// ============================================================

/** Le todas as linhas de um ficheiro */
vector<string> lerFicheiro(const string& nome) {
    vector<string> linhas;
    ifstream f(nome);
    if (!f.is_open()) return linhas;
    string linha;
    while (getline(f, linha)) {
        if (!linha.empty()) linhas.push_back(linha);
    }
    return linhas;
}

/** Sobrescreve um ficheiro com um vector de linhas */
void gravarFicheiro(const string& nome, const vector<string>& linhas) {
    ofstream f(nome);
    for (const auto& l : linhas) f << l << "\n";
}

/** Acrescenta uma linha a um ficheiro */
void acrescentarFicheiro(const string& nome, const string& linha) {
    ofstream f(nome, ios::app);
    f << linha << "\n";
}

// ============================================================
//  CLASSE: SISTEMA  (controlador principal)
// ============================================================
class Sistema {
private:
    // Dados em memoria
    vector<Utilizador>  admins;
    vector<Funcionario> funcionarios;
    vector<Presenca>    presencas;
    vector<Falta>       faltas;
    vector<Justificacao>justificacoes;
    vector<HoraExtra>   horasExtras;
    vector<Ferias>      listaFerias;

    // Utilizador actual
    Utilizador* utilizadorActual = nullptr;
    bool        isAdmin          = false;

    // Ficheiros
    const string ARQ_USUARIOS      = "usuarios.txt";
    const string ARQ_FUNCIONARIOS  = "funcionarios.txt";
    const string ARQ_PRESENCAS     = "presencas.txt";
    const string ARQ_FALTAS        = "faltas.txt";
    const string ARQ_JUSTIFICACOES = "justificacoes.txt";
    const string ARQ_HORAS_EXTRAS  = "horas_extras.txt";
    const string ARQ_FERIAS        = "ferias.txt";

    // Horario padrao (minutos)
    const int HORA_ENTRADA = 8 * 60;   // 08:00
    const int HORA_SAIDA   = 17 * 60;  // 17:00

    //  Proximos IDs 
    int proximoIdFuncionario() {
        int max = 0;
        for (auto& f : funcionarios) if (f.getId() > max) max = f.getId();
        return max + 1;
    }
    int proximoIdJustificacao() {
        int max = 0;
        for (auto& j : justificacoes) if (j.getId() > max) max = j.getId();
        return max + 1;
    }
    int proximoIdFerias() {
        int max = 0;
        for (auto& f : listaFerias) if (f.getId() > max) max = f.getId();
        return max + 1;
    }

    //  CARREGAMENTO 
    void carregarAdmins() {
        admins.clear();
        for (auto& l : lerFicheiro(ARQ_USUARIOS))
            admins.push_back(Utilizador::desserializar(l));
    }

    void carregarFuncionarios() {
        funcionarios.clear();
        for (auto& l : lerFicheiro(ARQ_FUNCIONARIOS))
            funcionarios.push_back(Funcionario::desserializar(l));
    }

    void carregarPresencas() {
        presencas.clear();
        for (auto& l : lerFicheiro(ARQ_PRESENCAS))
            presencas.push_back(Presenca::desserializar(l));
    }

    void carregarFaltas() {
        faltas.clear();
        for (auto& l : lerFicheiro(ARQ_FALTAS))
            faltas.push_back(Falta::desserializar(l));
    }

    void carregarJustificacoes() {
        justificacoes.clear();
        for (auto& l : lerFicheiro(ARQ_JUSTIFICACOES))
            justificacoes.push_back(Justificacao::desserializar(l));
    }

    void carregarHorasExtras() {
        horasExtras.clear();
        for (auto& l : lerFicheiro(ARQ_HORAS_EXTRAS))
            horasExtras.push_back(HoraExtra::desserializar(l));
    }

    void carregarFerias() {
        listaFerias.clear();
        for (auto& l : lerFicheiro(ARQ_FERIAS))
            listaFerias.push_back(Ferias::desserializar(l));
    }

    void carregarTudo() {
        carregarAdmins();
        carregarFuncionarios();
        carregarPresencas();
        carregarFaltas();
        carregarJustificacoes();
        carregarHorasExtras();
        carregarFerias();
    }

    //  GRAVACAO 
    void gravarAdmins() {
        vector<string> linhas;
        for (auto& a : admins) linhas.push_back(a.serializar());
        gravarFicheiro(ARQ_USUARIOS, linhas);
    }

    void gravarFuncionarios() {
        vector<string> linhas;
        for (auto& f : funcionarios) linhas.push_back(f.serializar());
        gravarFicheiro(ARQ_FUNCIONARIOS, linhas);
    }

    void gravarPresencas() {
        vector<string> linhas;
        for (auto& p : presencas) linhas.push_back(p.serializar());
        gravarFicheiro(ARQ_PRESENCAS, linhas);
    }

    void gravarFaltas() {
        vector<string> linhas;
        for (auto& f : faltas) linhas.push_back(f.serializar());
        gravarFicheiro(ARQ_FALTAS, linhas);
    }

    void gravarJustificacoes() {
        vector<string> linhas;
        for (auto& j : justificacoes) linhas.push_back(j.serializar());
        gravarFicheiro(ARQ_JUSTIFICACOES, linhas);
    }

    void gravarHorasExtras() {
        vector<string> linhas;
        for (auto& h : horasExtras) linhas.push_back(h.serializar());
        gravarFicheiro(ARQ_HORAS_EXTRAS, linhas);
    }

    void gravarFerias() {
        vector<string> linhas;
        for (auto& f : listaFerias) linhas.push_back(f.serializar());
        gravarFicheiro(ARQ_FERIAS, linhas);
    }

    //  INICIALIZACAO 
    void inicializarSistema() {
        // Criar admin padrao se nao existir
        ifstream f(ARQ_USUARIOS);
        if (!f.good()) {
            Utilizador admin(1, "Administrador", "admin@empresa.mz", "840000000",
                             "admin", "admin123", "admin");
            acrescentarFicheiro(ARQ_USUARIOS, admin.serializar());
        }
    }

    //  BUSCA 
    Funcionario* buscarFuncionarioPorId(int id) {
        for (auto& f : funcionarios) if (f.getId() == id) return &f;
        return nullptr;
    }

    Funcionario* buscarFuncionarioPorUsername(const string& user) {
        for (auto& f : funcionarios) if (f.getUsername() == user) return &f;
        return nullptr;
    }

    bool usernameExiste(const string& user) {
        for (auto& f : funcionarios) if (f.getUsername() == user) return true;
        for (auto& a : admins) if (a.getUsername() == user) return true;
        return false;
    }

    bool biExiste(const string& bi) {
        for (auto& f : funcionarios) if (f.getBI() == bi) return true;
        return false;
    }

    //  ULTIMO REGISTO DE PRESENCA DO FUNCIONARIO 
    string ultimoTipoPresenca(int idFunc, const string& data) {
        string ultimo = "";
        for (auto& p : presencas) {
            if (p.getIdFuncionario() == idFunc && p.getData() == data)
                ultimo = p.getTipo();
        }
        return ultimo;
    }

    //  CALCULAR HORAS EXTRAS 
    void calcularHorasExtras(int idFunc, const string& nome,
                             const string& data, const string& horaEntrada,
                             const string& horaSaida) {
        int minEntrada = horaParaMinutos(horaEntrada);
        int minSaida   = horaParaMinutos(horaSaida);
        int trabalhados = minSaida - minEntrada;
        int padrao      = HORA_SAIDA - HORA_ENTRADA;

        if (trabalhados > padrao) {
            double extras = (trabalhados - padrao) / 60.0;
            // Verificar se ja existe para esse dia
            for (auto& h : horasExtras) {
                if (h.getIdFuncionario() == idFunc && h.getData() == data)
                    return; // Ja registado
            }
            HoraExtra he(idFunc, nome, data, extras);
            horasExtras.push_back(he);
            gravarHorasExtras();
        }
    }

    //  VERIFICAR FALTA 
    bool temPresenca(int idFunc, const string& data) {
        for (auto& p : presencas)
            if (p.getIdFuncionario() == idFunc && p.getData() == data
                && p.getTipo() == "ENTRADA")
                return true;
        return false;
    }

    bool temFaltaRegistada(int idFunc, const string& data) {
        for (auto& f : faltas)
            if (f.getIdFuncionario() == idFunc && f.getData() == data)
                return true;
        return false;
    }

    //  LOGIN 
    bool fazerLogin() {
        cabecalho("  LOGIN  -  SISTEMA DE PRESENCA");
        string user, senha;
        int tentativas = 0;

        while (tentativas < 3) {
            cout << "  Utilizador : "; getline(cin, user);
            cout << "  Palavra-passe : "; getline(cin, senha);

            if (user.empty() || senha.empty()) {
                cout << "\n    Campos obrigatorios em branco!\n\n";
                tentativas++;
                continue;
            }

            // Verificar admins
            for (auto& a : admins) {
                if (a.getUsername() == user) {
                    if (!a.isActivo()) {
                        cout << "\n    Conta inactiva!\n";
                        pausar();
                        return false;
                    }
                    if (a.verificarSenha(senha)) {
                        utilizadorActual = &a;
                        isAdmin = true;
                        cout << "\n    Login efectuado! Bem-vindo, " << a.getNome() << "\n";
                        pausar();
                        return true;
                    }
                }
            }

            // Verificar funcionarios
            for (auto& f : funcionarios) {
                if (f.getUsername() == user) {
                    if (!f.isActivo()) {
                        cout << "\n    Conta inactiva!\n";
                        pausar();
                        return false;
                    }
                    if (f.verificarSenha(senha)) {
                        utilizadorActual = &f;
                        isAdmin = false;
                        cout << "\n    Login efectuado! Bem-vindo, " << f.getNome() << "\n";
                        pausar();
                        return true;
                    }
                }
            }

            tentativas++;
            cout << "\n    Credenciais invalidas! Tentativa " << tentativas << "/3\n\n";
        }

        cout << "\n    Limite de tentativas excedido!\n";
        pausar();
        return false;
    }

    // 
    //  MENUS DE ADMINISTRADOR
    // 

    //  CADASTRAR FUNCIONARIO 
    void cadastrarFuncionario() {
        cabecalho("CADASTRAR FUNCIONARIO");
        Funcionario f;
        string nome, email, tel, user, senha, bi, cargo, depto, admissao;
        double salario;

        cout << "  Nome completo    : "; getline(cin, nome);
        if (!nomeValido(nome)) {
            cout << "\n    Nome invalido (nao pode conter numeros)!\n";
            pausar(); return;
        }

        cout << "  BI / Documento   : "; getline(cin, bi);
        if (bi.empty()) { cout << "\n    BI obrigatorio!\n"; pausar(); return; }
        if (biExiste(bi)) { cout << "\n    BI ja registado!\n"; pausar(); return; }

        cout << "  Nome de utilizador: "; getline(cin, user);
        if (usernameExiste(user)) {
            cout << "\n    Nome de utilizador ja existe!\n";
            pausar(); return;
        }

        cout << "  Palavra-passe    : "; getline(cin, senha);
        if (senha.empty()) { cout << "\n    Senha obrigatoria!\n"; pausar(); return; }

        cout << "  Cargo            : "; getline(cin, cargo);
        cout << "  Departamento     : "; getline(cin, depto);
        cout << "  Telefone         : "; getline(cin, tel);
        if (!ehNumero(tel)) {
            cout << "\n    Telefone invalido (so numeros)!\n";
            pausar(); return;
        }

        cout << "  Email            : "; getline(cin, email);
        if (!emailValido(email)) {
            cout << "\n    Email invalido!\n";
            pausar(); return;
        }

        cout << "  Data de admissao (DD/MM/AAAA): "; getline(cin, admissao);

        cout << "  Salario Base (MZN): "; string sal; getline(cin, sal);
        try { salario = stod(sal); } catch (...) { salario = 0; }
        if (salario <= 0) {
            cout << "\n    Salario deve ser maior que zero!\n";
            pausar(); return;
        }

        int novoId = proximoIdFuncionario();
        Funcionario novo(novoId, nome, email, tel, user, senha,
                         bi, cargo, depto, admissao, salario);
        funcionarios.push_back(novo);
        gravarFuncionarios();

        cout << "\n    Funcionario cadastrado com sucesso! ID: " << novoId << "\n";
        pausar();
    }

    //  LISTAR FUNCIONARIOS 
    void listarFuncionarios() {
        cabecalho("LISTA DE FUNCIONARIOS");
        if (funcionarios.empty()) {
            cout << "  Nenhum funcionario cadastrado.\n";
            pausar(); return;
        }
        for (auto& f : funcionarios) f.exibir();
        cout << "\n  Total: " << funcionarios.size() << " funcionario(s)\n";
        pausar();
    }

    //  EDITAR FUNCIONARIO 
    void editarFuncionario() {
        cabecalho("EDITAR FUNCIONARIO");
        cout << "  ID do funcionario: "; string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }

        Funcionario* f = buscarFuncionarioPorId(id);
        if (!f) { cout << "\n    Funcionario nao encontrado!\n"; pausar(); return; }

        cout << "\n  Funcionario: " << f->getNome() << "\n";
        cout << "  Cargo actual: " << f->getCargo() << "\n";
        cout << "  Novo cargo (deixe vazio para manter): "; string cargo; getline(cin, cargo);
        if (!cargo.empty()) f->setCargo(cargo);

        cout << "  Novo departamento (deixe vazio para manter): "; string depto; getline(cin, depto);
        if (!depto.empty()) f->setDepartamento(depto);

        cout << "  Novo email (deixe vazio para manter): "; string email; getline(cin, email);
        if (!email.empty() && emailValido(email)) f->setEmail(email);

        cout << "  Novo telefone (deixe vazio para manter): "; string tel; getline(cin, tel);
        if (!tel.empty() && ehNumero(tel)) f->setTelefone(tel);

        gravarFuncionarios();
        cout << "\n    Funcionario actualizado!\n";
        pausar();
    }

    //  ACTIVAR / DESACTIVAR FUNCIONARIO 
    void alterarEstadoFuncionario(bool activar) {
        string acao = activar ? "ACTIVAR" : "DESACTIVAR";
        cabecalho(acao + " FUNCIONARIO");
        cout << "  ID do funcionario: "; string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }

        Funcionario* f = buscarFuncionarioPorId(id);
        if (!f) { cout << "\n    Funcionario nao encontrado!\n"; pausar(); return; }

        f->setActivo(activar);
        gravarFuncionarios();
        cout << "\n    Funcionario " << (activar ? "activado" : "desactivado") << "!\n";
        pausar();
    }

    //  REMOVER FUNCIONARIO 
    void removerFuncionario() {
        cabecalho("REMOVER FUNCIONARIO");
        cout << "  ID do funcionario: "; string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }

        auto it = funcionarios.begin();
        while (it != funcionarios.end()) {
            if (it->getId() == id) {
                cout << "\n  Confirmar remocao de " << it->getNome() << "? (S/N): ";
                string conf; getline(cin, conf);
                if (conf == "S" || conf == "s") {
                    it = funcionarios.erase(it);
                    gravarFuncionarios();
                    cout << "\n    Funcionario removido!\n";
                } else {
                    cout << "\n  Operacao cancelada.\n";
                }
                pausar(); return;
            } else ++it;
        }
        cout << "\n    Funcionario nao encontrado!\n";
        pausar();
    }

    //  PESQUISAR FUNCIONARIO 
    void pesquisarFuncionario() {
        cabecalho("PESQUISAR FUNCIONARIO");
        cout << "  1. Por ID\n  2. Por Nome\n  3. Por Cargo\n  4. Por Departamento\n";
        cout << "  Opcao: "; string op; getline(cin, op);
        cout << "  Termo: "; string termo; getline(cin, termo);

        bool encontrou = false;
        for (auto& f : funcionarios) {
            bool match = false;
            if (op == "1") match = (to_string(f.getId()) == termo);
            else if (op == "2") match = (f.getNome().find(termo) != string::npos);
            else if (op == "3") match = (f.getCargo().find(termo) != string::npos);
            else if (op == "4") match = (f.getDepartamento().find(termo) != string::npos);
            if (match) { f.exibir(); encontrou = true; }
        }
        if (!encontrou) cout << "\n  Nenhum resultado encontrado.\n";
        pausar();
    }

    //  REGISTAR PRESENCA (ADMIN MANUAL) 
    void registarPresencaAdmin() {
        cabecalho("REGISTAR PRESENCA (ADMIN)");
        cout << "  ID do funcionario: "; string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }

        Funcionario* f = buscarFuncionarioPorId(id);
        if (!f) { cout << "\n    Funcionario nao encontrado!\n"; pausar(); return; }
        if (!f->isActivo()) { cout << "\n    Funcionario inactivo!\n"; pausar(); return; }

        cout << "  Tipo (ENTRADA/SAIDA/SAIDA_ALMOCO/RETORNO_ALMOCO): ";
        string tipo; getline(cin, tipo);
        cout << "  Data (DD/MM/AAAA): "; string data; getline(cin, data);
        cout << "  Hora (HH:MM)     : "; string hora; getline(cin, hora);

        Presenca p(id, f->getNome(), data, hora, tipo);
        presencas.push_back(p);
        gravarPresencas();
        cout << "\n    Presenca registada!\n";
        pausar();
    }

    //  VER TODAS AS PRESENCAS 
    void verTodasPresencas() {
        cabecalho("PRESENCAS");
        cout << "  Filtrar por data (DD/MM/AAAA) ou deixe vazio para todas: ";
        string filtro; getline(cin, filtro);

        linha('-');
        cout << "  | ID   | Nome                   | Data       | Hora  | Tipo             |\n";
        linha('-');
        bool achou = false;
        for (auto& p : presencas) {
            if (filtro.empty() || p.getData() == filtro) {
                p.exibir();
                achou = true;
            }
        }
        if (!achou) cout << "\n  Nenhum registo encontrado.\n";
        linha('-');
        pausar();
    }

    //  GERIR JUSTIFICACOES 
    void gerirJustificacoes() {
        cabecalho("JUSTIFICACOES PENDENTES");
        bool achou = false;
        for (auto& j : justificacoes) {
            if (j.getEstado() == "PENDENTE") { j.exibir(); achou = true; }
        }
        if (!achou) { cout << "\n  Nenhuma justificacao pendente.\n"; pausar(); return; }

        cout << "\n  ID da justificacao para avaliar (0=cancelar): ";
        string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }
        if (id == 0) { pausar(); return; }

        for (auto& j : justificacoes) {
            if (j.getId() == id && j.getEstado() == "PENDENTE") {
                cout << "  1. Aprovar   2. Rejeitar: ";
                string op; getline(cin, op);
                if (op == "1") {
                    j.aprovar();
                    // Actualizar a falta correspondente
                    for (auto& f : faltas) {
                        if (f.getIdFuncionario() == j.getIdFuncionario() &&
                            f.getData() == j.getDataFalta())
                            f.setTipo("JUSTIFICADA");
                    }
                    gravarFaltas();
                    cout << "\n    Justificacao aprovada!\n";
                } else {
                    j.rejeitar();
                    cout << "\n    Justificacao rejeitada!\n";
                }
                gravarJustificacoes();
                pausar(); return;
            }
        }
        cout << "\n    ID nao encontrado!\n";
        pausar();
    }

    //  GERIR FERIAS 
    void gerirFerias() {
        cabecalho("PEDIDOS DE FERIAS");
        bool achou = false;
        for (auto& f : listaFerias) {
            if (f.getEstado() == "PENDENTE") { f.exibir(); achou = true; }
        }
        if (!achou) { cout << "\n  Nenhum pedido pendente.\n"; pausar(); return; }

        cout << "\n  ID do pedido para avaliar (0=cancelar): ";
        string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }
        if (id == 0) { pausar(); return; }

        for (auto& f : listaFerias) {
            if (f.getId() == id && f.getEstado() == "PENDENTE") {
                cout << "  1. Aprovar   2. Rejeitar: ";
                string op; getline(cin, op);
                if (op == "1") { f.aprovar(); cout << "\n    Ferias aprovadas!\n"; }
                else           { f.rejeitar(); cout << "\n    Ferias rejeitadas!\n"; }
                gravarFerias();
                pausar(); return;
            }
        }
        cout << "\n    ID nao encontrado!\n";
        pausar();
    }

    //  GERIR SALARIOS 
    void gerirSalarios() {
        cabecalho("GESTAO SALARIAL");
        cout << "  ID do funcionario: "; string sid; getline(cin, sid);
        int id; try { id = stoi(sid); } catch (...) { pausar(); return; }
        Funcionario* f = buscarFuncionarioPorId(id);
        if (!f) { cout << "\n    Nao encontrado!\n"; pausar(); return; }

        cout << "\n  Funcionario: " << f->getNome() << "\n";
        cout << "  Salario Base: MZN " << fixed << setprecision(2) << f->getSalarioBase() << "\n";
        cout << "  Bonus: MZN " << f->getBonus() << "\n";
        cout << "  Descontos: MZN " << f->getDescontos() << "\n";
        cout << "  SALARIO FINAL: MZN " << f->getSalarioFinal() << "\n\n";

        cout << "  1. Aplicar Bonus\n  2. Aplicar Desconto\n  3. Aplicar Aumento\n  0. Voltar\n";
        cout << "  Opcao: "; string op; getline(cin, op);

        if (op == "1" || op == "2" || op == "3") {
            cout << "  Valor (MZN): "; string sval; getline(cin, sval);
            double val; try { val = stod(sval); } catch (...) { val = 0; }
            if (val <= 0) { cout << "\n    Valor invalido!\n"; pausar(); return; }
            if (op == "1") { f->adicionarBonus(val);    cout << "\n    Bonus aplicado!\n"; }
            if (op == "2") { f->adicionarDesconto(val); cout << "\n    Desconto aplicado!\n"; }
            if (op == "3") { f->adicionarAumento(val);  cout << "\n    Aumento aplicado!\n"; }
            gravarFuncionarios();
        }
        pausar();
    }

    //  ALTERAR QUALQUER SENHA (ADMIN) 
    void alterarSenhaAdmin() {
        cabecalho("ALTERAR SENHA");
        cout << "  1. Alterar senha de administrador\n  2. Alterar senha de funcionario\n";
        cout << "  Opcao: "; string op; getline(cin, op);

        if (op == "1") {
            cout << "  Username do admin: "; string u; getline(cin, u);
            for (auto& a : admins) {
                if (a.getUsername() == u) {
                    cout << "  Nova senha: "; string ns; getline(cin, ns);
                    a.setSenha(ns); gravarAdmins();
                    cout << "\n    Senha alterada!\n"; pausar(); return;
                }
            }
            cout << "\n    Admin nao encontrado!\n";
        } else {
            cout << "  ID do funcionario: "; string sid; getline(cin, sid);
            int id; try { id = stoi(sid); } catch (...) { pausar(); return; }
            Funcionario* f = buscarFuncionarioPorId(id);
            if (!f) { cout << "\n    Nao encontrado!\n"; pausar(); return; }
            cout << "  Nova senha: "; string ns; getline(cin, ns);
            f->setSenha(ns); gravarFuncionarios();
            cout << "\n    Senha alterada!\n";
        }
        pausar();
    }

    //  RELATORIOS 
    void menuRelatorios() {
        string op;
        do {
            cabecalho("RELATORIOS");
            cout << "  1. Relatorio de Funcionarios\n";
            cout << "  2. Relatorio de Presencas\n";
            cout << "  3. Relatorio de Faltas\n";
            cout << "  4. Relatorio de Horas Extras\n";
            cout << "  5. Relatorio Salarial\n";
            cout << "  6. Relatorio de Ferias\n";
            cout << "  0. Voltar\n\n";
            cout << "  Opcao: "; getline(cin, op);

            if (op == "1") relatorioFuncionarios();
            else if (op == "2") relatorioPresencas();
            else if (op == "3") relatorioFaltas();
            else if (op == "4") relatorioHorasExtras();
            else if (op == "5") relatorioSalarial();
            else if (op == "6") relatorioFerias();
        } while (op != "0");
    }

    void relatorioFuncionarios() {
        cabecalho("RELATORIO DE FUNCIONARIOS");
        int activos = 0, inativos = 0;
        for (auto& f : funcionarios) {
            f.exibir();
            if (f.isActivo()) activos++; else inativos++;
        }
        linha('-');
        cout << "  Total: " << funcionarios.size()
             << " | Activos: " << activos << " | Inactivos: " << inativos << "\n";
        pausar();
    }

    void relatorioPresencas() {
        cabecalho("RELATORIO DE PRESENCAS");
        cout << "  Filtrar por mes (MM/AAAA) ou vazio para todas: ";
        string filtro; getline(cin, filtro);
        linha('-');
        cout << "  | ID   | Nome                   | Data       | Hora  | Tipo             |\n";
        linha('-');
        for (auto& p : presencas) {
            if (filtro.empty() || p.getData().substr(3) == filtro)
                p.exibir();
        }
        linha('-');
        pausar();
    }

    void relatorioFaltas() {
        cabecalho("RELATORIO DE FALTAS");
        cout << "  Filtrar por funcionario (ID) ou vazio para todos: ";
        string filtro; getline(cin, filtro);
        linha('-');
        for (auto& f : faltas) {
            if (filtro.empty() || to_string(f.getIdFuncionario()) == filtro)
                f.exibir();
        }
        linha('-');
        pausar();
    }

    void relatorioHorasExtras() {
        cabecalho("RELATORIO DE HORAS EXTRAS");
        double total = 0;
        linha('-');
        cout << "  | ID   | Nome                   | Data       | Horas  |\n";
        linha('-');
        for (auto& h : horasExtras) { h.exibir(); total += h.getHoras(); }
        linha('-');
        cout << "  Total de horas extras: " << fixed << setprecision(1) << total << " h\n";
        pausar();
    }

    void relatorioSalarial() {
        cabecalho("RELATORIO SALARIAL");
        double totalSalarios = 0;
        linha('-');
        cout << fixed << setprecision(2);
        for (auto& f : funcionarios) {
            cout << "  " << setw(20) << left << f.getNome()
                 << " | Base: " << setw(10) << f.getSalarioBase()
                 << " | Final: " << f.getSalarioFinal() << " MZN\n";
            totalSalarios += f.getSalarioFinal();
        }
        linha('-');
        cout << "  TOTAL DA FOLHA SALARIAL: MZN " << totalSalarios << "\n";
        pausar();
    }

    void relatorioFerias() {
        cabecalho("RELATORIO DE FERIAS");
        for (auto& f : listaFerias) f.exibir();
        if (listaFerias.empty()) cout << "\n  Nenhum pedido de ferias registado.\n";
        pausar();
    }

    //  DASHBOARD 
    void dashboard() {
        cabecalho("DASHBOARD EMPRESARIAL");
        string hoje = dataActual();

        int totalFunc = (int)funcionarios.size();
        int activos   = 0, presentesHoje = 0, faltasHoje = 0;
        int pendJust  = 0, pendFerias = 0;
        double totalHE = 0;

        for (auto& f : funcionarios) {
            if (f.isActivo()) activos++;
            if (temPresenca(f.getId(), hoje)) presentesHoje++;
            if (temFaltaRegistada(f.getId(), hoje)) faltasHoje++;
        }
        for (auto& j : justificacoes) if (j.getEstado() == "PENDENTE") pendJust++;
        for (auto& f : listaFerias)   if (f.getEstado() == "PENDENTE") pendFerias++;
        for (auto& h : horasExtras)   totalHE += h.getHoras();

        cout << "  Data : " << hoje << "\n\n";
        linha('-');
        cout << "    Total de Funcionarios    : " << totalFunc << "\n";
        cout << "    Funcionarios Activos     : " << activos << "\n";
        cout << "    Presentes Hoje           : " << presentesHoje << "\n";
        cout << "    Ausentes Hoje            : " << (activos - presentesHoje) << "\n";
        cout << "    Faltas Hoje              : " << faltasHoje << "\n";
        cout << "    Total Horas Extras       : " << fixed << setprecision(1) << totalHE << " h\n";
        cout << "    Justificacoes Pendentes  : " << pendJust << "\n";
        cout << "    Pedidos Ferias Pendentes : " << pendFerias << "\n";
        linha('-');

        // Alertas
        cout << "\n   ALERTAS \n";
        if (pendJust > 0)
            cout << "    " << pendJust << " justificacao(oes) aguardam aprovacao!\n";
        if (pendFerias > 0)
            cout << "    " << pendFerias << " pedido(s) de ferias aguardam decisao!\n";
        if (pendJust == 0 && pendFerias == 0)
            cout << "    Sem alertas pendentes.\n";

        pausar();
    }

    // 
    //  MENUS DE FUNCIONARIO
    // 

    void registarPresencaFuncionario() {
        cabecalho("REGISTAR PRESENCA");

        // Obter funcionario actual
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { cout << "\n    Erro interno!\n"; pausar(); return; }
        if (!f->isActivo()) { cout << "\n    Conta inactiva!\n"; pausar(); return; }

        string hoje  = dataActual();
        string agora = horaActual();
        string ultimo = ultimoTipoPresenca(f->getId(), hoje);

        cout << "  Funcionario : " << f->getNome() << "\n";
        cout << "  Data/Hora   : " << hoje << " " << agora << "\n";
        cout << "  Ultimo registo hoje: " << (ultimo.empty() ? "Nenhum" : ultimo) << "\n\n";

        cout << "  1. Registar Entrada\n";
        cout << "  2. Registar Saida para Almoco\n";
        cout << "  3. Registar Retorno do Almoco\n";
        cout << "  4. Registar Saida\n";
        cout << "  0. Cancelar\n\n";
        cout << "  Opcao: "; string op; getline(cin, op);

        string tipo;
        if      (op == "1") tipo = "ENTRADA";
        else if (op == "2") tipo = "SAIDA_ALMOCO";
        else if (op == "3") tipo = "RETORNO_ALMOCO";
        else if (op == "4") tipo = "SAIDA";
        else { pausar(); return; }

        // Validacoes de sequencia
        if (tipo == "ENTRADA" && ultimo == "ENTRADA") {
            cout << "\n    Ja existe uma entrada registada hoje!\n"; pausar(); return;
        }
        if ((tipo == "SAIDA" || tipo == "SAIDA_ALMOCO") && ultimo.empty()) {
            cout << "\n    Nao e possivel registar saida sem entrada!\n"; pausar(); return;
        }
        if (tipo == "SAIDA" && ultimo == "SAIDA") {
            cout << "\n    Ja existe uma saida registada!\n"; pausar(); return;
        }

        Presenca p(f->getId(), f->getNome(), hoje, agora, tipo);
        presencas.push_back(p);
        gravarPresencas();

        // Calcular horas extras na saida
        if (tipo == "SAIDA") {
            // Buscar hora de entrada de hoje
            string horaEntrada = "";
            for (auto& pr : presencas) {
                if (pr.getIdFuncionario() == f->getId() &&
                    pr.getData() == hoje && pr.getTipo() == "ENTRADA")
                    horaEntrada = pr.getHora();
            }
            if (!horaEntrada.empty())
                calcularHorasExtras(f->getId(), f->getNome(), hoje, horaEntrada, agora);

            // Verificar atraso
            int minEntrada = 0;
            for (auto& pr : presencas) {
                if (pr.getIdFuncionario() == f->getId() &&
                    pr.getData() == hoje && pr.getTipo() == "ENTRADA")
                    minEntrada = horaParaMinutos(pr.getHora());
            }
            int atraso = minEntrada - HORA_ENTRADA;
            if (atraso > 0) {
                string classif;
                if      (atraso <= 5)  classif = "Toleravel";
                else if (atraso <= 15) classif = "Leve";
                else if (atraso <= 30) classif = "Moderado";
                else                   classif = "Grave";
                cout << "\n    Atraso detectado: " << atraso << " min (" << classif << ")\n";
            }
        }

        cout << "\n    " << tipo << " registada as " << agora << "\n";
        pausar();
    }

    void verHistoricoPessoal() {
        cabecalho("MEU HISTORICO DE PRESENCAS");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        linha('-');
        cout << "  | Data       | Hora  | Tipo             |\n";
        linha('-');
        bool achou = false;
        for (auto& p : presencas) {
            if (p.getIdFuncionario() == f->getId()) {
                cout << "  | " << p.getData() << " | " << p.getHora()
                     << " | " << setw(16) << left << p.getTipo() << " |\n";
                achou = true;
            }
        }
        if (!achou) cout << "\n  Nenhum registo.\n";
        linha('-');
        pausar();
    }

    void verMinhasFaltas() {
        cabecalho("MINHAS FALTAS");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        bool achou = false;
        for (auto& fa : faltas) {
            if (fa.getIdFuncionario() == f->getId()) { fa.exibir(); achou = true; }
        }
        if (!achou) cout << "\n  Nenhuma falta registada.\n";
        pausar();
    }

    void justificarFalta() {
        cabecalho("JUSTIFICAR FALTA");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        cout << "  Data da falta (DD/MM/AAAA): "; string data; getline(cin, data);

        // Verificar se existe falta
        bool temFalta = false;
        for (auto& fa : faltas)
            if (fa.getIdFuncionario() == f->getId() && fa.getData() == data)
                temFalta = true;

        if (!temFalta) {
            cout << "\n    Nao existe falta registada nessa data!\n"; pausar(); return;
        }

        // Verificar se ja justificou
        for (auto& j : justificacoes)
            if (j.getIdFuncionario() == f->getId() && j.getDataFalta() == data) {
                cout << "\n    Ja enviou justificacao para esta data!\n"; pausar(); return;
            }

        cout << "  Motivo      : "; string motivo; getline(cin, motivo);
        cout << "  Documento   : "; string doc;    getline(cin, doc);

        int novoId = proximoIdJustificacao();
        Justificacao j(novoId, f->getId(), f->getNome(), data, motivo, doc);
        justificacoes.push_back(j);
        gravarJustificacoes();
        cout << "\n    Justificacao enviada! Aguarde aprovacao.\n";
        pausar();
    }

    void verMinhasHorasExtras() {
        cabecalho("MINHAS HORAS EXTRAS");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        double total = 0;
        linha('-');
        cout << "  | Data       | Horas  |\n";
        linha('-');
        bool achou = false;
        for (auto& h : horasExtras) {
            if (h.getIdFuncionario() == f->getId()) {
                cout << "  | " << h.getData() << " | "
                     << fixed << setprecision(1) << h.getHoras() << " h |\n";
                total += h.getHoras();
                achou = true;
            }
        }
        if (!achou) cout << "\n  Nenhuma hora extra registada.\n";
        linha('-');
        cout << "  Total: " << total << " h\n";
        pausar();
    }

    void verMeuSalario() {
        cabecalho("MEU SALARIO");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        cout << fixed << setprecision(2);
        linha('-');
        cout << "  Salario Base   : MZN " << f->getSalarioBase() << "\n";
        cout << "  Bonus          : MZN " << f->getBonus() << "\n";
        cout << "  Aumentos       : MZN " << f->getAumentos() << "\n";
        cout << "  Descontos      : MZN " << f->getDescontos() << "\n";
        linha('-');
        cout << "  SALARIO FINAL  : MZN " << f->getSalarioFinal() << "\n";
        linha('-');
        pausar();
    }

    void solicitarFerias() {
        cabecalho("SOLICITAR FERIAS");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        // Verificar pedido pendente
        for (auto& fer : listaFerias)
            if (fer.getIdFuncionario() == f->getId() && fer.getEstado() == "PENDENTE") {
                cout << "\n    Ja tem um pedido de ferias pendente!\n"; pausar(); return;
            }

        cout << "  Data inicio (DD/MM/AAAA): "; string ini; getline(cin, ini);
        cout << "  Data fim    (DD/MM/AAAA): "; string fim; getline(cin, fim);
        cout << "  Nº de dias              : "; string sdias; getline(cin, sdias);
        int dias; try { dias = stoi(sdias); } catch (...) { dias = 0; }
        if (dias <= 0) { cout << "\n    Numero de dias invalido!\n"; pausar(); return; }

        int novoId = proximoIdFerias();
        Ferias fer(novoId, f->getId(), f->getNome(), ini, fim, dias);
        listaFerias.push_back(fer);
        gravarFerias();
        cout << "\n    Pedido de ferias enviado! Aguarde aprovacao.\n";
        pausar();
    }

    void alterarMinhaSenha() {
        cabecalho("ALTERAR MINHA SENHA");
        Funcionario* f = buscarFuncionarioPorUsername(utilizadorActual->getUsername());
        if (!f) { pausar(); return; }

        cout << "  Senha actual : "; string atual; getline(cin, atual);
        if (!f->verificarSenha(atual)) {
            cout << "\n    Senha actual incorrecta!\n"; pausar(); return;
        }
        cout << "  Nova senha   : "; string nova; getline(cin, nova);
        cout << "  Confirmar    : "; string conf; getline(cin, conf);
        if (nova != conf) { cout << "\n    As senhas nao coincidem!\n"; pausar(); return; }

        f->setSenha(nova);
        gravarFuncionarios();
        cout << "\n    Senha alterada com sucesso!\n";
        pausar();
    }

    //  GERAR FALTAS AUTOMATICAS 
    void gerarFaltasAutomaticas() {
        string hoje = dataActual();
        for (auto& f : funcionarios) {
            if (!f.isActivo()) continue;
            if (!temPresenca(f.getId(), hoje) && !temFaltaRegistada(f.getId(), hoje)) {
                Falta falta(f.getId(), f.getNome(), hoje, "NAO_JUSTIFICADA");
                faltas.push_back(falta);
            }
        }
        gravarFaltas();
    }

    // 
    //  MENUS PRINCIPAIS
    // 

    void menuAdmin() {
        string op;
        do {
            cabecalho("MENU ADMINISTRADOR");
            cout << "  1.  Cadastrar Funcionario\n";
            cout << "  2.  Listar Funcionarios\n";
            cout << "  3.  Editar Funcionario\n";
            cout << "  4.  Activar Funcionario\n";
            cout << "  5.  Desactivar Funcionario\n";
            cout << "  6.  Remover Funcionario\n";
            cout << "  7.  Pesquisar Funcionario\n";
            cout << "  8.  Registar Presenca (Manual)\n";
            cout << "  9.  Ver Todas as Presencas\n";
            cout << "  10. Gerir Justificacoes\n";
            cout << "  11. Gerir Ferias\n";
            cout << "  12. Gerir Salarios\n";
            cout << "  13. Alterar Senha\n";
            cout << "  14. Relatorios\n";
            cout << "  15. Dashboard\n";
            cout << "  16. Gerar Faltas (Hoje)\n";
            cout << "  0.  Terminar Sessao\n\n";
            cout << "  Opcao: "; getline(cin, op);

            if      (op == "1")  cadastrarFuncionario();
            else if (op == "2")  listarFuncionarios();
            else if (op == "3")  editarFuncionario();
            else if (op == "4")  alterarEstadoFuncionario(true);
            else if (op == "5")  alterarEstadoFuncionario(false);
            else if (op == "6")  removerFuncionario();
            else if (op == "7")  pesquisarFuncionario();
            else if (op == "8")  registarPresencaAdmin();
            else if (op == "9")  verTodasPresencas();
            else if (op == "10") gerirJustificacoes();
            else if (op == "11") gerirFerias();
            else if (op == "12") gerirSalarios();
            else if (op == "13") alterarSenhaAdmin();
            else if (op == "14") menuRelatorios();
            else if (op == "15") dashboard();
            else if (op == "16") { gerarFaltasAutomaticas(); cout << "\n    Faltas geradas!\n"; pausar(); }
        } while (op != "0");

        utilizadorActual = nullptr;
        isAdmin = false;
    }

    void menuFuncionario() {
        string op;
        do {
            cabecalho("MENU FUNCIONARIO");
            cout << "  1. Registar Presenca\n";
            cout << "  2. Ver Meu Historico\n";
            cout << "  3. Ver Minhas Faltas\n";
            cout << "  4. Justificar Falta\n";
            cout << "  5. Ver Horas Extras\n";
            cout << "  6. Ver Meu Salario\n";
            cout << "  7. Solicitar Ferias\n";
            cout << "  8. Alterar Minha Senha\n";
            cout << "  0. Terminar Sessao\n\n";
            cout << "  Opcao: "; getline(cin, op);

            if      (op == "1") registarPresencaFuncionario();
            else if (op == "2") verHistoricoPessoal();
            else if (op == "3") verMinhasFaltas();
            else if (op == "4") justificarFalta();
            else if (op == "5") verMinhasHorasExtras();
            else if (op == "6") verMeuSalario();
            else if (op == "7") solicitarFerias();
            else if (op == "8") alterarMinhaSenha();
        } while (op != "0");

        utilizadorActual = nullptr;
        isAdmin = false;
    }

public:
    //  PONTO DE ENTRADA 
    void executar() {
        inicializarSistema();
        carregarTudo();

        string op;
        do {
            cabecalho("SISTEMA EMPRESARIAL DE PRESENCA");
            cout << "  1. Iniciar Sessao\n";
            cout << "  0. Sair\n\n";
            cout << "  Opcao: "; getline(cin, op);

            if (op == "1") {
                // Recarregar dados frescos antes do login
                carregarTudo();
                if (fazerLogin()) {
                    if (isAdmin) menuAdmin();
                    else         menuFuncionario();
                    // Recarregar apos sessao (dados podem ter mudado)
                    carregarTudo();
                }
            }
        } while (op != "0");

        cabecalho("SISTEMA EMPRESARIAL DE PRESENCA");
        cout << "  Obrigado por utilizar o sistema!\n";
        cout << "  Ate breve.\n\n";
    }
};

// ============================================================
//  MAIN
// ============================================================
int main() {
    Sistema sistema;
    sistema.executar();
    return 0;
}
