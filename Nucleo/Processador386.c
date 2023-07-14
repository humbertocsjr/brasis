#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Debug.h"
#include "Semaforo.h"

#pragma pack(1)

SEMAFORO(_Processador_Semaforo);

// == GDT =======================================

#define GDT_TABELA_QTD 5

#define GDT_MARCADOR_ACESSO 0x1          //00000001
#define GDT_MARCADOR_GRAVAVEL 0x2        //00000010
#define GDT_MARCADOR_APENAS_LEITURA 0    //00000010 ZERO
#define GDT_MARCADOR_EXPANSAO 0x4        //00000100
#define GDT_MARCADOR_CODIGO 0x8          //00001000
#define GDT_MARCADOR_DADOS 0             //00001000 ZERO
#define GDT_MARCADOR_CODIGO_OU_DADOS 0x10//00010000
#define GDT_MARCADOR_ANEL0 0x00          //00000000
#define GDT_MARCADOR_ANEL1 0x60          //00100000
#define GDT_MARCADOR_ANEL2 0x60          //01000000
#define GDT_MARCADOR_ANEL3 0x60          //01100000
#define GDT_MARCADOR_NA_MEMORIA 0x80     //10000000

#define GDT_GRANULARIDADE_SISOP 0x10     //00010000
#define GDT_GRANULARIDADE_32BIT 0x40     //01000000
#define GDT_GRANULARIDADE_4K 0x80        //10000000


typedef struct GDT_Item_s
{
    uint16_t Limite;
    uint16_t BaseBaixa;
    uint8_t BaseMedia;
    uint8_t Marcadores;
    uint8_t Misto;
    uint8_t BaseAlta;
} GDT_Item_t;

typedef struct GDT_Ponteiro_s
{
    uint16_t Limite;
    uint32_t Base;
} GDT_Ponteiro_t;

static GDT_Item_t _GDT_Tabela[GDT_TABELA_QTD];
static GDT_Ponteiro_t _GDT_Ponteiro;

static void _GDT_Escreva(size_t i, size_t base, size_t limite, uint8_t marcadores, uint8_t granularidade)
{
    if(i > GDT_TABELA_QTD) return;
    
    _GDT_Tabela[i].BaseBaixa = base & 0xffff;
    _GDT_Tabela[i].BaseMedia = (base >> 16) & 0xff;
    _GDT_Tabela[i].BaseAlta = (base >> 24) & 0xff;
    _GDT_Tabela[i].Limite = limite & 0xffff;
    _GDT_Tabela[i].Marcadores = marcadores;
    _GDT_Tabela[i].Misto = ((limite >> 16) & 0xf) | (granularidade & 0xf0);
}

static void _GDT()
{
    _GDT_Escreva(0, 0,0,0,0);
    _GDT_Escreva(1, 0,0xffffffff, 
        GDT_MARCADOR_ANEL0 | GDT_MARCADOR_CODIGO | GDT_MARCADOR_GRAVAVEL | GDT_MARCADOR_CODIGO_OU_DADOS | GDT_MARCADOR_NA_MEMORIA,
        GDT_GRANULARIDADE_4K | GDT_GRANULARIDADE_32BIT
    );
    _GDT_Escreva(2, 0,0xffffffff, 
        GDT_MARCADOR_ANEL0 | GDT_MARCADOR_DADOS | GDT_MARCADOR_GRAVAVEL | GDT_MARCADOR_CODIGO_OU_DADOS | GDT_MARCADOR_NA_MEMORIA,
        GDT_GRANULARIDADE_4K | GDT_GRANULARIDADE_32BIT
    );
    _GDT_Escreva(3, 0,0xffffffff, 
        GDT_MARCADOR_ANEL3 | GDT_MARCADOR_CODIGO | GDT_MARCADOR_GRAVAVEL | GDT_MARCADOR_CODIGO_OU_DADOS | GDT_MARCADOR_NA_MEMORIA,
        GDT_GRANULARIDADE_4K | GDT_GRANULARIDADE_32BIT
    );
    _GDT_Escreva(4, 0,0xffffffff, 
        GDT_MARCADOR_ANEL3 | GDT_MARCADOR_DADOS | GDT_MARCADOR_GRAVAVEL | GDT_MARCADOR_CODIGO_OU_DADOS | GDT_MARCADOR_NA_MEMORIA,
        GDT_GRANULARIDADE_4K | GDT_GRANULARIDADE_32BIT
    );

    _GDT_Ponteiro.Limite = (sizeof (GDT_Item_t) * GDT_TABELA_QTD)-1;
    _GDT_Ponteiro.Base = (size_t)&_GDT_Tabela;

    asm("lgdt [__GDT_Ponteiro]");
}

// == IDT =======================================


#define IDT_TABELA_QTD 256

#define IDT_MARCADOR_BIT16 0x06     // 00000110
#define IDT_MARCADOR_BIT32 0x0E     // 00001110
#define IDT_MARCADOR_ANEL0 0x40     // 00000000
#define IDT_MARCADOR_ANEL1 0x40     // 01000000
#define IDT_MARCADOR_ANEL2 0x20     // 00100000
#define IDT_MARCADOR_ANEL3 0x60     // 01100000
#define IDT_MARCADOR_PRESENTE 0x80  // 10000000

typedef struct IDT_Item_s
{
    uint16_t BaseBaixa;
    uint16_t Seletor;
    uint8_t Reservado;
    uint8_t Marcadores;
    uint16_t BaseAlta;
} IDT_Item_t;

typedef struct IDT_Ponteiro_s
{
    uint16_t Limite;
    uint32_t Base;
} IDT_Ponteiro_t;

static IDT_Item_t _IDT_Tabela[IDT_TABELA_QTD];
static IDT_Ponteiro_t _IDT_Ponteiro;

static void _IDT_Escreva(size_t i, uint16_t marcadores, uint16_t seletor, void (*manipulador)())
{
    if(i > IDT_TABELA_QTD) return;
    
    size_t ptr = (size_t)&(*manipulador);

    _IDT_Tabela[i].BaseBaixa = ptr & 0xffff;
    _IDT_Tabela[i].BaseAlta = (ptr >> 16) & 0xffff;
    _IDT_Tabela[i].Reservado = 0;
    _IDT_Tabela[i].Marcadores = marcadores;
    _IDT_Tabela[i].Seletor = seletor;
}

static void _IDT_ManipuladorGenerico()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Requisicao nao implementada.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro0()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 0 - Divisao por zero.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro1()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 1 - Erro no Debug.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro2()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 2 - Interrupcao nao mascaravel.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro3()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 3 - Breakpoint - Debug.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro4()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 4 - Estouro detectado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro5()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 5 - Fora dos limites.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro6()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 6 - Opcode inválido.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro7()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 7 - Sem co processador.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro8()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 8 - Falha dupla.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro9()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 9 - Erro no Co processador.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro10()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 10 - TSS com erros.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro11()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 11 - Segmento nao encontrado.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro12()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 12 - Falha na pilha.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro13()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 13 - Erro Geral de Protecao.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro14()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 14 - Falha na paginacao.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro15()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 15 - Interrupcao desconhecida.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro16()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 16 - Falha de co processador.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro17()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 17 - Erro de alinhamento.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro18()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 18 - Verificacao de maquina.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro19()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 19 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro20()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 20 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro21()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 21 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("pop eax");
    asm("iretd");
}

static void _IDT_ManipuladorErro22()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 22 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro23()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 23 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro24()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 24 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro25()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 25 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro26()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 26 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro27()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 27 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro28()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 28 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro29()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 29 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro30()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 30 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT_ManipuladorErro31()
{
    asm("pushad");
    Debug_Escreva("Processador: [IDT] Erro 31 - Reservado.\n");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _IDT()
{
    for (size_t i = 0; i < IDT_TABELA_QTD; i++)
    {
        _IDT_Escreva(i, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorGenerico);
    }
    
    _IDT_Escreva(0, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro0);
    _IDT_Escreva(1, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro1);
    _IDT_Escreva(2, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro2);
    _IDT_Escreva(3, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro3);
    _IDT_Escreva(4, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro4);
    _IDT_Escreva(5, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro5);
    _IDT_Escreva(6, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro6);
    _IDT_Escreva(7, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro7);
    _IDT_Escreva(8, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro8);
    _IDT_Escreva(9, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro9);
    _IDT_Escreva(10, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro10);
    _IDT_Escreva(11, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro11);
    _IDT_Escreva(12, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro12);
    _IDT_Escreva(13, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro13);
    _IDT_Escreva(14, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro14);
    _IDT_Escreva(15, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro15);
    _IDT_Escreva(16, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro16);
    _IDT_Escreva(17, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro17);
    _IDT_Escreva(18, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro18);
    _IDT_Escreva(19, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro19);
    _IDT_Escreva(20, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro20);
    _IDT_Escreva(21, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro21);
    _IDT_Escreva(22, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro22);
    _IDT_Escreva(23, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro23);
    _IDT_Escreva(24, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro24);
    _IDT_Escreva(25, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro25);
    _IDT_Escreva(26, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro26);
    _IDT_Escreva(27, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro27);
    _IDT_Escreva(28, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro28);
    _IDT_Escreva(29, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro29);
    _IDT_Escreva(30, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro30);
    _IDT_Escreva(31, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_IDT_ManipuladorErro31);

    _IDT_Ponteiro.Limite = (sizeof (IDT_Item_t) * IDT_TABELA_QTD)-1;
    _IDT_Ponteiro.Base = (size_t)&_IDT_Tabela;

    asm("lidt [__IDT_Ponteiro]");

}
// == PIC =======================================

#define PIC_MANIPULADORES_QTD 16

static void (*_PIC_Manipuladores[PIC_MANIPULADORES_QTD])();

static void _PIC_Manipulador0()
{
    asm("pushad");
    if(_PIC_Manipuladores[0])_PIC_Manipuladores[0]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador1()
{
    asm("pushad");
    if(_PIC_Manipuladores[1])_PIC_Manipuladores[1]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador2()
{
    asm("pushad");
    if(_PIC_Manipuladores[2])_PIC_Manipuladores[2]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador3()
{
    asm("pushad");
    if(_PIC_Manipuladores[3])_PIC_Manipuladores[3]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador4()
{
    asm("pushad");
    if(_PIC_Manipuladores[4])_PIC_Manipuladores[4]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador5()
{
    asm("pushad");
    if(_PIC_Manipuladores[5])_PIC_Manipuladores[5]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador6()
{
    asm("pushad");
    if(_PIC_Manipuladores[6])_PIC_Manipuladores[6]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador7()
{
    asm("pushad");
    if(_PIC_Manipuladores[7])_PIC_Manipuladores[7]();
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador8()
{
    asm("pushad");
    if(_PIC_Manipuladores[8])_PIC_Manipuladores[8]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador9()
{
    asm("pushad");
    if(_PIC_Manipuladores[9])_PIC_Manipuladores[9]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador10()
{
    asm("pushad");
    if(_PIC_Manipuladores[10])_PIC_Manipuladores[10]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador11()
{
    asm("pushad");
    if(_PIC_Manipuladores[11])_PIC_Manipuladores[11]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador12()
{
    asm("pushad");
    if(_PIC_Manipuladores[12])_PIC_Manipuladores[12]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador13()
{
    asm("pushad");
    if(_PIC_Manipuladores[13])_PIC_Manipuladores[13]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador14()
{
    asm("pushad");
    if(_PIC_Manipuladores[14])_PIC_Manipuladores[14]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC_Manipulador15()
{
    asm("pushad");
    if(_PIC_Manipuladores[15])_PIC_Manipuladores[15]();
    asm("mov al, 0x20");
    asm("out 0xa0, al");
    asm("mov al, 0x20");
    asm("out 0x20, al");
    asm("popad");
    asm("leave");
    asm("iretd");
}

static void _PIC()
{
    for (size_t i = 0; i < PIC_MANIPULADORES_QTD; i++)
    {
        _PIC_Manipuladores[i] = 0;
    }
    // Registra manipuladores
    _IDT_Escreva(32, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador0);
    _IDT_Escreva(33, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador1);
    _IDT_Escreva(34, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador2);
    _IDT_Escreva(35, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador3);
    _IDT_Escreva(36, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador4);
    _IDT_Escreva(37, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador5);
    _IDT_Escreva(38, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador6);
    _IDT_Escreva(39, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador7);
    _IDT_Escreva(40, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador8);
    _IDT_Escreva(41, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador9);
    _IDT_Escreva(42, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador10);
    _IDT_Escreva(43, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador11);
    _IDT_Escreva(44, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador12);
    _IDT_Escreva(45, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador13);
    _IDT_Escreva(46, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador14);
    _IDT_Escreva(47, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, &_PIC_Manipulador15);
    
    
    //Ativia o modo de inicializacao
    ES_EscrevaUInt8(0x20, 0x11);
    ES_EscrevaUInt8(0xa0, 0x11);
    //Alinha IRQs nas interrupcoes
    ES_EscrevaUInt8(0x21, 0x20);
    ES_EscrevaUInt8(0xa1, 0x28);
    //Conecta os PICs
    ES_EscrevaUInt8(0x21, 0x4);
    ES_EscrevaUInt8(0xa1, 0x2);
    //Ativo modo 8086
    ES_EscrevaUInt8(0x21, 0x1);
    ES_EscrevaUInt8(0xa1, 0x1);
    //Zera os registradores
    ES_EscrevaUInt8(0x21, 0x0);
    ES_EscrevaUInt8(0xa1, 0x0);

}
// == PIT =======================================

static size_t _PIT_Contador = 0;

static void _PIT_Tick()
{
    _PIT_Contador++;
}

static void _PIT()
{
    size_t frequencia = 1193180 / 100;
    ES_EscrevaUInt8(0x43,0x36);
    ES_EscrevaUInt16(0x40,frequencia & 0xff);
    ES_EscrevaUInt16(0x40,(frequencia >> 8) & 0xff);
    _PIC_Manipuladores[0] = &_PIT_Tick;
}

// == Geral =====================================

void Processador()
{
    _GDT();
    _IDT();
    _PIC();
    _PIT();
    asm("sti");
}


void Processador_ChamaInterrupcao(size_t numero)
{
    Bloqueia(_Processador_Semaforo);
    asm("mov al, [bp+8]");
    asm("mov [__chama_int+1], al");
    asm("jmp __chama_int");
    asm("__chama_int:");
    asm("int 0");
    Libera(_Processador_Semaforo);
}

void Processador_RegistraInterrupcao(size_t numero, void (*acao)())
{
    if(numero > 31) return;
    _IDT_Escreva(numero, IDT_MARCADOR_PRESENTE | IDT_MARCADOR_BIT32, 0x8, acao);
}

void Processador_RegistraIRQ(size_t numero, void (*acao)())
{
    if(numero > 31) return;
    _PIC_Manipuladores[numero] = acao;
}

void Processador_DesabilitarInterrupcoes()
{
    asm("cli");
}

void Processador_HabilitarInterrupcoes()
{
    asm("sti");
}