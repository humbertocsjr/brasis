# BRASIS - Sistema Operacional Brasileiro

Este sistema operacional tem como objetivo executar em computadores antigos, focado preferencialmente no recorte entre o 386 SX até o Pentium 133, inicialmente suportando maquinas com até 16 MiB de RAM.

# Funcionamento

Ao iniciar é carregado pela BIOS o estágio 1 do sistema de inicialização, que é responsável por carregar do disco o estágio 2 na posição 0x6000 da memória RAM e executá-lo.

Executando o estágio 2, este é responsável por abrir o arquivo de configuração na raiz do disco, e carregar o núcleo e uma imagem de disco contendo os módulos para a memória, posteriormente executando o núcleo.

O Núcleo é responsável por inicializar os Módulos conforme sua ordem de relevancia.

# Objetivos

- Ser Open Source (De verdade, usando apenas licenças BSD ou Dominio Publico, jamais GPL)
- Código Fonte em Português, afinal o mundo da tecnologia já em em inglês, e muita coisa da retrocomputação está ou em espanhol ou em japonês, e falta código fonte facilmente legível por lusófonos.
- Ser leve o suficiente para executar em um 386.
- Ter uma interface gráfica inspirada na no inicio dos anos 90, principalmente no Windows 3.11 e OS/2 3.0, porém com um menu "Iniciar", usar como inspiração no BeOS/Haiku, CDE
- Ter o próprimo ambiente de desenvolvimento mínimo baseado no SmallerC, (Testar a portabilidade do YASM, NASM ou em último caso do FASM)
- Ser totalmente independente de softwares modernos e pesados, reescrevendo sempre que possível os programas com enfase em gerar o código mais enxuto possível e com menor consumo de memória.
- Usar padrões e tecnologias modernas sempre que possível, por exemplo, usar Markdown como linguagem de documentação, YAML ou JSON como formato de dados, UTF-8 para armazenar texto.
- Implementar aos poucos suporte para Hardwares antigos, usar 86box como inspiração e consulta do comportamento dos equipamentos.

# Compatibilidade

| Componente | Requisitos Mínimos | Recomendado |
|------------|--------------------|-------------|
| Processador| 386 SX 16 Mhz      | 386 SX 25 Mhz |
| RAM        | 3 MiB              | 8 MiB       |
| Disquete   | 3 1/2 - 1.44 MiB   | 3 1/2 - 1.44 MiB |

# Requisitos para Desenvolvimento

- Unix-like: macOS, Linux, FreeBSD ou WSL
- NASM
- [SmallerC](https://github.com/humbertocsjr/SmallerC)
- [MinixFS Tool](https://github.com/humbertocsjr/minixfs)
- DOSBox

## Recomendável

- mtools (Suporte a FAT)
- 86box
- Visual Code

# Extensões de Arquivos

| Extensão | Descrição |
|----------|-----------|
| .Prog | Programa / Aplicativo |
| .Modulo | Módulo do Núcleo |
| .Nucleo | Núcleo do sistema |
| .Inicial | Sistema Inicial |
| .Imagem | Imagem de Disco |
| .bin | Binario 16 bits |
| .cfg | Arquivo de configuração |
| .tga | Imagem Targa |
| .md | Documento de Texto MarkDown|


# Situação do Projeto

## Projeto

### Pendente

- Suporte a Tradução

## Núcleo: Base

### Completo

- Leitura de Parâmetros do Gerenciador de Inicialização

### Parciais

- Gerenciador de Memória
    - Status: Inicializa estruturas e calcula memória livre
- Interface de Depuração
    - Status: Emissor de mensagens de inicialização em video texto

### Pendentes

- Multitarefa
- Gerenciador de Processos

## Núcleo: Suporte a Hardware

### Parciais

- Disquete
    - Status: Apenas na Inicialização, faltando Modulo do Núcleo

### Pendentes

- Video EGA/VGA 16 cores
- Video SVGA Clássico (640x480, 800x600, 1024x768) a 16 bits
- Video VBE 2.0 (Modos diversos)
- Disco IDE
- Disco SATA
- PCI
- PCMCIA / PCCard
- Rede NE2000
- Rede Orinoco Silver/Gold

## Núcleo: Modulos Internos

### Parciais

- Sistemas de Arquivos MinixFS (Versões 1 e 2)
    - Status: Implementado suporte a Versão 1 com 30 caracteres de nome
- Sistemas de Arquivos FAT (Versões 12, 16 e 32)
    - Status: Apenas Primeiro estágio da inicialização em FAT12

### Pendente

- Sistema de Arquivos CPIO (Usado na inicialização)

## Interface Gráfica

### Pendentes

- Gerenciador de Video
- Biblioteca Gráfica
- Gerenciador de Programas (Interface principal com o usuário)