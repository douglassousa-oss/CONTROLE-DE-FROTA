#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <stdexcept>

using namespace std;

struct Rota {
    int id;
    string veiculo;
    string origem;
    string destino;
    double distanciaKm;
    string status;
};

const char* ARQUIVO_BINARIO = "rotas.dat";

template <typename T>
bool gravarValor(ofstream& arquivo, const T& valor) {
    arquivo.write(reinterpret_cast<const char*>(&valor), sizeof(valor));
    return arquivo.good();
}

template <typename T>
bool lerValor(ifstream& arquivo, T& valor) {
    arquivo.read(reinterpret_cast<char*>(&valor), sizeof(valor));
    return arquivo.good();
}

bool gravarString(ofstream& arquivo, const string& texto) {
    unsigned long tamanho = static_cast<unsigned long>(texto.size());

    if (tamanho > 1000000UL) {
        return false;
    }

    if (!gravarValor(arquivo, tamanho)) {
        return false;
    }

    if (tamanho > 0) {
        arquivo.write(texto.c_str(), static_cast<streamsize>(tamanho));
    }

    return arquivo.good();
}

bool lerString(ifstream& arquivo, string& texto) {
    unsigned long tamanho = 0;

    if (!lerValor(arquivo, tamanho)) {
        return false;
    }

    if (tamanho > 1000000UL) {
        return false;
    }

    if (tamanho == 0) {
        texto.clear();
        return true;
    }

    vector<char> buffer(tamanho);

    arquivo.read(&buffer[0], static_cast<streamsize>(tamanho));

    if (!arquivo.good()) {
        return false;
    }

    texto.assign(&buffer[0], tamanho);
    return true;
}

string converterIdParaBinario(int id) {
    if (id == 0) {
        return "0";
    }

    string binario;

    while (id > 0) {
        binario = char('0' + (id % 2)) + binario;
        id /= 2;
    }

    return binario;
}

string nomeArquivoRota(int id) {
    ostringstream nome;
    nome << "rota_" << id << ".txt";
    return nome.str();
}

bool exportarRotaTxt(const Rota& rota) {
    string nomeArquivo = nomeArquivoRota(rota.id);
    ofstream arquivo(nomeArquivo.c_str());

    if (!arquivo.is_open()) {
        return false;
    }

    arquivo << "CONTROLE DE ROTAS\n";
    arquivo << "ID: " << rota.id << "\n";
    arquivo << "Codigo binario do ID: "
            << converterIdParaBinario(rota.id) << "\n";
    arquivo << "Veiculo: " << rota.veiculo << "\n";
    arquivo << "Origem: " << rota.origem << "\n";
    arquivo << "Destino: " << rota.destino << "\n";
    arquivo << "Distancia: " << fixed << setprecision(1)
            << rota.distanciaKm << " km\n";
    arquivo << "Status: " << rota.status << "\n";

    arquivo.close();
    return !arquivo.fail();
}

bool salvarRotas(const vector<Rota>& rotas) {
    ofstream arquivo(ARQUIVO_BINARIO, ios::binary);

    if (!arquivo.is_open()) {
        return false;
    }

    unsigned long quantidade = static_cast<unsigned long>(rotas.size());

    if (!gravarValor(arquivo, quantidade)) {
        return false;
    }

    for (size_t i = 0; i < rotas.size(); ++i) {
        if (!gravarValor(arquivo, rotas[i].id) ||
            !gravarString(arquivo, rotas[i].veiculo) ||
            !gravarString(arquivo, rotas[i].origem) ||
            !gravarString(arquivo, rotas[i].destino) ||
            !gravarValor(arquivo, rotas[i].distanciaKm) ||
            !gravarString(arquivo, rotas[i].status)) {
            return false;
        }
    }

    arquivo.close();
    return !arquivo.fail();
}

bool carregarRotas(vector<Rota>& rotas, int& proximoId) {
    ifstream arquivo(ARQUIVO_BINARIO, ios::binary);

    if (!arquivo.is_open()) {
        proximoId = 1;
        return true;
    }

    unsigned long quantidade = 0;

    if (!lerValor(arquivo, quantidade) || quantidade > 100000UL) {
        return false;
    }

    int maiorId = 0;

    for (unsigned long i = 0; i < quantidade; ++i) {
        Rota rota;
        rota.id = 0;
        rota.distanciaKm = 0;

        if (!lerValor(arquivo, rota.id) ||
            !lerString(arquivo, rota.veiculo) ||
            !lerString(arquivo, rota.origem) ||
            !lerString(arquivo, rota.destino) ||
            !lerValor(arquivo, rota.distanciaKm) ||
            !lerString(arquivo, rota.status)) {
            return false;
        }

        if (rota.id <= 0 ||
            rota.veiculo.empty() ||
            rota.origem.empty() ||
            rota.destino.empty() ||
            !(rota.distanciaKm > 0) ||
            rota.distanciaKm > numeric_limits<double>::max()) {
            return false;
        }

        if (rota.status != "Planejada" &&
            rota.status != "Em andamento" &&
            rota.status != "Concluida" &&
            rota.status != "Cancelada") {
            return false;
        }

        for (size_t j = 0; j < rotas.size(); ++j) {
            if (rotas[j].id == rota.id) {
                return false;
            }
        }

        rotas.push_back(rota);

        if (rota.id > maiorId) {
            maiorId = rota.id;
        }
    }

    if (maiorId == numeric_limits<int>::max()) {
        return false;
    }

    proximoId = maiorId + 1;
    return true;
}

int lerInteiro(const string& mensagem) {
    int valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }

        if (cin.eof()) {
            throw runtime_error("Fim da entrada.");
        }

        cout << "Entrada invalida. Digite um numero inteiro.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double lerDistancia(const string& mensagem) {
    double valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (valor > 0 &&
                valor <= numeric_limits<double>::max()) {
                return valor;
            }

            cout << "Distancia invalida. Digite um valor maior que zero.\n";
            continue;
        }

        if (cin.eof()) {
            throw runtime_error("Fim da entrada.");
        }

        cout << "Distancia invalida. Digite um valor numerico.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string lerTexto(const string& mensagem) {
    string texto;

    while (true) {
        cout << mensagem;

        if (!getline(cin, texto)) {
            throw runtime_error("Fim da entrada.");
        }

        if (!texto.empty()) {
            return texto;
        }

        cout << "O campo nao pode ficar vazio.\n";
    }
}

int buscarIndicePorId(const vector<Rota>& rotas, int id) {
    for (size_t i = 0; i < rotas.size(); ++i) {
        if (rotas[i].id == id) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void cadastrarRota(vector<Rota>& rotas, int& proximoId) {
    Rota novaRota;

    cout << "\n--- Cadastro de rota ---\n";

    novaRota.id = proximoId;
    novaRota.veiculo = lerTexto("Veiculo (placa ou identificacao): ");
    novaRota.origem = lerTexto("Origem: ");
    novaRota.destino = lerTexto("Destino: ");
    novaRota.distanciaKm = lerDistancia("Distancia em km: ");
    novaRota.status = "Planejada";

    rotas.push_back(novaRota);

    if (!salvarRotas(rotas)) {
        rotas.pop_back();
        cout << "Erro ao salvar os dados em rotas.dat.\n";
        return;
    }

    ++proximoId;

    if (!exportarRotaTxt(novaRota)) {
        cout << "Rota salva em rotas.dat, mas nao foi possivel criar "
             << nomeArquivoRota(novaRota.id) << ".\n";
        return;
    }

    cout << "Rota cadastrada com sucesso.\n";
    cout << "ID: " << novaRota.id << "\n";
    cout << "Codigo binario: "
         << converterIdParaBinario(novaRota.id) << "\n";
}

void listarRotas(const vector<Rota>& rotas) {
    if (rotas.empty()) {
        cout << "\nNenhuma rota cadastrada.\n";
        return;
    }

    cout << "\n--- Rotas cadastradas ---\n";
    cout << left
         << setw(6) << "ID"
         << setw(18) << "Veiculo"
         << setw(20) << "Origem"
         << setw(20) << "Destino"
         << setw(14) << "Distancia"
         << "Status\n";

    cout << "----------------------------------------------------------------------------------------\n";

    for (size_t i = 0; i < rotas.size(); ++i) {
        cout << left
             << setw(6) << rotas[i].id
             << setw(18) << rotas[i].veiculo
             << setw(20) << rotas[i].origem
             << setw(20) << rotas[i].destino
             << fixed << setprecision(1)
             << setw(14) << rotas[i].distanciaKm
             << rotas[i].status << "\n";
    }
}

void buscarRota(const vector<Rota>& rotas) {
    int id = lerInteiro("\nDigite o ID da rota: ");
    int indice = buscarIndicePorId(rotas, id);

    if (indice == -1) {
        cout << "Rota nao encontrada.\n";
        return;
    }

    cout << "\nID: " << rotas[indice].id
         << "\nCodigo binario: "
         << converterIdParaBinario(rotas[indice].id)
         << "\nVeiculo: " << rotas[indice].veiculo
         << "\nOrigem: " << rotas[indice].origem
         << "\nDestino: " << rotas[indice].destino
         << "\nDistancia: " << fixed << setprecision(1)
         << rotas[indice].distanciaKm << " km"
         << "\nStatus: " << rotas[indice].status << "\n";
}

void atualizarStatus(vector<Rota>& rotas) {
    int id = lerInteiro("\nDigite o ID da rota: ");
    int indice = buscarIndicePorId(rotas, id);

    if (indice == -1) {
        cout << "Rota nao encontrada.\n";
        return;
    }

    cout << "\n1. Planejada\n";
    cout << "2. Em andamento\n";
    cout << "3. Concluida\n";
    cout << "4. Cancelada\n";

    int opcao = lerInteiro("Escolha o novo status: ");
    string statusAnterior = rotas[indice].status;

    switch (opcao) {
        case 1:
            rotas[indice].status = "Planejada";
            break;
        case 2:
            rotas[indice].status = "Em andamento";
            break;
        case 3:
            rotas[indice].status = "Concluida";
            break;
        case 4:
            rotas[indice].status = "Cancelada";
            break;
        default:
            cout << "Opcao invalida.\n";
            return;
    }

    if (!salvarRotas(rotas)) {
        rotas[indice].status = statusAnterior;
        cout << "Erro ao salvar a alteracao em rotas.dat.\n";
        return;
    }

    if (!exportarRotaTxt(rotas[indice])) {
        cout << "Status salvo em rotas.dat, mas houve erro ao atualizar "
             << nomeArquivoRota(rotas[indice].id) << ".\n";
        return;
    }

    cout << "Status atualizado e salvo.\n";
}

void excluirRota(vector<Rota>& rotas) {
    int id = lerInteiro("\nDigite o ID da rota: ");
    int indice = buscarIndicePorId(rotas, id);

    if (indice == -1) {
        cout << "Rota nao encontrada.\n";
        return;
    }

    Rota rotaExcluida = rotas[indice];
    rotas.erase(rotas.begin() + indice);

    if (!salvarRotas(rotas)) {
        rotas.insert(rotas.begin() + indice, rotaExcluida);
        cout << "Erro ao salvar a exclusao. A rota foi mantida.\n";
        return;
    }

    string nomeArquivo = nomeArquivoRota(rotaExcluida.id);

    if (remove(nomeArquivo.c_str()) != 0) {
        cout << "Rota removida de rotas.dat, mas nao foi possivel excluir "
             << nomeArquivo << ".\n";
        return;
    }

    cout << "Rota e arquivo TXT excluidos com sucesso.\n";
}

void exibirResumo(const vector<Rota>& rotas) {
    if (rotas.empty()) {
        cout << "\nNenhuma rota cadastrada.\n";
        return;
    }

    int planejadas = 0;
    int emAndamento = 0;
    int concluidas = 0;
    int canceladas = 0;
    double distanciaTotal = 0;

    for (size_t i = 0; i < rotas.size(); ++i) {
        distanciaTotal += rotas[i].distanciaKm;

        if (rotas[i].status == "Planejada") {
            ++planejadas;
        } else if (rotas[i].status == "Em andamento") {
            ++emAndamento;
        } else if (rotas[i].status == "Concluida") {
            ++concluidas;
        } else if (rotas[i].status == "Cancelada") {
            ++canceladas;
        }
    }

    cout << "\n--- Resumo ---\n";
    cout << "Total de rotas: " << rotas.size() << "\n";
    cout << "Distancia total: " << fixed << setprecision(1)
         << distanciaTotal << " km\n";
    cout << "Planejadas: " << planejadas << "\n";
    cout << "Em andamento: " << emAndamento << "\n";
    cout << "Concluidas: " << concluidas << "\n";
    cout << "Canceladas: " << canceladas << "\n";
}

void exibirMenu() {
    cout << "\n========== CONTROLE DE ROTAS ==========\n";
    cout << "1. Cadastrar rota\n";
    cout << "2. Listar rotas\n";
    cout << "3. Buscar rota por ID\n";
    cout << "4. Atualizar status\n";
    cout << "5. Excluir rota\n";
    cout << "6. Exibir resumo\n";
    cout << "0. Sair\n";
    cout << "=======================================\n";
}

int main() {
    vector<Rota> rotas;
    int proximoId = 1;

    if (!carregarRotas(rotas, proximoId)) {
        cout << "Erro ao ler rotas.dat. O arquivo pode estar danificado.\n";
        cout << "O programa sera encerrado para evitar sobrescrever os dados.\n";
        return 1;
    }

    if (!rotas.empty()) {
        cout << rotas.size()
             << " rota(s) carregada(s) de rotas.dat.\n";

        for (size_t i = 0; i < rotas.size(); ++i) {
            exportarRotaTxt(rotas[i]);
        }
    }

    try {
        int opcao = -1;

        do {
            exibirMenu();
            opcao = lerInteiro("Escolha uma opcao: ");

            switch (opcao) {
                case 1:
                    cadastrarRota(rotas, proximoId);
                    break;
                case 2:
                    listarRotas(rotas);
                    break;
                case 3:
                    buscarRota(rotas);
                    break;
                case 4:
                    atualizarStatus(rotas);
                    break;
                case 5:
                    excluirRota(rotas);
                    break;
                case 6:
                    exibirResumo(rotas);
                    break;
                case 0:
                    cout << "Programa encerrado.\n";
                    break;
                default:
                    cout << "Opcao invalida. Tente novamente.\n";
            }
        } while (opcao != 0);

    } catch (const runtime_error&) {
        cout << "\nEntrada encerrada. Programa finalizado.\n";
    }

    return 0;
}
