<h1>Escalonador de Processos - Sistemas Operacionais </h1>
<hr>
Seja um sistema operacional que possui um escalonador de processos e considera um número limitado de recursos disponíveis, sendo que quatro desses recursos são CPUs. Neste sistema,  todas as CPUs compartilham a memória principal (RAM). 
<br>
O sistema possui os seguintes recursos:
<ul>
  <li>quatro CPUs;</li>
  <li>quatro discos;</li>
  <li>32gb de memória RAM.</li>
</ul>
<h3>Escalonador de curto prazo:</h3>
Os processos que são submetidos vão ser escalonados de acordo com  a seguinte politica <i>feedback</i> considerando as seguintes características:
<ul>
  <li>Quatro filas de processos prontos que são manipuladas de acordo com a especificação do <i>feedback</i>;</li>
  <li>O quantum definido é de 3 unidades de tempo (defina sua unidade de tempo).</li>
</ul>
<b>Processos de usuários</b> podem usar qualquer um dos recursos, mas o sistema é notificado de que recursos o processo vai precisar no momento de sua submissão. O despachante assegura que cada recursos solicitado está apenas disponível para o processo requisitante durante toda a sua execução.
<h3>Alocação de memória:</h3>
A imagem do processo  deve ser toda carregada para memória quando disponível. Quando o processo terminar, aquele espaço de memória deve ser liberado.  Deve ser proposto uma forma de gerenciar essa memória, alocando o espaço nela e não simplesmente contabilizando a memória alocada.
<h3>Processos:</h3>
Processos no sistema em questão são criados pelo despachante. Mensagens devem ser apresentadas nas seguintes situações:
<ul>
  <li>Uma mensagem que mostra a identificação do processo a ser criado;</li>
  <li>Uma mensagem relata quando um processo muda de estado. Exemplo "Processo #id de: <i>estado-atual</i> para <i>novo-estado</i>", onde #id é a identificação do processo.</li>
  <li>Todo processo ao ser executado passa por três fases:</li>
  <ul>
    <li>fase 1 de CPU;</li>
    <li>fase de I/O;</li>
    <li>fase 2 de CPU.</li>
  </ul>
</ul>
OBS: o percentual de CPU e da I/O será dado no arquivo de entrada que simula a criação (com características) dos processos a serem executadas (dado abaixo).
<h3>Filas:</h3>
De acordo com o estado do processo ao longo de sua vida no sistema, o processo pode estar em algum tipo de fila. É necessário definir as filas tanto para implementar a política de escalonamento, como para manipular a espera por eventos que venham a ocorrer.
<hr>
<h3>Entrada do simulador:</h3>
Uma lista de processos e seus eventos descreve informações sobre os processos a serem executados pelo sistema. A lista deve estar contida em um arquivo de texto que serve como entrada para o seu sistema. Cada linha da lista descreve um processo com os seguintes dados, delimitados por vírgula + espaço:
<br>
<b>(Momento Chegada), (duração de CPU fase 1), (duração de I/O), (duração de CPU fase 2), (#Mbytes), (#disco)</b>
<hr>
Alunos:
<br>

| Nome            | GitHub                  | Email                  |
| --------------- | ----------------------- | ---------------------- |
| Amanda S. Zírpolo      | https://github.com/amandazirpolo  | amandazirpolo@id.uff.br |
| Gabriel J. Panza      | https://github.com/Gabriel-Panza/Gabriel-Panza  | gabrieljp@id.uff.br |
| Gustavo C. M. de Medeiros      | https://github.com/GusMedeiros  | gustavomedeiros@id.uff.br |
| João Vitor de Santana      | https://github.com/Jvssaa  | santanajoao@id.uff.br |
| João Vitor de Moraes      | https://github.com/JohKemPo/JohKemPo  | joaovitormoraes@id.uff.br |

<h6>Professora: <a href="https://www.ic.uff.br/blog/pessoas/maria-cristina-silva-boeres/">Maria Cristina Boeres</a></h6>

<h2 align="center"> <img = src="![image](https://github.com/amandazirpolo/escalonador-de-processos/assets/123826914/e93c2b4f-5707-4b13-b576-e8b965ad1166)
" width=20%> <img = src="![image](https://github.com/amandazirpolo/escalonador-de-processos/assets/123826914/2b1929ff-dd4e-440a-96db-a2079a4e52b9)
" width=18%>
<hr>
<i align="center">Universidade Federal Fluminense, 2024</i>
