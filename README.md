# Separador de Tokens
Buscador de tokens de arquivos de texto em C

Criado por Augusto César Bisognin
Versão 1.1 - 13/12/2016
Codificação Western Windows 1252

O usuário informa o nome de uma pasta - deve ser subpasta de onde está o
executável - e o programa buscará por todos os arquivos de texto nesta pasta.
	Para cada arquivo de texto (.txt) lido, o programa criará uma lista das
palavras encontradas.
	Após todos os arquivos serem lidos, será solicitado os termos da busca.

* O usuário pode:
	1. Informar cada termo da busca, separados por ENTER ou ESPAÇOS
		Quando informar '1', as palavras serão buscadas e os resultados serão
		exibidos. Se uma palavra aparecer mais de uma vez no arquivo, ela será
		igualmente apresentada como um novo resultado.
		Será informado se um arquivo contiver todos os termos buscados.
	2. Inserir '2' para exibir uma lista de todas as palavras encontradas nos
		arquivos.
	3. Inserir '0' para encerrar o programa.
