#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ES.h"

// Rotina genérica de Escrita de Caractere
static void (* _Debug_EscrevaC) (uint8_t c) = 0;

// Variáveis Globais do Sistema VideoTexto
static uint16_t * _VT_Ptr = (uint16_t *)0xb8000;
static size_t _VT_X = 0;
static size_t _VT_Y = 0;
static bool _VT_Escape = false;

// Calculo da posição na RAM do video texto
static void _VT_Calc()
{
    if(_VT_X >= 80)
    {
        _VT_X = 0;
        _VT_Y++;
    }
    while(_VT_Y >= 25)
    {
        for (size_t i = 0; i < 80*24; i++)
        {
            _VT_Ptr[i] = _VT_Ptr[i + 80];
        }
        for (size_t i = 0; i < 80; i++)
        {
            _VT_Ptr[80*24 + i] = 0x720;
        }
        _VT_Y--;
    }
}

// Atualiza cursor na tela de video texto
static void _VT_AtualizaCursor(uint16_t endereco)
{
    ES_EscrevaUInt8(0x03D4, 0xf);
    ES_EscrevaUInt8(0x03D5, endereco & 0xff);
    ES_EscrevaUInt8(0x03D4, 0xe);
    ES_EscrevaUInt8(0x03D5, endereco >> 8);
}

// Escreve um caractere em video texto
static void _VT_EscrevaC(uint8_t c)
{
    uint16_t endereco;
    if(_VT_Escape)
    {
        _VT_Escape = false;
        switch (c)
        {
        case 'n':
            _VT_EscrevaC('\n');
            break;
        case 'r':
            _VT_EscrevaC('\r');
            break;
        case 't':
            _VT_EscrevaC('\t');
            break;
        default:
            goto _vt_escreva_c;
            break;
        }
    }
    else
    {
        if(c == '\\')
        {
            _VT_Escape = true;
            return;
        }
        if(c == 13)
        {
            _VT_X = 0;
            c = 0;
        }
        if(c == 10)
        {
            _VT_X = 0;
            _VT_Y++;
            c = 0;
        }
        _vt_escreva_c:
        _VT_Calc();
        endereco = _VT_Y * 80 + _VT_X;
        if(c > 0)
        {
            _VT_Ptr[endereco] = 0x700 | c;
            _VT_X++;
            _VT_Calc();
        }
        _VT_AtualizaCursor(endereco);
    }
}

// Configura o Debug para VideoTexto
void Debug_ConfigVideoTexto()
{
    _Debug_EscrevaC = &_VT_EscrevaC;
    for (size_t i = 0; i < 80*25; i++)
    {
        _VT_Ptr[i] = 0x720;
    }
    _VT_AtualizaCursor(0);
}

// Escreve um texto no console de depuração
void Debug_EscrevaTexto(uint8_t * txt)
{
    while(*txt != 0)
    {
        _Debug_EscrevaC(*txt++);
    }
}

// Escreve um número inteiro sem sinal
void Debug_EscrevaUInt(uint32_t valor)
{
    uint32_t prox = valor / 10;
    if(prox > 0) Debug_EscrevaUInt(prox);
    _Debug_EscrevaC(valor % 10 + '0');
}

// Escreve um número inteiro com sinal
void Debug_EscrevaInt(int32_t valor)
{
    if(valor < 0) 
    {
        _Debug_EscrevaC('-');
        valor = -valor;
    }
    Debug_EscrevaUInt((uint32_t)valor);
}

// Escreve um texto formatado
void Debug_Escreva(uint8_t * fmt, ...)
{
    size_t ptr = &fmt;
    size_t * arg = (size_t *)ptr;
    arg++;
    while(*fmt != 0)
    {
        if((*fmt == '%') && (_VT_Escape == false))
        {
            fmt++;
            if(*fmt == 0) return;
            switch (*fmt)
            {
            case 'c':
                _Debug_EscrevaC(*arg);
                break;
            case 'd':
                Debug_EscrevaInt(*arg);
                break;
            case 'u':
                Debug_EscrevaUInt(*arg);
                break;
            default:
                _Debug_EscrevaC(*fmt);
                break;
            }
            arg++;
            fmt++;
        }
        else
        {
            _Debug_EscrevaC(*fmt++);
        }
    }
}