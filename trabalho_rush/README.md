
## Demonstração

1: Resumo do projeto:
			
			O projeto se consiste em um simples código que faz um interpretador de 
		shell básico, as funções desse shell se resumem em:
			
			1: 
                Adicionar a saida final de um comando, ou multiplos "pipes" dentro
			    de um arquivo com o operador ">", caso o arquivo não exista, ele   criará,
                caso ja exista, irá sobrepor os dados do arquivo antigo.
			
			2:
                Adicionar a saida final de um comando, ou multiplos "pipes", dentro do 
                final de um arquivo com o operador ">>", caso o arquivo não exista, 
                gerará uma mensagem de erro.
			
			3: 
                fazer o primeiro comando de uma cadeia de pipes, ou o unico comando
			    a executar usar como entrada padrão um arquivo com o operador "<", o 
    			arquivo precisa existir, caso não exista, gerará um erro.
			
			4:
                É possivel mudar o diretorio padrão com a função shell cd, bastando
                digitar cd e o nome do diretorio que deseja ir, caso digitar somente cd
                o interpretador entende que deseja ir para home.
			
			5: 
                O interpretador consegue interpretar o apelido ~/ como /home/user.
			
			6:
                2 ou mais pipes para comunicação entre processos, sendo possível 
                conectar em sequência a saida de um comando na entrada de outro comando.
                Obs: "Foi usado pipes infinitos para essa funcionalidade, isso é, caso
                seja realizado o comando 'x | y | z | a', o programa criara 3 pipes, e 
                não reutilizara o primeiro pipe quando os processos x e y que escreve 
                e lê forem encerrados".

## Apêndice


2: Desisões de projeto:
		
			1: Foi implementado um TAD para fornecer o controle do parser de string,
			 sendo especifido no arquivo types.h.
			 
			2: Funções que manipulam pipes estão todas no arquivo dup.h.
			
			3: O caso base que roda somente um comando esta dentro da main, separado
			das funções que rodam processos que precisam de pipes.
