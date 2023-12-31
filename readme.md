# BRASIS - Sistema Operacional Brasileiro

Este sistema operacional tem como objetivo executar em computadores antigos, focado preferencialmente no recorte entre o 386 SX até o Pentium 133, inicialmente suportando maquinas com até 16 MiB de RAM.

Tenho como inspiração alguns projetos modernos de sistema operacional para equipamentos antigos ou mesmo inspirados em sistemas legados como o SymbOS, UZIX, HellenOS, SerenityOS, Haiku.

# Projeto

Para compilar o projeto se deve primeiro seguir os requisítos abaixo, e após instala-los, executar os comandos abaixo:

```sh
chmod +x configure
./configure
make clean
make run
```

* **Sempre que compilar em um computador novo, mesmo que copiando a pasta, execute antes o ```./configure``` pois ele gera apontamentos que funcionam apenas na maquina local**

## Depurando

Para depurar basta executar o comando:

- No macOS:

    ```sh
    make debug_macos
    ```

- No Linux:

    ```sh
    make debug_linux
    ```

### Depurando no macOS Moderno

Recomendo a utilização do Bochs, porém não recomendo a utilização da versão enviada via HomeBrew por suas limitações, neste caso baixe o código fonte diretamente do site e compile com o comando:

**Copie o arquivo ```configure_bochs_macosx``` do diretorio Extra, para o diretorio do código do Bochs**

```sh
chmod +x configure_bochs_macosx
./configure_bochs_macosx  --with-sdl2 --enable-debugger --enable-plugins --enable-cdrom --enable-pci  --enable-show-ips  --enable-x86-64 --enable-fpu --enable-all-optimizations --enable-sb16 --disable-gameport --with-wx
make
sudo make install
```

Ira gerar uma versão com o suporte mínimo de emulação e depuração com suporte as interface SDL2, porém sem suporte a Rede.

## Observações para Desenvolvedores

- **Núcleo e Módulos Controladores com código 16/32bits misto**: Sempre observar os BUGS presentes nos processadores, principalmente os que afetam o 386 e 486, pois são difíceis de contornar, um exemplo é no estágio 2 da inicialização, existem varios comandos ```a32 nop``` usados para contornar erros das versões iniciais do 386 que crashavam ao mudar repentinamente de 32 bits para 16 bits.

- Para evitar distorções, a paleta de cores em 8bits, será RGB de 6 Bits e o restante do espaço uma paleta monocromatica, limita um pouco o RGB porém as cores ficam mais fieis ao original RGB ao diminuir a escala.

## Requisitos para Desenvolvimento

- Unix-like: macOS, Linux, FreeBSD ou WSL
- NASM
- [SmallerC](https://github.com/humbertocsjr/SmallerC)
- [MinixFS Tool](https://github.com/humbertocsjr/minixfs)
- DOSBox (No macOS usar a versão do [Homebrew](https://brew.sh))
- Bochs

## Ferramentas Recomendáveis

- Qemu
- Bochs com Depuração
- mtools (Suporte a FAT)
- 86box
- Visual Code

## Estrutura do Projeto

- **Distro**: Imagens geradas durante a compilação
- **Incluir**: Pasta global de Include da linguagem C
- **Nucleo**: Núcleo do Sistema e seus Módulos
- **Inicial**: Sistema de Inicialização, contendo todos as variações do Estágio 1 e o Estágio 2
- **Programas**: Programas do BRASIS

## Funcionamento durante a inicialização

- Ao iniciar é carregado pela BIOS o estágio 1 do sistema de inicialização, que é responsável por carregar do disco o estágio 2 na posição 0x6000 da memória RAM e executá-lo.

- Executando o estágio 2, este é responsável por abrir o arquivo de configuração na raiz do disco, e carregar o núcleo e uma imagem de disco contendo os módulos para a memória, posteriormente executando o núcleo.

- O Núcleo é responsável por inicializar os Módulos conforme sua ordem de relevancia.

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
    - Status: Inicializa estruturas, calcula memória livre, e aloca memória em endereçamento fisico
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

### Completo

- Sistema de Arquivos UNIX Archive (AR) (Usado na inicialização)

### Parciais

- Sistemas de Arquivos MinixFS (Versões 1 e 2)
    - Status: Implementado suporte a Versão 1 com 30 caracteres de nome
- Sistemas de Arquivos FAT (Versões 12, 16 e 32)
    - Status: Apenas Primeiro estágio da inicialização em FAT12

## Interface Gráfica

### Pendentes

- Gerenciador de Video
- Biblioteca Gráfica
- Gerenciador de Programas (Interface principal com o usuário)