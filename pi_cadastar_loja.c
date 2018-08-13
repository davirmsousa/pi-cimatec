#include <stdio.h>
#include <stdlib.h>

// funcao para abrir ps arquivos que vamos usar
int abreArquivos(FILE **p_arq_shop, FILE **p_arq_loj, FILE **p_arq_mapa, FILE **p_arq_obs);
// funcao para ler o shopping ao qual a loja pertence
int lerShopping(FILE *p_arq_shop, int *escolha);
// funcao para retornar o cnpj do shopping escolhido
int pegaCnpjShopping(FILE *p_arq_shop, long int *shop_cnpj, int indice_escolhido);
// funcao para pegar o tamanho do mapa
int pegaTamMapa(FILE *p_arq_mapa, long int shop_cnpj, int *tamX, int *tamY, int *id_mapa);
// funcao para pegar os obstaculos e por na matriz
void pegaObstaculos(FILE *p_arq_obs, int tamY, int tamX, int mapa[tamX][tamY], int id_mapa);
// funcao para informar a entrada da loja
void pegaEntradaLoja(char nome_loj[], int tamY, int tamX, int mapa[tamX][tamY], int *posx, int *posy);

int main(void){
	
	// ponteiros para os arquivos
		FILE *arq_shop;
		FILE *arq_loj;
		FILE *arq_mapa;
		FILE *arq_obs;
	
	// abrir os arquivos
	if(!abreArquivos(&arq_shop, &arq_loj, &arq_mapa, &arq_obs)){
		// se algum arquivo nao foi aberto
		printf("ERRO 1");
		return 0;
	}
	
	// guarda o shopping escolhido pelo user
		int escolha = 0;
	
	// ler o shopping que a loja pertence
	if(!lerShopping(arq_shop, &escolha)){
		// se o arquivo de shoppings estiver vazio
		printf("ERRO 2");
		return 0;
	}
	
	system("cls");
	
	// ler e armazenar o nome da loja
	char nome_loj[31];
	printf("QUAL O NOME DA LOJA? ");
	scanf("%30[^\n]s", nome_loj); setbuf(stdin, NULL);
	
	// ler e armazenar o cnpj da loja
	long int loj_cnpj;
	printf("QUAL O CNPJ DA LOJA? ");
	scanf("%li", &loj_cnpj); setbuf(stdin, NULL);
	
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
	
	// guarda o tamanho do mapa
		int tamX, tamY;

	// guarda o id do mapa
		int id_mapa;
	
	// pego o tamanho do mapa do shopping escolhido
	if(!pegaTamMapa(arq_mapa, shop_cnpj, &tamX, &tamY, &id_mapa)){
		// se nao pegou o tamanho
		printf("ERRO 4");
		return 0;
	}
	
	// crio o mapa do sopping
	int mapa[tamX][tamY], i, j;
	
	// zero o mapa
	for(i = 0; i < tamX; i++){
		for(j = 0; j < tamY; j++){
			mapa[i][j] = 0;
		}
	}
	
	// insiro os obstaculos na matriz
	pegaObstaculos(arq_obs, tamY, tamX, mapa, id_mapa);
					
	int posx, posy; // coordenadas da loja no mapa
	
	// leio as coordenasdas digitadas pelo usuario para a entrada da loja
	pegaEntradaLoja(nome_loj, tamY, tamX, mapa, &posx, &posy);
	
	// inserir os valores na table lojas
	fprintf(arq_loj, "%li - %li - %d - %d - %s\n", loj_cnpj, shop_cnpj, posx, posy, nome_loj);
	
	// fecho os arquivos
		fclose(arq_obs);
		fclose(arq_loj);
		fclose(arq_mapa);
		fclose(arq_shop);
	
	printf("\n\n");
	system("pause");
	return 0;
}

// funcao para abrir ps arquivos que vamos usar
int abreArquivos(FILE **p_arq_shop, FILE **p_arq_loj, FILE **p_arq_mapa, FILE **p_arq_obs){
	// abro os arquivos como leitura
	*p_arq_shop = fopen("banco\\shoppings.txt", "r");
	if(*p_arq_shop == NULL){
		printf("Erro ao conectar com a tabela de Shoppings...\n");
		return 0;
	}

	*p_arq_loj = fopen("banco\\lojas.txt", "a");
	if(*p_arq_loj == NULL){
		printf("Erro ao conectar com a tabela de lojas...\n");
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
	printf("A QUAL SHOPPING A LOJA PERTENCE?\n");
	fscanf(p_arq_shop, "%li - %30[^\n]s", &shop_cnpj, nome_shop); setbuf(stdin, NULL);
	while(!feof(p_arq_shop)){
		printf("\t%d. %s;\n", indice, nome_shop);
		encontrado = 1;
		indice++;
		fscanf(p_arq_shop, "%li - %30[^\n]s", &shop_cnpj, nome_shop); setbuf(stdin, NULL);
	}
	
	// se a lista estiver vazia
	if(!encontrado){
		printf("NAO HA SHOPPINGS CADASTRADOS...\nO PROCESSO FOI INTERROMPIDO.");
		return 0;
	}

	// tratamento de erro da escolha do shopping
	do{
		printf("-> ");
		scanf("%d", escolha); setbuf(stdin, NULL);
		if(*escolha < 1 || *escolha > indice - 1){
			printf("INDICE NAO EXISTE...\n");
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

// funcao para pegar o tamanho do mapa
int pegaTamMapa(FILE *p_arq_mapa, long int shop_cnpj, int *tamX, int *tamY, int *id_mapa){
	long int mapa_cnpj; // cnpj do shopping como chave estrangeira
	int encontrado = 0;	// diz se algum registro foi encontrado
	
	fscanf(p_arq_mapa, "%d - %li - %d - %d", id_mapa, &mapa_cnpj, tamX, tamY); setbuf(stdin, NULL);
	while(!feof(p_arq_mapa)){
		if(shop_cnpj == mapa_cnpj){
			encontrado = 1;
			break;
		}
		fscanf(p_arq_mapa, "%d - %li - %d - %d", id_mapa, &mapa_cnpj, tamX, tamY); setbuf(stdin, NULL);
	}
	
	return encontrado;
}

// funcao para pegar os obstaculos e por na matriz
void pegaObstaculos(FILE *p_arq_obs, int tamY, int tamX, int mapa[tamX][tamY], int id_mapa){
	int obs_id, mapa_id, i, j;
	
	fscanf(p_arq_obs, "%d - %d - %d - %d", &obs_id, &mapa_id, &i, &j); setbuf(stdin, NULL);
	while(!feof(p_arq_obs)){
		if(mapa_id == id_mapa){
			mapa[i][j] = 1;
		}
		fscanf(p_arq_obs, "%d - %d - %d - %d", &obs_id, &mapa_id, &i, &j); setbuf(stdin, NULL);
	}
	
	return;
}

// funcao para informar a entrada da loja
void pegaEntradaLoja(char nome_loj[], int tamY, int tamX, int mapa[tamX][tamY], int *posx, int *posy){
	int i, j;       // contadores pra matriz
	int x, y;       // posicao escolhida pelo usuario
	int ok3 = 0;    // diz se ja digitou a posicao da loja
	
	do{
		// limpa a tela e mostra o nome da loja
		system("cls");
		printf("\t\t\t\t\t\t\t%s\n\n", nome_loj);
		printf("\t\t\t     ");
		
		// mostra a matriz
		for(j = 0; j < tamY; j++){
			printf("% 3d  ", j);
		}
		printf("\n");
		for(i = 0; i < tamX; i++){
			printf("\t\t\t% 3d |", i);
			for(j = 0; j < tamY; j++){
				switch (mapa[i][j]) {
		        	case 1: printf("%c%c%c%c%c", 219, 219, 219, 219, 219); break;
		        	case 2: printf("  %c  ", 446); break;
		        	case 3: printf("  %c  ", 414); break;
		        	default: printf("     "); break;
		        }
			}
			printf("|\n\n");
		}

		// ler as coordenadas
		printf("DIGITE A POSICAO DA ENTRADA DA LOJA (-1 -1 para sair):\n");
		do{
			printf("LINHA: ");
			scanf("%d", &x);
			if((x < 0 && x != -1) || x >= tamX){
				printf("POSICAO INVALIDA...\n");
			}
		}while((x < 0 && x != -1) || x >= tamX);
		do{
			printf("COLUNA: ");
			scanf("%d", &y);
			if((y < 0 && y != -1) || y >= tamY){
				printf("POSICAO INVALIDA...\n");
			}
		}while((y < 0 && y != -1) || y >= tamY);

		// tratar os valores
		if(x == -1 && y == - 1){
			if(!ok3){
				printf("VOCE NAO DIGITOU A POSICAO DA LOJA...");
				Sleep(1500);
			}
		}else if(x == -1 || y == - 1){
			printf("POSICAO INVALIDA...\n");
			Sleep(1500);
		}else{
			if(mapa[x][y] == 0){ // vejo se a posicao eh um obstaculo
				if(ok3){ // vejo se ele ja digitou a posicao antes
					for(i = 0; i < tamX; i++){
						for(j = 0; j < tamY; j++){
							if(mapa[i][j] == 3){ // ao encontrar a posicao antiga eu zero
								mapa[i][j] = 0;
							}
						}
					}
				}
				// digo que ele ja digitou a posicao
				ok3 = 1;
				// gravo a posicao no mapa
				mapa[x][y] = 3;
				// guardo as coordenadas pra colocar no arquivo
				*posx = x;
				*posy = y;
			}else{
				printf("POSICAO INVALIDA...\n");
				Sleep(1500);
			}
		}
	}while((x != -1 && y != -1) || !ok3);
	
	return;
}

