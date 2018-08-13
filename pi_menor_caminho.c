#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>

#define MAXFILHOS 8

// struct que vai se comportar como o mapa com as coordenadas
struct celula{
	int custoG;		    	// custo da celula ate a partida
	int custoH;		    	// custo da celula ate o objetivo
	int custoF;    			// custo total da celula (F + G)
	struct celula *pai;		// apontador para o pai
	int valor;              // valor (0, 1, 2, 3)
	int caminho;            // afirma se a celula faz parte do caminho (mostrar no mapa)
	int x, y;				// coordenadas
	struct celula *myEnder; // teste
};


// funcao para abrir ps arquivos que vamos usar
int abreArquivos(FILE **p_arq_shop, FILE **p_arq_mapa, FILE **p_arq_obs, FILE **p_arq_loj);
// funcao para ler o shopping que o usuario esta
int lerShopping(FILE *p_arq_shop, int *escolha);
// funcao para retornar o cnpj do shopping escolhido
int pegaCnpjShopping(FILE *p_arq_shop, long int *shop_cnpj, int indice_escolhido);
// funcao para ler o destino (loja) do usuario
int lerLojaDestino(FILE **p_arq_loj, long int shop_cnpj_esco, int *escolha);
// funcao para pegar as coordenadas da loja no mapa
int pegaCoordenadasLoja(FILE **p_arq_loj, int loja_escolhida, long int shop_cnpj_esco, int *posX, int *posY);
// funcao para pegar tamanho do mapa
int pegaTamMapa(FILE **p_arq_mapa, int *mapaX, int *mapaY, long int cnpj_shop_ecolhido, int *id_map);
// funcao para colocar os obstaculos no mapa
void addObsMapa(FILE **p_arq_obs, int id_mapa_selecio, int i, int j, struct celula mapa[i][j]);
// zerar o mapa
void zeraMapa(int lin, int col, struct celula map[lin][col], int all);
// encontra o caminho
int findpath(int linha, int coluna, struct celula mapa[linha][coluna], struct celula *inicio, struct celula *fim);
// calcula o custo F do filho
int calculaCustH(int xOb, int yOb, int xF, int yF);
// calcula o custo G do filho
int calculaCustG(int xP, int yP, int xF, int yF, int fhatersG);
// adiciona a celula na lista aberta
void addOpenList(struct celula *celula, struct celula *openlist[]);
// remove a celula da lista aberta
void removeOpenList(struct celula *openlist[]);
// adiciona a celula na lista fechada
void addClosedList(struct celula *celula, struct celula *closedlist[]);
// procurar celula na lista fechada
int searchClosedList(struct celula *celula, struct celula *closedlist[]);
// procurar celula na lista aberta
int searchOpenList(struct celula *celula, struct celula *openlist[]);
// ordear lista aberta
void sortOpenList(struct celula *openlist[], int inicio, int fim);
// descobrir os vizinhos da celula
void findNeighbor(struct celula *neighbors[], int lin, int col, struct celula map[lin][col], struct celula *father);
// funcao para por os vizinhos encontrados no fim
void poeNoFim(struct celula *neighbors[], struct celula *neighbor);
// compilar os movimentos
void compilarMov(struct celula *current);
// arrastar os valores para o fim
void dragToEnd(int tamanho, char *moviments);
// mostrar o mapa com o caminho
void finalMap(int line, int cols, struct celula map[line][cols], int detalhe);
// fazer o CEGO andar
void moveOn(int linha, int coluna, struct celula mapa[linha][coluna], struct celula *inicio, struct celula *fim);
// valida o movimento do CEGO
void validaMovimento(int desX, int desY, struct celula **atual, int linha, int coluna, struct celula mapa[linha][coluna], struct celula *fim);
// procura o proximo movimento do cego
void nextStep(struct celula *father, int lin, int col, struct celula mapa[lin][col]);

int main(void){
	
	// muda o titulo do console
	if(!SetConsoleTitle("PI - Financegos")){
		printf("Erro ao chamar SetConsoleTitle... \n\nAguarde..."); Sleep(1500); system("cls");
	}
	
	// muda a cor do console
	system("color F0");
	
	// ponteiros para os arquivos
		FILE *arq_shop;
		FILE *arq_mapa;
		FILE *arq_obs;
		FILE *arq_loj;
	
	// abrir os arquivos como leitura
	if(!abreArquivos(&arq_shop, &arq_mapa, &arq_obs, &arq_loj)){
		// se algum arquivo nao foi aberto
		printf("ERRO 1");
		return 0;
	}
	
	// guarda o shopping escolhido pelo user
		int escolha = 0;
	
	// ler o shopping que o usuario esta
	if(!lerShopping(arq_shop, &escolha)){
		// se o arquivo de shoppings estiver vazio
		printf("ERRO 2");
		return 0;
	}
	
	system("cls");
	
	// retorno o ponteiro do arquivo de shoppings ara o inicio
	rewind(arq_shop);
	
	// guarda o cnpj do shopping
		long int shop_cnpj;
	
	// pegar o cnpj do shopping que o usuario escolher
	if(!pegaCnpjShopping(arq_shop, &shop_cnpj, escolha)){
		// se nao pegou o cnpj
		printf("ERRO 3");
		return 0;
	}
	
	// ler a loja que o usuario que ir
	if(!lerLojaDestino(arq_loj, shop_cnpj, &escolha)){
		printf("ERRO 4");
		return 0;
	}
	
	// retorno o ponteiro do arquivo de shoppings ara o inicio
	rewind(arq_loj);
	
	// coordenada da loja escolhida
		int fimX, fimY;
	
	// recuperar a posicao da loja (vai ser o destino)
	if(!pegaCoordenadasLoja(arq_loj, escolha, shop_cnpj, &fimX, &fimY)){
		printf("ERRO 5");
		return 0;
	}
	
	// guarda o tamanho do mapa
		int linha, coluna;
	// guarda o id do mapa
		int id_mapa;
	
	// pegar o tamanho do mapa
	if(!pegaTamMapa(arq_mapa, &linha, &coluna, shop_cnpj, &id_mapa)){
		printf("ERRO 6");
		return 0;
	}
	
	struct celula *fim, *inicio;		// objetivo, partida
	struct celula mapa[linha][coluna]; 	// mapa

	// inicializar o mapa
	zeraMapa(linha, coluna, mapa, 1);
	
	// adiciona os obstaculos do arquivo na matriz
	addObsMapa(arq_obs, id_mapa, linha, coluna, mapa);

	system("cls");
	
	// linha e coluna do inicio
		int iniX, iniY;
	
	do{
		printf("\t\t\tPOSICAO ATUAL: \n\n");
		finalMap(linha, coluna, mapa, 1);
		printf("\t\t\tLINHA: "); scanf("%d", &iniX);
		printf("\t\t\tCOLUNA: "); scanf("%d", &iniY);
		
		if(mapa[iniX][iniY].valor == 1){
			printf("\t\t\tPOSICAO INVALIDA... \n\n");
			Sleep(1500);
		}else if(iniX == fimX && iniY == fimY){
			printf("\t\t\tPONTO DE PARTIDA COINCIDE COM O DESTINO...\n\n");
			Sleep(1500);
		}
		
		system("cls");
	}while((mapa[iniX][iniY].valor != 0) || (iniX == fimX && iniY == fimY));
	
	mapa[iniX][iniY].valor = 2;
	inicio = &mapa[iniX][iniY];
	mapa[fimX][fimY].valor = 3;
	fim = &mapa[fimX][fimY];
	
	// chama a funcao para encontrar o caminho
	if(findpath(linha, coluna, mapa, inicio, fim)){
		printf("\t\t\tO CAMINHO FOI ENCONTRADO!\n\n");
		
		Sleep(1500);
		system("cls");

		// mostrar o caminho mais curto
		compilarMov(fim);

		// mover-se pelo mapa
		moveOn(linha, coluna, mapa, inicio, fim);
	}else{
		printf("\t\t\tO CAMINHO NAO FOI ENCONTRADO.\n\n");
	}

	system("pause");
	return 0;
}

// funcao para abrir ps arquivos que vamos usar
int abreArquivos(FILE **p_arq_shop, FILE **p_arq_mapa, FILE **p_arq_obs, FILE **p_arq_loj){
	
	*p_arq_shop = fopen("banco\\shoppings.txt", "r");
	if(*p_arq_shop == NULL){
		printf("Erro ao conectar com a tabela de Shoppings...\n");
		return 0;
	}

	*p_arq_mapa = fopen("banco\\mapas.txt", "r");
	if(*p_arq_mapa == NULL){
		printf("Erro ao conectar com a tabela de Mapas...\n");
		return 0;
	}

	*p_arq_obs = fopen("banco\\pontos_fisicos.txt", "r");
	if(*p_arq_obs == NULL){
		printf("Erro ao conectar com a tabela de Obstaculos...\n");
		return 0;
	}

	*p_arq_loj = fopen("banco\\lojas.txt", "r");
	if(*p_arq_loj == NULL){
		printf("Erro ao conectar com a tabela de lojas...\n");
		return 0;
	}

	return 1;
}

// funcao para ler o shopping ao qual a loja pertence
int lerShopping(FILE *p_arq_shop, int *escolha){
	// variaveis para ler os registros
		int indice = 1;		// indice do shopping pra mostrar
		int encontrado = 0;	// diz se algum registro foi encontrado
		long int shop_cnpj; // cnpj (nao mostro mas preciso ler)
		char nome_shop[31]; // nome do shopping para ser mostrado

	// mostro a lista de shoppings
 printf("\t\t\tESCOLHA UM DESTINO:\n");
	fscanf(p_arq_shop, "%li - %30[^\n]s", &shop_cnpj, nome_shop); setbuf(stdin, NULL);
	while(!feof(p_arq_shop)){
		printf("\t%d. %s;\n", indice, nome_shop);
		encontrado = 1;
		indice++;
		fscanf(p_arq_shop, "%li - %30[^\n]s", &shop_cnpj, nome_shop); setbuf(stdin, NULL);
	}

	// se a lista estiver vazia
	if(!encontrado){
		printf("NAO HA SHOPPINGS CADASTRADOS...\nO PROCESSO FOI INTERROMPIDO.\n");
		return 0;
	}
	
	printf("\n\t\t\tEM QUAL SHOPPING VOCE ESTA? ");
	// tratamento de erro da escolha do shopping
	do{
		scanf("%d", escolha); setbuf(stdin, NULL);
		if(*escolha < 1 || *escolha > indice - 1){
			printf("\t\t\tEsse indice nao existe... Digite novamente: ");
		}
	}while(*escolha < 1 || *escolha > indice - 1);

	return 1;
}

// funcao para retornar o cnpj do shopping escolhido
int pegaCnpjShopping(FILE *p_arq_shop, long int *shop_cnpj, int indice_escolhido){
	int indice = 1;     // contador
	int encontrado = 0;	// diz se algum registro foi encontrado
	char nome_shop[31]; // nome do shopping

	fscanf(p_arq_shop, "%li - %30[^\n]s", shop_cnpj, nome_shop); setbuf(stdin, NULL);
	while(!feof(p_arq_shop)){
		if(indice == indice_escolhido){
			encontrado = 1;
			break;
		}
		indice++;
		fscanf(p_arq_shop, "%li - %30[^\n]s", shop_cnpj, nome_shop); setbuf(stdin, NULL);
	}

	return encontrado;
}

// funcao para ler o destino (loja) do usuario
int lerLojaDestino(FILE **p_arq_loj, long int shop_cnpj_esco, int *escolha){
	long int shop_cnpj; // cnpj do shopping no arquivo de lojas
	long int loj_cnpj;  // cnpj da loja
	char loj_name[31];  // nome da loja
	int fimX, fimY;     // coordenadas da loja
	int indice = 1;     // contador
	int encontrado = 0; // diz se algum registro foi encontrado
	
	printf("\t\t\tESCOLHA UM DESTINO:\n");
	fscanf(p_arq_loj, "%li - %li - %d - %d - %30[^\n]s", &loj_cnpj, &shop_cnpj, &fimX, &fimY, loj_name);
	while(!feof(p_arq_loj)){
		if(shop_cnpj == shop_cnpj_esco){
			printf("\t\t\t\t%d - %s\n", indice, loj_name);
			encontrado = 1;
			indice++;
		}
		fscanf(p_arq_loj, "%li - %li - %d - %d - %30[^\n]s", &loj_cnpj, &shop_cnpj, &fimX, &fimY, loj_name);
	}
	
	// se a lista estiver vazia
	if(!encontrado){
		printf("NAO HA LOJAS CADASTRADOS...\nO PROCESSO FOI INTERROMPIDO.\n");
		return 0;
	}
	
	printf("\n\t\t\tPARA QUAL LOJA DESEJA IR? ");
	do{
		scanf("%d", escolha);
		if(*escolha < 1 || *escolha > indice - 1){
			printf("\t\t\tEsse indice nao existe... Digite novamente: ");
		}
	}while(*escolha < 1 || *escolha > indice - 1);
	
	return 1;
}

// funcao para pegar as coordenadas da loja no mapa
int pegaCoordenadasLoja(FILE **p_arq_loj, int loja_escolhida, long int shop_cnpj_esco, int *posX, int *posY){
	int indice = 1;     // contador
	long int loj_cnpj;  // cnpj da loja
	char loj_name[31];  // nome da loja
	long int shop_cnpj; // cnpj do shopping no arquivo de lojas
	int encontrado = 0; // diz se algum registro foi encontrado
	
	fscanf(p_arq_loj, "%li - %li - %d - %d - %30[^\n]", &loj_cnpj, &shop_cnpj, posX, posY, loj_name);
	while(!feof(p_arq_loj)){
		if(shop_cnpj == shop_cnpj_esco){
			if(indice == loja_escolhida){
				encontrado = 1;
				break;
			}
			indice++;
		fscanf(p_arq_loj, "%li - %li - %d - %d - %30[^\n]", &loj_cnpj, &shop_cnpj, posX, posY, loj_name);
		}
	}
	
	// verifica se conseguiu encontrar o registro acima
	if(!encontrado){
		printf("\t\t\tREGISTRO NAO ENCONTRADO... TENTE NOVAMENTE.");
		return 0;
	}
	
	return 1;
}

// funcao para pegar tamanho do mapa
int pegaTamMapa(FILE **p_arq_mapa, int *mapaX, int *mapaY, long int cnpj_shop_ecolhido, int *id_map){
	long int shop_cnpj; // cnpj do shopping nno arquivo
	int encontrado = 0; // diz se algum registro foi encontrado
	
	fscanf(p_arq_mapa, "%d - %li - %d - %d", id_map, &shop_cnpj, mapaX, mapaY);
	while(!feof(p_arq_mapa)){
		if(shop_cnpj == cnpj_shop_ecolhido){
			encontrado = 1;
			break;
		}
		fscanf(p_arq_mapa, "%d - %li - %d - %d", id_map, &shop_cnpj, mapaX, mapaY);
	}

	// verifica se conseguiu encontrar o registro acima
	if(!encontrado){
		printf("\t\t\tREGISTRO NAO ENCONTRADO... TENTE NOVAMENTE.");
		return 0;
	}
	
	return 1;
}

// funcao para colocar os obstaculos no mapa
void addObsMapa(FILE **p_arq_obs, int id_mapa_selecio, int i, int j, struct celula mapa[i][j]){
	int idObs;      // id do obstaculo
	int idMap;      // id do mapa
	int obsX, obsY; // linha e coluna do obstaculo

	fscanf(p_arq_obs, "%d - %d - %d - %d", &idObs, &idMap, &obsX, &obsY);
	while(!feof(p_arq_obs)){
		if(idMap == id_mapa_selecio){
			mapa[obsX][obsY].valor = 1;
		}
		fscanf(p_arq_obs, "%d - %d - %d - %d", &idObs, &idMap, &obsX, &obsY);
	}
	
	return;
}

// funcao para zerar os valores do mapa
void zeraMapa(int lin, int col, struct celula map[lin][col], int all){
	int i, j;
	for(i = 0; i < lin; i++){
		for(j = 0; j < col; j++){
			map[i][j].caminho = 0;
			map[i][j].custoF = 0;
			map[i][j].custoG = 0;
			map[i][j].custoH = 0;
			map[i][j].pai = NULL;
			if(all){
				map[i][j].valor = 0;
				map[i][j].myEnder = &map[i][j];
				map[i][j].x = i;
				map[i][j].y = j;
			}else if(map[i][j].valor == 2)
				map[i][j].valor = 0;
		}
	}
}

// funcao para encontrar o caminho
int findpath(int linha, int coluna, struct celula mapa[linha][coluna], struct celula *inicio, struct celula *fim){
	int qtdViz = 1, i;						// quantidade total de vizinhos validos na lista aberta (usado na ordenacao aa lista)
	int encontrado = 0;                     // diz se o objetivo foi encontrado
	struct celula *atual; 					// celula atual na verificacao
	struct celula *aberto[linha * coluna];	// lista de ponteiros para as proximas celulas a serem verificadas
	struct celula *fechado[linha * coluna];	// lista de ponteiros para as celulas que ja foram verificadas
	
	// inicializar os vetores com nulo
	for(i = 0; i < (linha * coluna); i++){
		aberto[i] = NULL; fechado[i] = NULL;
	}
	
	// coloco a posi��o de partida na lista aberta
	addOpenList(inicio, aberto);
	
	do{
		atual = aberto[0]; // pego a celula de menor custo

		if(atual == fim){ // se encontramos o caminho
			encontrado = 1;
			continue;
		}

		if(aberto[0] == 0){ // se nao encontramos o caminho
			continue;
		}

		removeOpenList(aberto); // removo a celula atual da lista aberta

		addClosedList(atual, fechado); // adiciono a celula atual na lista fechada

		// decremento a contagem de vizinhos
		qtdViz--;

		struct celula *tempNeighbor[MAXFILHOS]; // guarda temporariamente os filhos da celula pra validar depois

		findNeighbor(tempNeighbor, linha, coluna, mapa, atual); // funcao para encontra os filhos da celula

		for(i = 0; tempNeighbor[i] != NULL && i < MAXFILHOS;i++){
			if(tempNeighbor[i]->valor == 1 || searchClosedList(tempNeighbor[i], fechado)){ // se for um obstaculo ou tiver na lista fechada
				continue;
			}

			if(!searchOpenList(tempNeighbor[i], aberto)){ // se o vizinho nao estiver na lista aberta
				int x = tempNeighbor[i]->x;
				int y = tempNeighbor[i]->y;
				// defino o no 'atual' como o pai do vizinho
				mapa[x][y].pai = atual;

				// calcular os custos do vizinho
				mapa[x][y].custoH = calculaCustH(fim->x, fim->y, x, y);
				mapa[x][y].custoG = calculaCustG(atual->x, atual->y, x, y, atual->custoG);
				mapa[x][y].custoF = mapa[x][y].custoG + mapa[x][y].custoH;

				// adiciono o vizinho na lista aberta
				addOpenList(&mapa[x][y], aberto);

				// incremento a contagem de vizinhos
				qtdViz++;
			}else{ // se o vizinho estiver na lista aberta
				int x = tempNeighbor[i]->x;
				int y = tempNeighbor[i]->y;
				
				// calculo o novo custo G
				int newG = calculaCustG(atual->x, atual->y, x, y, atual->custoG);

				if(newG < mapa[x][y].custoG){// se o novo custo foi menor que o anterior
					// atualizo os custos
					mapa[x][y].custoG = newG;
					mapa[x][y].custoF = mapa[x][y].custoG + mapa[x][y].custoH;

					// mudo o pai da celula
					mapa[x][y].pai = atual;
				}
			}
		}

		// ordenar a lista aberta
		sortOpenList(aberto, 0, qtdViz);
	}while(aberto[0] != 0 && !encontrado);
	
	return encontrado;
}

// funcao para adicionar na lista aberta
void addOpenList(struct celula *celula, struct celula *openlist[]){
	int i;

	// varre o vetor ate encontrar a primeira posicao vazia
	for(i = 0; openlist[i] != 0; i++){
		continue;
	}

	// coloca o endereco de memoria da celula na posicao
	openlist[i] = celula;

	return;
}

// funcao para remover da lista aberta
void removeOpenList(struct celula *openlist[]){
	int i;

	// como o vetor vai estar ordenado eu so preciso remover o primeiro valor
	// entao eu vou sobreescrevendo todos os valores
	for(i = 1; openlist[i] != 0; i++){
		openlist[i - 1] = openlist[i];
	}

	// no final um vai estar repetido, so tenho que zera-lo aqui
	openlist[i - 1] = NULL;

	return;
}

// funcao para adicionar na lista fehada
void addClosedList(struct celula *celula, struct celula *closedlist[]){
	int i;

	// varro o vetor ate encontrar a primeira posicao igual a zero
	for(i = 0; closedlist[i] != 0; i++){
		continue;
	}

	// adiciono a celula na posicao vazia
	closedlist[i] = celula;

	return;
}

// funcao para encontrar vizinhos
void findNeighbor(struct celula *neighbors[], int lin, int col, struct celula map[lin][col], struct celula *father){
	int i;
	for(i = 0; i < MAXFILHOS; i++){
		neighbors[i] = NULL;
	}

	if(father->x + 1 < lin){
		poeNoFim(neighbors, &map[father->x + 1][father->y]); // posicao abaixo

		if(father->y + 1 < col){
			if(map[father->x][father->y + 1].valor != 1 && map[father->x + 1][father->y].valor != 1){
				poeNoFim(neighbors, &map[father->x + 1][father->y + 1]); // diagonal inferior frontal
			}
		}

		if(father->y - 1 < col && father->y - 1 >= 0){
			if(map[father->x][father->y - 1].valor != 1 && map[father->x + 1][father->y].valor != 1){
				poeNoFim(neighbors, &map[father->x + 1][father->y - 1]); // diagonal inferior traseira
			}
		}
	}

	if(father->y + 1 < col)
		poeNoFim(neighbors, &map[father->x][father->y + 1]); // posicao frontal

	if(father->y - 1 < col && father->y - 1 >= 0)
		poeNoFim(neighbors, &map[father->x][father->y - 1]); // posicao atras

	if(father->x - 1 < lin && father->x - 1 >= 0){
		poeNoFim(neighbors, &map[father->x - 1][father->y]); // posicao acima

		if(father->y + 1 < col){
			if(map[father->x][father->y + 1].valor != 1 && map[father->x - 1][father->y].valor != 1){
				poeNoFim(neighbors, &map[father->x - 1][father->y + 1]);// diagonal superior frontal
			}
		}
		if(father->y - 1 < col && father->y - 1 >= 0){
			if(map[father->x][father->y - 1].valor != 1 && map[father->x - 1][father->y].valor != 1){
				poeNoFim(neighbors, &map[father->x - 1][father->y - 1]);// diagonal superior traseira
			}
		}
	}
	return;
}

//funcao para por os vizinhos encontrados no fim
void poeNoFim(struct celula *neighbors[], struct celula *neighbor){
	int i;
	for(i = 0; neighbors[i] != NULL; i++){
		continue;
	}

	neighbors[i] = neighbor;

	return;
}

// funcao para procurar celula na lista fechada
int searchClosedList(struct celula *celula, struct celula *closedlist[]){
	int i;

	for(i = 0;closedlist[i] != NULL;i++){
		if(closedlist[i] == celula)
			return 1;
	}

	return 0;
}

// funcao para procurar celula na lista aberta
int searchOpenList(struct celula *celula, struct celula *openlist[]){
	int i;

	for(i = 0;openlist[i] != NULL;i++){
		if(openlist[i] == celula)
			return 1;
	}

	return 0;
}

// funcao para calcular o custo H do filho (dela ate o objetivo)
int calculaCustH(int xOb, int yOb, int xF, int yF){
	int soma = 0;

	soma += abs(xOb - xF); // pego a diferenca de linha
	soma += abs(yOb - yF); // pego a diferenca de coluna

	return soma;
}

// funcao para calcular o custo G do filho (dela ate a partida)
int calculaCustG(int xP, int yP, int xF, int yF, int fhatersG){
	int soma = 0;

	if(xP != xF && yP != yF){ // se estiver na diagonal
		soma = fhatersG + 14;
	}else{
		soma = fhatersG + 10;
	}

	return soma;
}

// funcao para ordenar lista aberta
void sortOpenList(struct celula *openlist[], int inicio, int fim){
	int i = inicio;
	int j = fim - 1;
	struct celula *pivo;
	struct celula *aux;

	pivo = openlist[(inicio + fim) / 2];

	while(i <= j){

		while(openlist[i]->custoF < pivo->custoF && i < fim)
			i++;

		while(openlist[j]->custoF > pivo->custoF && j > inicio)
			j--;

		if(i <= j){
			aux = openlist[i];
			openlist[i] = openlist[j];
			openlist[j] = aux;
			i++;
			j--;
		}
	}

	if(j > inicio)
		sortOpenList(openlist, inicio, j + 1);

	if(i < fim)
		sortOpenList(openlist, i, fim);
}

// funcao para compilar os movimentos
void compilarMov(struct celula *current){
	
	// repito enquanto nao chegar no ponto de partida
	while(current->valor != 2){
		// pego o pai da celula atual
		struct celula *next = current->pai;

		// digo que essa celula faz parte do caminho
		current->caminho = 1;

		// atualizo atual para a sua ceula pai
		current = next;
	}
	
	return;
}

// funcao para arrastar os valores para o fim
void dragToEnd(int tamanho, char *moviments){
	// aloco mais memoria para ele
	moviments = realloc(moviments, sizeof(char) * tamanho);

	int i = tamanho - 1;

	for(;tamanho > 0; tamanho--){
		moviments[tamanho] = moviments[tamanho - 1];
	}

	return;
}

// funcao para mostrar o mapa com o caminho
void finalMap(int line, int cols, struct celula map[line][cols], int detalhe){
	int i, j;
	// percorrer a matriz pra mostrar o mapinha
	if(detalhe){
		printf("\t\t\t     ");
		for(j = 0; j < cols; j++){
			printf("% 3d  ", j);
		}
		printf("\n");
	}
	
	for(i = 0; i < line; i++){
		if(detalhe)
			printf("\t\t\t% 3d |", i);
		else
			printf("\t\t\t|");
		for(j = 0; j < cols; j++){
			switch (map[i][j].valor) {
	          //case 1: printf("\xb2\xb2\xb2\xb2\xb2"); break;
	          case 1: printf("%c%c%c%c%c", 219, 219, 219, 219, 219); break;
	          case 2: printf("  %c  ", 446); break;
	          case 3: printf("  %c  ", 414); break;
	          default:
			  	if(map[i][j].caminho)
			  		printf("  \xa9  ");
			  	else
			  		printf("     ");
			  	break;
	        }
		}
		printf("|\n\n");
	}

	return;
}

// funcao para fazer o cego andar
void moveOn(int linha, int coluna, struct celula mapa[linha][coluna], struct celula *inicio, struct celula *fim){
	// variavel para ler os movimentos
	char mov[3] = {' ', ' ', '\0'};
	// posicao atual dele
	struct celula *atual = inicio;
	int i;
	
	do{
		// mostrar o mapinha dahora
		finalMap(linha, coluna, mapa, 0);
		
		// menu de acoes
		printf("\t\t\tPARA MOVER-SE USE AS TECLAS:\n");
		printf("\t\t\t\tW - CIMA;\n");
		printf("\t\t\t\tS - BAIXO;\n");
		printf("\t\t\t\tA - ESQUERDA;\n");
		printf("\t\t\t\tD - DIREITA;\n");
		printf("\t\t\t\tSA - DIAGONAL INFERIOR ESQUERDA;\n");
		printf("\t\t\t\tSD - DIAGONAL INFERIOR DIREITA;\n");
		printf("\t\t\t\tWA - DIAGONAL SUPERIOR ESQUERDA;\n");
		printf("\t\t\t\tWD - DIAGONAL SUPERIOR DIREITA;\n");
		printf("\t\t\t\tSUGERIDO - ");
		// procura o proximo passo do cego
		nextStep(atual, linha, coluna, mapa);
		printf("\t\t\t\t->"); scanf("%s", mov);
		
		// transformo a entrada em caixa alta
		for(i = 0; i < 3; i++){
			mov[i] = toupper(mov[i]);
		}

		// identifico a movimentacao
		if(!strcmp(mov, "W")){
			validaMovimento(-1, 0, &atual, linha, coluna, mapa, fim);
		}else if(!strcmp(mov, "A")){
			validaMovimento(0, -1, &atual, linha, coluna, mapa, fim);
		}else if(!strcmp(mov, "S")){
			validaMovimento(1, 0, &atual, linha, coluna, mapa, fim);
		}else if(!strcmp(mov, "D")){
			validaMovimento(0, 1, &atual, linha, coluna, mapa, fim);
		}else if(!strcmp(mov, "SA") || !strcmp(mov, "AS")){
			if(mapa[atual->x][atual->y - 1].valor != 1 && mapa[atual->x + 1][atual->y].valor != 1)
				validaMovimento(1, -1, &atual, linha, coluna, mapa, fim);
			else{
				printf("\t\t\tVOCE VAI COLIDIR COM UM OBSTACULO..");
				Sleep(1500);
			}
		}else if(!strcmp(mov, "SD") || !strcmp(mov, "DS")){
			if(mapa[atual->x][atual->y + 1].valor != 1 && mapa[atual->x + 1][atual->y].valor != 1)
                validaMovimento(1, 1, &atual, linha, coluna, mapa, fim);
			else{
				printf("\t\t\tVOCE VAI COLIDIR COM UM OBSTACULO..");
				Sleep(1500);
			}
		}else if(!strcmp(mov, "WA") || !strcmp(mov, "AW")){
			if(mapa[atual->x][atual->y - 1].valor != 1 && mapa[atual->x - 1][atual->y].valor != 1)
                validaMovimento(-1, -1, &atual, linha, coluna, mapa, fim);
			else{
				printf("\t\t\tVOCE VAI COLIDIR COM UM OBSTACULO..");
				Sleep(1500);
			}
		}else if(!strcmp(mov, "WD") || !strcmp(mov, "DW")){
			if(mapa[atual->x][atual->y + 1].valor != 1 && mapa[atual->x - 1][atual->y].valor != 1)
                validaMovimento(-1, 1, &atual, linha, coluna, mapa, fim);
			else{
				printf("\t\t\tVOCE VAI COLIDIR COM UM OBSTACULO..");
				Sleep(1500);
			}
		}else{
			printf("\t\t\tMOVIMENTO INEXISTENTE...");
			Sleep(1500);
		}
		system("cls");
		
	}while(atual->valor != 3);
	
	return;
}

// funcao para validar o movimento do cego
void validaMovimento(int desX, int desY, struct celula **atual, int linha, int coluna, struct celula mapa[linha][coluna], struct celula *fim){
	// eu trabalho com atual como ponteiro de ponteiro pq eu preciso modificar o valor dele na funcao anterior
	if((((*atual)->x + desX >= 0) && ((*atual)->x + desX < linha)) && (((*atual)->y + desY >= 0) && ((*atual)->y + desY < coluna))){ // se ele estiver dentro do mapa
		switch(mapa[(*atual)->x + desX][(*atual)->y + desY].valor){
			case 0: // se estiver num caminho transitavel
				if(mapa[(*atual)->x + desX][(*atual)->y + desY].caminho){ // se ele ainda estiver no caminho
					mapa[(*atual)->x + desX][(*atual)->y + desY].caminho = 0;
					mapa[(*atual)->x + desX][(*atual)->y + desY].valor = 2;
					mapa[(*atual)->x][(*atual)->y].valor = 0;
					*atual = &mapa[(*atual)->x + desX][(*atual)->y + desY];
				}else{ // se ele sair do caminho
					// zero o mapa
					zeraMapa(linha, coluna, mapa, 0);
					// atribuo o novo inicio
					mapa[(*atual)->x + desX][(*atual)->y + desY].valor = 2;
					// atualizo a posicao atual
					*atual = &mapa[(*atual)->x + desX][(*atual)->y + desY];
					// encontro o caminho novamente
					if(findpath(linha, coluna, mapa, *atual, fim)){
						// compilo o caminho mais curto
						compilarMov(fim);
					}else{
						printf("\t\t\tERRO AO RECALCULAR O CAMINHO..");
						Sleep(1500);
						// zero o mapa
						zeraMapa(linha, coluna, mapa, 0);
						// atribuo o inicio antigo
						mapa[(*atual)->x - desX][(*atual)->y - desY].valor = 2;
						// atualizo a posicao atual
						*atual = &mapa[(*atual)->x - desX][(*atual)->y - desY];
						// encontro o caminho novamente
						if(findpath(linha, coluna, mapa, *atual, fim)){
							// compilo o caminho mais curto
							compilarMov(fim);
						}else{
							printf("\t\t\tSE FODEU");
						}
					}
				}
				break;
			case 1: // se for bater num obstaculo
				printf("\t\t\tVOCE VAI COLIDIR COM UM OBSTACULO..");
				Sleep(1500);
				break;
			default: // se chegar ao fim
				printf("\t\t\tVOCE CHEGOU AO FIM!");
				*atual = &mapa[(*atual)->x + desX][(*atual)->y + desY];
				Sleep(1500);
				break;
		}
	}else{
		printf("\t\t\tLIMITE DO MAPA EXCEDIDO..");
		Sleep(1500);
	}
	
	return;
}

// funcao para procurar o proximo passo do cego
void nextStep(struct celula *father, int lin, int col, struct celula mapa[lin][col]){
	if(father->x + 1 < lin){
		if(mapa[father->x + 1][father->y].caminho){
			printf("BAIXO;\n");
			return;
		}

		if(father->y + 1 < col){
			if(mapa[father->x][father->y + 1].valor != 1 && mapa[father->x + 1][father->y].valor != 1){
				if(mapa[father->x + 1][father->y + 1].caminho){
					printf("DIAGONAL INFERIOR DIREITA;\n");
					return;
				}
			}
		}

		if(father->y - 1 < col && father->y - 1 >= 0){
			if(mapa[father->x][father->y - 1].valor != 1 && mapa[father->x + 1][father->y].valor != 1){
				if(mapa[father->x + 1][father->y - 1].caminho){
					printf("DIAGONAL INFERIOR ESQUERDA;\n");
					return;
				}
			}
		}
	}

	if(father->y + 1 < col && mapa[father->x][father->y + 1].caminho){
		if(mapa[father->x][father->y + 1].caminho){
			printf("DIREITA;\n");
			return;
		}
	}

	if(father->y - 1 < col && father->y - 1 >= 0){
		if(mapa[father->x][father->y - 1].caminho){
			printf("ESQUERDA;\n");
			return;
		}
	}

	if(father->x - 1 < lin && father->x - 1 >= 0){
		if(mapa[father->x - 1][father->y].caminho){
			printf("CIMA;\n");
			return;
		}

		if(father->y + 1 < col){
			if(mapa[father->x][father->y + 1].valor != 1 && mapa[father->x - 1][father->y].valor != 1){
				if(mapa[father->x - 1][father->y + 1].caminho){
					printf("DIAGONAL SUPERIOR DIREITA;\n");
					return;
				}
			}
		}
		if(father->y - 1 < col && father->y - 1 >= 0){
			if(mapa[father->x][father->y - 1].valor != 1 && mapa[father->x - 1][father->y].valor != 1){
				if(mapa[father->x - 1][father->y - 1].caminho){
					printf("DIAGONAL SUPERIOR ESQUERDA;\n");
					return;
				}
			}
		}
	}
}
