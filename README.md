# Sistema de Controle de Rotas

Programa em C++ para cadastrar e gerenciar rotas de veículos. O código foi adaptado para funcionar em versões antigas do Dev-C++.

## Funções

- **Cadastrar rota:** registra veículo, origem, destino e distância. O status inicial é “Planejada”.
- **Listar rotas:** mostra as rotas cadastradas.
- **Buscar rota:** exibe os dados de uma rota pelo ID.
- **Atualizar status:** altera o status para Planejada, Em andamento, Concluida ou Cancelada.
- **Excluir rota:** remove uma rota e seu arquivo TXT individual.
- **Exibir resumo:** mostra o total de rotas, a distância acumulada e a quantidade por status.

## Arquivos gerados

- `rotas.dat`: armazena os dados das rotas em formato binário. O programa lê esse arquivo ao iniciar e grava nele após alterações.
- `rota_ID.txt`: arquivo de texto individual criado para cada rota. Contém os dados legíveis e o ID em binário. Por exemplo, o ID 5 aparece como `101`.

O arquivo `rotas.dat` não é destinado à leitura manual em um editor de texto.

## Como compilar no Dev-C++

1. Abra o Dev-C++.
2. Crie um novo arquivo-fonte C++ ou abra o arquivo do programa.
3. Cole o código completo e salve com a extensão `.cpp`, por exemplo, `controle_rotas.cpp`.
4. Selecione **Executar > Compilar e Executar** ou pressione **F11**.
5. Use o menu exibido no console para escolher uma opção.

## Como usar

Ao iniciar, o programa carrega as rotas existentes em `rotas.dat`. Se esse arquivo ainda não existir, começa sem rotas.

No menu, digite o número da opção desejada e pressione Enter. Para cadastrar uma rota, informe os dados solicitados. A distância deve ser maior que zero. Para encerrar, escolha `0`.

## Observações

- Mantenha `rotas.dat` na mesma pasta de trabalho do programa para que os dados sejam encontrados ao iniciar.
- Os arquivos são criados no diretório de trabalho configurado no Dev-C++, que pode não ser necessariamente a pasta do arquivo `.cpp`.
- Os dados do antigo `rotas.txt` não são importados automaticamente. Faça uma cópia de segurança antes de testar a versão binária.
- O formato binário usa a representação de tipos numéricos do compilador e pode não ser compatível entre diferentes compiladores ou arquiteturas.
