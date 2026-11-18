#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <sstream>

using namespace std;

struct Rota {
    int id;
    string veiculo;
    string origem;
    string destino;
    double distanciaKm;
    string status;
};

const char* ARQUIVO_DADOS = "rotas.txt";

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
    ofstream arquivo(ARQUIVO_DADOS);

    if (!arquivo.is_open()) {
        return false;
    }

    // Primeira linha: quantidade de rotas
    arquivo << rotas.size() << "\n";

    // Cada rota ocupa seis linhas
    for (int i = 0; i < static_cast<int>(rotas.size()); i++) {
        arquivo << rotas[i].id << "\n";
        arquivo << rotas[i].veiculo << "\n";
        arquivo << rotas[i].origem << "\n";
        arquivo << rotas[i].destino << "\n";
        arquivo << setprecision(15) << rotas[i].distanciaKm << "\n";
        arquivo << rotas[i].status << "\n";
    }

    arquivo.close();
    return !arquivo.fail();
}

bool carregarRotas(vector<Rota>& rotas, int& proximoId) {
    ifstream arquivo(ARQUIVO_DADOS);

    // Se ainda não existe arquivo, começa sem rotas.
    if (!arquivo.is_open()) {
        proximoId = 1;
        return true;
    }

    string linha;
    int quantidade;

    if (!getline(arquivo, linha)) {
        return false;
    }

    {
        stringstream conversor(linha);
        if (!(conversor >> quantidade) || quantidade < 0) {
            return false;
        }
    }

    int maiorId = 0;

    for (int i = 0; i < quantidade; i++) {
        Rota rota;
        string linhaDistancia;

        if (!getline(arquivo, linha)) {
            return false;
        }
        {
            stringstream conversor(linha);
            if (!(conversor >> rota.id)) {
                return false;
            }
        }

        if (!getline(arquivo, rota.veiculo)) return false;
        if (!getline(arquivo, rota.origem)) return false;
        if (!getline(arquivo, rota.destino)) return false;
        if (!getline(arquivo, linhaDistancia)) return false;
        if (!getline(arquivo, rota.status)) return false;

        {
            stringstream conversor(linhaDistancia);
            if (!(conversor >> rota.distanciaKm)) {
                return false;
            }
        }

        rotas.push_back(rota);

        if (rota.id > maiorId) {
            maiorId = rota.id;
        }
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

        cout << "Entrada invalida. Digite um numero inteiro.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double lerDistancia(const string& mensagem) {
    double valor;

    while (true) {
        cout << mensagem;

        if (cin >> valor && valor > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }

        cout << "Distancia invalida. Digite um valor maior que zero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string lerTexto(const string& mensagem) {
    string texto;

    while (true) {
        cout << mensagem;
        getline(cin, texto);

        if (!texto.empty()) {
            return texto;
        }

        cout << "O campo nao pode ficar vazio.\n";
    }
}

int buscarIndicePorId(const vector<Rota>& rotas, int id) {
    for (int i = 0; i < static_cast<int>(rotas.size()); i++) {
        if (rotas[i].id == id) {
            return i;
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
        cout << "Erro ao salvar os dados em rotas.txt.\n";
        return;
    }

    proximoId++;

    if (!exportarRotaTxt(novaRota)) {
        cout << "Rota salva em rotas.txt, mas nao foi possivel criar "
             << nomeArquivoRota(novaRota.id) << ".\n";
        return;
    }

    cout << "Rota cadastrada e salva nos arquivos TXT.\n";
    cout << "ID: " << novaRota.id << "\n";
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

    for (int i = 0; i < static_cast<int>(rotas.size()); i++) {
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

    cout << "\n1. Planejada\n"
         << "2. Em andamento\n"
         << "3. Concluida\n"
         << "4. Cancelada\n";

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
        cout << "Erro ao salvar a alteracao em rotas.txt.\n";
        return;
    }

    if (!exportarRotaTxt(rotas[indice])) {
        cout << "Status salvo em rotas.txt, mas houve erro ao atualizar "
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
        cout << "Cadastro removido de rotas.txt, mas nao foi possivel "
             << "excluir " << nomeArquivo << ".\n";
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

    for (int i = 0; i < static_cast<int>(rotas.size()); i++) {
        distanciaTotal += rotas[i].distanciaKm;

        if (rotas[i].status == "Planejada") {
            planejadas++;
        } else if (rotas[i].status == "Em andamento") {
            emAndamento++;
        } else if (rotas[i].status == "Concluida") {
            concluidas++;
        } else if (rotas[i].status == "Cancelada") {
            canceladas++;
        }
    }

    cout << "\n--- Resumo ---\n"
         << "Total de rotas: " << rotas.size()
         << "\nDistancia total: " << fixed << setprecision(1)
         << distanciaTotal << " km"
         << "\nPlanejadas: " << planejadas
         << "\nEm andamento: " << emAndamento
         << "\nConcluidas: " << concluidas
         << "\nCanceladas: " << canceladas << "\n";
}

void exibirMenu() {
    cout << "\n========== CONTROLE DE ROTAS ==========\n"
         << "1. Cadastrar rota\n"
         << "2. Listar rotas\n"
         << "3. Buscar rota por ID\n"
         << "4. Atualizar status\n"
         << "5. Excluir rota\n"
         << "6. Exibir resumo\n"
         << "0. Sair\n"
         << "=======================================\n";
}

int main() {
    vector<Rota> rotas;
    int proximoId = 1;
    int opcao;

    if (!carregarRotas(rotas, proximoId)) {
        cout << "Erro ao ler rotas.txt. O arquivo pode estar danificado.\n";
        cout << "O programa sera iniciado sem carregar os cadastros.\n";
        rotas.clear();
        proximoId = 1;
    } else if (!rotas.empty()) {
        cout << rotas.size() << " rota(s) carregada(s) de rotas.txt.\n";

        // Recria os arquivos individuais caso estejam faltando.
        for (int i = 0; i < static_cast<int>(rotas.size()); i++) {
            exportarRotaTxt(rotas[i]);
        }
    }

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

    return 0;
}
