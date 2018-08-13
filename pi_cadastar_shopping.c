#include <stdio.h>
#include <stdlib.h>

#define READING "r"
#define WRITING "a"

// funcao para abrir ps arquivos que vamos usar
int abreArquivos(FILE **p_arq_shop, FILE **p_arq_mapa, FILE **p_arq_obs, char *formato);
// funcao para contar quantos registros de mapas tem
void contaMapas(FILE *p_arq_mapa, int *countMapa);
// funcao para contar quantos registros de obstaculos tem
void contaObstaculo(FILE *p_arq_obs, int *countObs);
// funcao para preencher o mapa
void preencheMapa(char nome_shopping[], int tamX, int tamY, int mapa[tamX][tamY]);

int main(void){
	
	// ponteiros para os arquivos
	FILE *arq_shop;
	FILE *arq_mapa;
	FILE *arq_obs;
	
	// abrir os arquivos como leitura
	if(!abreArquivos(&arq_shop, &arq_mapa, &arq_obs, READING)){
		// se algum arquivo nao foi aberto
		printf("ERRO 1");
		return 0;
	}

	// id do registro
		int idRegistroMapa = 1;
		int idRegistroObs = 1;
	
	// contar quantos registro de mapas tem
	contaMapas(arq_mapa, &idRegistroMapa);
	
	// contar quantos registro de obstaculos tem
	contaObstaculo(arq_obs, &idRegistroObs);

	// fecho os arquivos
		fclose(arq_obs);
		fclose(arq_mapa);
		fclose(arq_shop);
	
	// leio o nome do shopping
	char shopping[31];
	printf("DIGITE O NOME DO SHOPPING: ");
	scanf("%30[^\n]s", shopping); setbuf(stdin, NULL);
	
	long int cnpj;
	printf("DIGITE O CNPJ DO SHOPPING: ");
	scanf("%li", &cnpj); setbuf(stdin, NULL);
	
	// leio o tamanho do mapa
	int tamX, tamY;
	printf("DIGITE O TAMANHO DO MAPA:\n");
	printf("LINHAS: ");
	scanf("%d", &tamX);
	setbuf(stdin, NULL);
	printf("COLUNAS: ");
	scanf("%d", &tamY);
	setbuf(stdin, NULL);
	
	// crio o mapa
	int mapa[tamX][tamY], i, j;
	
	// zero o mapa
	for(i = 0; i < tamX; i++){
		for(j = 0; j < tamY; j++){
			mapa[i][j] = 0;
		}
	}
	
	// preencher o mapa
	preencheMapa(shopping, tamX, tamY, mapa);
	
	// abrir os arquivos como escrita
	if(!abreArquivos(&arq_shop, &arq_mapa, &arq_obs, WRITING)){
		// se algum arquivo nao foi aberto
		printf("ERRO 2");
		return 0;
	}
	
	// coloco o shopping na tabela de shoppings
	fprintf(arq_shop, "%li - %s\n", cnpj, shopping);

	// coloco o mapa na tabela de mapas
	fprintf(arq_mapa, "%d - %li - %d - %d\n", idRegistroMapa, cnpj, tamX, tamY);

	// coloco os obstaculos do mapa
	for(i = 0; i < tamX; i++){
		for(j = 0; j < tamY; j++){
			if(mapa[i][j]){
				fprintf(arq_obs, "%d - %d - %d - %d\n", idRegistroObs, idRegistroMapa, i, j);
				idRegistroObs++;
			}
		}
	}

	// fecho os arquivos
		fclose(arq_obs);
		fclose(arq_mapa);
		fclose(arq_shop);
	
	system("pause");
	return 0;
}

// funcao para abrir ps arquivos que vamos usar
int abreArquivos(FILE **p_arq_shop, FILE **p_arq_mapa, FILE **p_arq_obs, char *formato){
	*p_arq_shop = fopen("banco\\shoppings.txt", formato);
	if(*p_arq_shop == NULL){
		printf("Erro ao conectar com a tabela de Shoppings...\n");
		return 0;
	}

	*p_arq_mapa = fopen("banco\\mapas.txt", formato);
	if(*p_arq_mapa == NULL){
		printf("Erro ao conectar com a tabela de Mapas...\n");
		return 0;
	}

	*p_arq_obs = fopen("banco\\pontos_fisicos.txt", formato);
	if(*p_arq_obs == NULL){
		printf("Erro ao conectar com a tabela de Obstaculos...\n");
		return 0;
	}
	
	return 1;
}

// funcao para contar quantos registros de mapas tem
void contaMapas(FILE *p_arq_mapa, int *countMapa){
	int id_mapa;        // id do mapa
	long int cnpj_shop; // cnpj do shopping
	int tamX, tamY;     // tamanho do mapa
	
	fscanf(p_arq_mapa, "%d - %li - %d - %d", &id_mapa, &cnpj_shop, &tamX, &tamY); setbuf(stdin, NULL);
	while(!feof(p_arq_mapa)){
		*countMapa += 1;
		fscanf(p_arq_mapa, "%d - %li - %d - %d", &id_mapa, &cnpj_shop, &tamX, &tamY); setbuf(stdin, NULL);
	}
	
	return;
}

// funcao para contar quantos registros de obstaculos tem
void contaObstaculo(FILE *p_arq_obs, int *countObs){
	int id_obs;     // id do obstaculo
	int id_mapa;    // id do mapa
	int posX, posY; // coordenada do obstaculo
	
	fscanf(p_arq_obs, "%d - %d - %d - %d", &id_obs, &id_mapa, &posX, &posY); setbuf(stdin, NULL);
	while(!feof(p_arq_obs)){
		*countObs += 1;
		fscanf(p_arq_obs, "%d - %d - %d - %d", &id_obs, &id_mapa, &posX, &posY); setbuf(stdin, NULL);
	}
	
	return;
}

// funcao para preencher o mapa
void preencheMapa(char nome_shopping[], int tamX, int tamY, int mapa[tamX][tamY]){
	int valor;  // o que o usuario digita no menu
	int i, j;   // contadores
	int x, y;   // coordenadas para por o valor
	
	do{
		// mostrar o nome do shopping
		system("cls");
		printf("\t\t\t\t\t\t\t%s\n", nome_shopping);

		// mostrar o mapa
		printf("\t\t\t     ");
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

		// menu
		printf("\t\t\t\t\tDIGITE O VALOR QUE DESEJA INSERIR:\n");
		printf("\t\t\t\t\t\t0. Posicao transitavel;\n");
		printf("\t\t\t\t\t\t1. Posicao com obstaculo;\n");
		printf("\t\t\t\t\t\t>1. Encerrar a criacao do mapa.\n");
		printf("\t\t\t\t\t\t->"); scanf("%d", &valor);

		// se ele escolher sair
		if(valor > 1){
			continue;
		}

		// ler as coordenadas
		printf("\t\t\t\t\tDIGITE ONDE O VALOR DEVE SER INSERIDO:\n");
		do{
			printf("\t\t\t\t\tLINHA: "); scanf("%d", &x);
				if((x < 0) || (x >= tamX)){
				printf("\t\t\t\t\t\tVALORES INVALIDOS. DIGITE-OS NOVAMENTE\n");
			}
		}while((x < 0) || (x >= tamX));
		do{
			printf("\t\t\t\t\tCOLUNA: "); scanf("%d", &y);
				if((y < 0) || (y >= tamY)){
				printf("\t\t\t\t\t\tVALORES INVALIDOS. DIGITE-OS NOVAMENTE\n");
			}
		}while((y < 0) || (y >= tamY));

		// inserir o valor
		switch(valor){
			case 0: mapa[x][y] = valor; break;
			case 1: mapa[x][y] = valor; break;
			default: break;
		}
	}while(valor < 2);
	
	return;
}

