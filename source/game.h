/**
 * game.h — Definições Globais e Estruturas de Dados
 */

#ifndef GAME_H
#define GAME_H

#include <tiny3d.h>
#include <io/pad.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ==================== CONFIGURAÇÕES GERAIS ====================
#define SCREEN_W 848
#define SCREEN_H 512

// Grid do VAB
#define GRID_COLS 12
#define GRID_ROWS 16
#define CELL_SIZE 28

// Posição inicial do grid na tela
#define GRID_OFFSET_X ((SCREEN_W - GRID_COLS * CELL_SIZE) / 2)
#define GRID_OFFSET_Y ((SCREEN_H - GRID_ROWS * CELL_SIZE) / 2)

// ==================== TIPOS DE PEÇAS ====================
typedef enum {
    PECA_VAZIO = 0,
    PECA_CAPSULA,
    PECA_TANQUE_PEQUENO,
    PECA_TANQUE_GRANDE,
    PECA_MOTOR_LEVE,
    PECA_MOTOR_PESADO,
    PECA_SEPARADOR,
    PECA_PARAQUEDAS,
    PECA_PERNA_POUSO,
    PECA_MAX
} TipoPeca;

typedef struct {
    const char* nome;
    float massa;           // kg
    float combustivel;     // litros
    float empuxo;          // kN no vácuo
    u32 cor;
} DadosPeca;

// Estados do jogo
typedef enum {
    MODO_VAB = 0,
    MODO_VOO
} ModoJogo;

// Célula do grid
typedef struct {
    TipoPeca tipo;
} CelulaGrid;

// Estrutura do Foguete
typedef struct {
    CelulaGrid grid[GRID_ROWS][GRID_COLS];
    float massaTotal;
    float empuxoTotal;
    float combustivelTotal;
    int pecasCount;
} Foguete;

// Estrutura do VAB
typedef struct {
    int cursorX;
    int cursorY;
    int pecaSelecionada;
    int menuAberto;
} EstadoVAB;

// ==================== DADOS DAS PEÇAS (Formato RGBA Tiny3D) ====================
static const DadosPeca tabelaPecas[PECA_MAX] = {
    [PECA_VAZIO] =           {"",                 0.0f,   0.0f,   0.0f, 0x00000000},
    [PECA_CAPSULA] =         {"Capsula",        120.0f,   0.0f,   0.0f, 0xFFFFFFFF},
    [PECA_TANQUE_PEQUENO] =  {"Tanque P",        60.0f,  50.0f,   0.0f, 0xCCCCCCFF},
    [PECA_TANQUE_GRANDE] =   {"Tanque G",       100.0f, 120.0f,   0.0f, 0x999999FF},
    [PECA_MOTOR_LEVE] =      {"Motor Leve",      80.0f,   0.0f,  85.0f, 0x555555FF},
    [PECA_MOTOR_PESADO] =    {"Motor Pesado",   180.0f,   0.0f, 350.0f, 0x333333FF},
    [PECA_SEPARADOR] =       {"Separador",       25.0f,   0.0f,   0.0f, 0xAAAAAAFF},
    [PECA_PARAQUEDAS] =      {"Paraquedas",      40.0f,   0.0f,   0.0f, 0xFF8800FF},
    [PECA_PERNA_POUSO] =     {"Perna de Pouso",  30.0f,   0.0f,   0.0f, 0x4488FFFF}
};

// ==================== VARIÁVEIS GLOBAIS ====================
extern ModoJogo g_modoJogo;
extern Foguete g_foguete;
extern EstadoVAB g_vab;

// ==================== FUNÇÕES ====================
void InicializarFoguete(Foguete* f);
void RecalcularEstatisticas(Foguete* f);

#endif // GAME_H
