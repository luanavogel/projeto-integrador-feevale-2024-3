Este programa em C tem como objetivo monitorar o arquivo de log do sistema, especificamente o /var/log/syslog, para capturar e registrar eventos de rede relacionados ao NetworkManager, como conexões e desconexões.
Os eventos identificados são armazenados em uma lista duplamente encadeada e podem ser gerenciados através de um menu interativo que oferece funcionalidades como listar, filtrar, remover e contar os eventos registrados.

O programa oferece as seguintes funcionalidades:

* Monitoramento do log do sistema: O programa lê o arquivo /var/log/syslog para identificar eventos de rede, como conexões (quando o dispositivo se conecta à rede) e desconexões (quando o dispositivo se desconecta).
* Armazenamento dos eventos: Cada evento identificado é armazenado em uma estrutura de lista duplamente encadeada, facilitando o gerenciamento dos eventos.
* Menu interativo: O usuário pode interagir com o programa através de um menu que oferece as seguintes opções:
1) Listar todos os eventos registrados.
2) Filtrar eventos por tipo (conexão ou desconexão).
3) Remover um evento específico com base em seu código.
4) Contar o número total de eventos registrados.

Requisitos:
* Sistema operacional Linux.
* Acesso ao arquivo /var/log/syslog (normalmente, o programa deve ser executado como superusuário).
