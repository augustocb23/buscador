/*	BUSCADOR DE TOKENS EM ARQUIVOS DE TEXTO
Criado por Augusto C�sar Bisognin
Vers�o 1.1 - 13/12/2016
Codifica��o Western Windows 1252

	O usu�rio informa o nome de uma pasta - deve ser subpasta de onde est� o 
execut�vel - e o programa buscar� por todos os arquivos de texto nesta pasta.
	Para cada arquivo de texto (.txt) lido, o programa criar� uma lista das
palavras encontradas.
	Ap�s todos os arquivos serem lidos, ser� solicitado os termos da busca.

O usu�rio pode:
	1. Informar cada termo da busca, separados por ENTER ou ESPA�OS
		Quando informar '1', as palavras ser�o buscadas e os resultados ser�o
		exibidos. Se uma palavra aparecer mais de uma vez no arquivo, ela ser�
		igualmente apresentada como um novo resultado.
		Ser� informado se um arquivo contiver todos os termos buscados.
	2. Inserir '2' para exibir uma lista de todas as palavras encontradas nos
		arquivos.
	3. Inserir '0' para encerrar o programa.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

/*declara��es de listas encadeadas*/
struct Lista {
	void* ponteiro;
	struct Lista* prox;
};
typedef struct Lista lista;
lista* lista_cria();
lista* lista_adiciona(lista* anterior, void* item);
void lista_apaga(lista* item);
bool lista_compara(lista* termo1, lista* termo2);
/*converter de um vetor para uma nova string*/
char* cria_palavra(char* palavra);
char* aloca_palavra(char* palavra, int tamanho);
/*declara��o de um arquivo*/
struct Arquivo {
	char* nome;
	lista* palavras;
};
typedef struct Arquivo arquivo;
arquivo* cria_arquivo(char* nome);

/*declara��es das fun��es principais*/
lista* le_pasta(char* pasta, char* delimitadores);
void busca_palavras(lista* pasta, lista* termos);
void imprime_lista(lista* pasta);

int main(){
	setlocale (LC_ALL, "portuguese"); /*Define a codifica��o*/
	char palavra[64]; /*tamanho m�ximo de cada palavra*/
	char delimitadores[32]="\n':/|\\!?. ()[]{},;-\""; /*lista de delimitadores dos tokens*/

	/*l� o nome da pasta*/
	printf("Digite o nome da pasta: "); scanf("%s",palavra);
	
	/*carrega a lista de palavras*/
	printf("\n");
	lista* pasta = le_pasta(palavra, delimitadores);

	/*le os termos da busca e salva em uma lista*/
	while (true){
		printf("\n");
		printf("Digite os termos a serem buscados:\n");
		printf("Insira 1 para continuar, 2 para listar todas as palavras ou 0 para sair.\n");
		lista* termos = lista_cria(); /*cria uma lista encadeada para salvar os termos*/
		lista* busca = termos;
		scanf("%s",palavra);
		/*se digitar '1', busca os termos*/
		if (strcmp(palavra,"1") == 0){
			printf("\tNenhum termo inserido!\n");
			continue;
		}
		/*se digitar '2', exibe uma lista completa dos tokens encontrados*/
		if (strcmp(palavra,"2") == 0){
			printf("\n");
			printf("Palavras encontradas:\n");
			imprime_lista(pasta);
			continue;
		}
		/*se digitar '0', o programa � encerrado*/
		if (strcmp(palavra,"0") == 0)
			return 0;
		/*copia o termo lido para uma string dinamicamente alocada*/
		termos->ponteiro = cria_palavra(palavra);
		/*continua lendo e adicionando � lista at� ser digitado '0', '1' ou '2'*/
		while (true){
			scanf("%s",palavra);
			if (strcmp(palavra,"1") == 0)
				break;
			if (strcmp(palavra,"2") == 0){
				printf("\n");
				printf("Palavras encontradas:\n");
				imprime_lista(pasta);
				continue;
			}
			if (strcmp(palavra,"0") == 0)
				return 0;
			termos = lista_adiciona(termos,cria_palavra(palavra));
		}
		printf("\n");
		/*busca a lista de palavras lidas*/
		busca_palavras(pasta,busca);
		/*apaga a lista de palavras para permitir uma nova busca*/
		lista_apaga(busca);
	}
	
	return 0;
}
/*l� todos os arquivos de uma pasta e salva em uma lista*/
lista* le_pasta(char* pasta, char* delimitadores){
	/*declara a pasta e arquivo*/
	DIR *dir;
	struct dirent *pasta_atual;
	FILE* arquivo_aberto;
	pasta=aloca_palavra(pasta,1);
	strcat(pasta,"/");
	/*abre a pasta e declara as strings*/
	dir = opendir(pasta);
	if (dir == NULL){
		printf("Falha ao abrir a pasta!\n");
		printf("\tVerifique se o nome foi digitado corretamente e se voce possui\n\tos privil�gios necess�rios para acessar a pasta.\n");
		exit(6);
	}
	char* palavra;
	/*cria a lista de arquivos*/
	lista* arquivos = lista_cria();
	lista* primeiro_arquivo = arquivos;
	bool primeiro = true;
	/*busca cada arquivo na pasta e salva na lista*/
	printf("Buscando todas as palavras em %s...\n",pasta);
	while ((pasta_atual=readdir(dir))!= NULL) {
		/*verifica se n�o s�o as pastas padr�o (/. e /..)*/
		if (strcmp(".",pasta_atual->d_name)==0 || strcmp("..",pasta_atual->d_name)==0)
			continue;
		/*verifica se � um arquivo de texto*/
		char* ext = strrchr(pasta_atual->d_name,'.');
		if (strcmp(ext,".txt") != 0){
			printf("\t%s\tIgnorado\n",pasta_atual->d_name);
			continue;
		}
		/*carrega o nome completo do arquivo*/
		char* nome = aloca_palavra(pasta,strlen(pasta_atual->d_name)+1);
		printf("\t%s",pasta_atual->d_name);
		strcat(nome, pasta_atual->d_name);
		/*abre o arquivo e testa*/
		arquivo_aberto = fopen(nome,"r");
		if (arquivo_aberto==NULL){
			printf("\tFalha ao abrir arquivo\n");
			free(nome);
			continue;
		}
		/*adiciona o arquivo � lista e aloca a lista de palavras*/
		arquivo* arquivo_atual = cria_arquivo(nome);
		lista* lista_palavras = lista_cria();
		arquivo_atual->nome=nome;
		arquivo_atual->palavras=lista_palavras;
		/*verifica se � o primeiro arquivo da lista*/
		if (primeiro){
			arquivos->ponteiro = arquivo_atual;
			primeiro=false;
		}
		else
			arquivos=lista_adiciona(arquivos, arquivo_atual);
		/*l� o arquivo e separa os tokens, adicionando-os � lista*/
		printf("\tlendo...");
		bool primeira_iteracao = true;
		while (!feof(arquivo_aberto)){
			/*l� uma linha do arquivo e copia para o buffer*/
			char buffer[1024];
			if (fgets(buffer,1024,arquivo_aberto) == NULL)
				continue;
			/*busca o primeiro token e testa se n�o era uma linha em branco*/
			palavra=strtok(buffer,delimitadores);
			if (palavra == NULL)
				continue;
			/*verifica se � a primeira palavra do buffer e copia para a lista*/
			if (primeira_iteracao){
				lista_palavras->ponteiro=cria_palavra(palavra);
				primeira_iteracao = false;
			}
			else
				lista_palavras=lista_adiciona(lista_palavras,cria_palavra(palavra));
			/*continua separando cada palavra e adicionando na lista*/
			while (true){
				palavra=strtok(NULL,delimitadores);
				if (palavra != NULL)
					lista_palavras=lista_adiciona(lista_palavras,cria_palavra(palavra));
				else
					break;
			}
		}
		/*fecha o arquivo*/
		printf("\tlido!\n");
		fclose(arquivo_aberto);
	}
	/*fecha a pasta*/
	closedir(dir);
	printf("Leitura de arquivos conclu�da com sucesso.\n");
	/*retorna a lista de arquivos lidos*/
	return primeiro_arquivo;
}
/*recebe uma lista de arquivos e busca por uma lista de palavras*/
void busca_palavras(lista* pasta, lista* termos){
	/*declara��o das listas*/
	lista* arquivos = pasta;
	arquivo* arquivo_atual = arquivos->ponteiro;
	lista* palavra;
	lista* palavras;
	printf("Buscando o(s) termo(s) da lista...\n");
	/*para cada arquivo informado*/
	while (arquivos != NULL){
		/*cria uma lista para salvar os resultados*/
		lista* encontrados = lista_cria();
		lista* encontrados_inic = encontrados;
		bool primeiro = true;
		int i = 0;
		/*carrega a lista de palavras*/
		arquivo_atual = arquivos->ponteiro;
		palavras = arquivo_atual->palavras;
		printf("\t%s:\n",arquivo_atual->nome);
		/*para cada palavra do arquivo, compara com as palavras da busca*/
		while (palavras != NULL){
			palavra = termos;
			while (palavra != NULL){
				/*se encontrar, incrementa o contador e salva na lista*/
				if (strcmp(palavra->ponteiro,palavras->ponteiro) == 0){
					printf("\t\t%s\n",(char*)palavra->ponteiro);
					i++;
					if (primeiro){
						encontrados->ponteiro = palavra->ponteiro; 
						primeiro = false;
					} else
						encontrados = lista_adiciona(encontrados, palavra->ponteiro);
				}
				palavra = palavra->prox;
			}
			palavras = palavras->prox;
		}
		/*exibe o n�mero de resultados encontrados*/
		if (i == 0)
			printf("\t\t\tNenhum resultado.\n");
		else {
			printf("\t\t\t%d resultado(s).\n", i);
			/*verifica se todos os termos foram encontrados no arquivo*/
			if (lista_compara(termos, encontrados_inic))
				printf("\t\t\tO arquivo cont�m todas as palavras buscadas.\n");
		}
		lista_apaga(encontrados);
		arquivos = arquivos->prox;
	}
	printf("Busca conclu�da.\n");
	return;
}
/*imprime todas as palavras que est�o na lista*/
void imprime_lista(lista* pasta){
	while (pasta != NULL){
		/*declara as listas*/
		arquivo* arquivo_atual = pasta->ponteiro;
		printf("\t%s\n",arquivo_atual->nome);
		lista* palavra = arquivo_atual->palavras;
		/*imprime cada string encontrada*/
		while (palavra != NULL){
			printf("\t\t%s\n", (char*)palavra->ponteiro);
			palavra=palavra->prox;
		}
		pasta=pasta->prox;
	}
	printf("Fim da lista.\n");
	return;
}

/*FUN��ES SECUND�RIAS*/
	/*aloca um arquivo*/
	arquivo* cria_arquivo(char* nome){
		arquivo* ponteiro = malloc(sizeof(arquivo));
		if (ponteiro==NULL){
			printf("Mem�ria insuficiente!\n\tFalha ao alocar mem�ria para a lista de arquivos.\n");
			exit(10);
		}
		ponteiro->palavras = NULL;
		return ponteiro;
	}
	/*implementa��o de listas encadeadas de ponteiros*/
	/*cria uma nova lista*/
	lista* lista_cria(){
		lista* ponteiro = malloc(sizeof(lista));
		if (ponteiro==NULL){
			printf("Mem�ria insuficiente!\n\tEncerre alguns aplicativos e tente novamente.\n");
			exit(11);
		}
		ponteiro->prox=NULL;
		return ponteiro;
	}
	/*adiciona um novo item � lista e avan�a para o pr�ximo*/
	lista* lista_adiciona(lista* anterior, void* item){
		lista* novo = malloc(sizeof(lista));
		if (novo == NULL){
			printf("Mem�ria insuficiente!\n\tEncerre alguns aplicativos e tente novamente.\n");
			exit(12);
		}
		novo->ponteiro=item;
		anterior->prox = novo;
		novo->prox = NULL;
		return novo;
	}
	/*apada todos os itens de uma lista*/
	void lista_apaga(lista* inic){
		while (inic != NULL){
			lista* prox = inic->prox;
			if (inic->ponteiro != NULL)
				free(inic->ponteiro);
			free(inic);
			inic = prox;
		}
		return;
	}
	/*verifica se todos os itens da lista1 est�o contidos na lista2*/
	bool lista_compara(lista* termo1, lista* termo2){
		lista* lista1 = termo1;
		while (lista1 != NULL){
			bool validado = false;
			lista* lista2 = termo2;
			while (lista2 != NULL){
				if (strcmp(lista1->ponteiro,lista2->ponteiro) == 0)
					validado = true;
				lista2 = lista2->prox;
			}
			if (!validado)
				return false;
			lista1 = lista1->prox;
		}
		return true;
	}
	/*copia uma string para um novo endere�o de mem�ria dinamicamente alocado*/
	char* cria_palavra(char* palavra){
		char* string = malloc(sizeof(char)*strlen(palavra));
		if (string==NULL){
			printf("Mem�ria insuficiente!\n\tLibere mais memoria e tente novamente.\n");
			exit(13);
		}
		strcpy(string,palavra);
		return string;
	}
	/*recebe uma string e um tamanho para expandi-la*/
	char* aloca_palavra(char* palavra, int tamanho){
		char* string = malloc(sizeof(char)*(strlen(palavra)+tamanho));
		if (string==NULL){
			printf("Mem�ria insuficiente!\n\tLibere mais memoria e tente novamente.\n");
			exit(14);
		}
		strcpy(string,palavra);
		return string;
	}