#ifndef BRASIS_H
#define BRASIS_H

#define VERSAO 0
#define SUBVERSAO 1
#define REVISAO 0

#define VIDEO_TEXTO 0
#define VIDEO_EGA 1
#define VIDEO_VGA 2
#define VIDEO_SVGA 3
#define VIDEO_VBE2 4

#pragma pack(1)

typedef struct BrasisParams_s {
    uint8_t Assinatura[6];
    uint8_t Versao;
    uint8_t Revisao;
    uint16_t VideoModo;
    uint16_t VideoHoriz;
    uint16_t VideoVert;
    uint16_t VideoCores;
    uint32_t NucleoInicio;
    uint32_t NucleoTam;
    uint32_t ImagemInicio;
    uint32_t ImagemTam;
    uint32_t VideoInicio;
    uint32_t VideoTam;
    uint32_t MemoriaBaixa; // Topo do bloco 0 - 1 MiB
    uint32_t MemoriaMedia; // Topo do bloco 1 - 16 MiB
    uint32_t MemoriaAlta;  // Topo do bloco 16 MiG - 4 GiB
    uint8_t NomeArqCfg[32];
    uint8_t NomeArqNucleo[32];
    uint8_t NomeArqImagem[32];
} BrasisParams_t;

#endif