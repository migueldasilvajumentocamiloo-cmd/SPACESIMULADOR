/**
 * vab.c — Lógica e Controles do Modo de Construção (VAB)
 */

#include "game.h"

void DesenharGrade(void);
void DesenharPeca(float cx, float cy, TipoPeca tipo, float s);
void DesenharMenuLateral(void);
void DesenharCursor(void);
void DesenharEstatisticas(void);

void InicializarVAB(void) {
    memset(&g_vab, 0, sizeof(EstadoVAB));
    g_vab.cursorX = GRID_COLS / 2;
    g_vab.cursorY = GRID_ROWS / 2;
    g_vab.pecaSelecionada = PECA_CAPSULA;
    g_vab.menuAberto = 1;
    InicializarFoguete(&g_foguete);
}

static void ProcessarEntradaVAB(void) {
    padInfo padinfo;
    padData paddata;
    static int pUp=0, pDown=0, pLeft=0, pRight=0, pL1=0, pR1=0, pX=0, pSq=0, pCi=0, pSt=0;

    ioPadGetInfo(&padinfo);
    if (!padinfo.status) return;
    ioPadGetData(0, &paddata);

    if (paddata.BTN_UP && !pUp && g_vab.cursorY > 0) g_vab.cursorY--;
    if (paddata.BTN_DOWN && !pDown && g_vab.cursorY < GRID_ROWS - 1) g_vab.cursorY++;
    if (paddata.BTN_LEFT && !pLeft && g_vab.cursorX > 0) g_vab.cursorX--;
    if (paddata.BTN_RIGHT && !pRight && g_vab.cursorX < GRID_COLS - 1) g_vab.cursorX++;

    if (paddata.BTN_L1 && !pL1) {
        g_vab.pecaSelecionada--;
        if (g_vab.pecaSelecionada < PECA_CAPSULA) g_vab.pecaSelecionada = PECA_MAX - 1;
    }
    if (paddata.BTN_R1 && !pR1) {
        g_vab.pecaSelecionada++;
        if (g_vab.pecaSelecionada >= PECA_MAX) g_vab.pecaSelecionada = PECA_CAPSULA;
    }

    if (paddata.BTN_CROSS && !pX)
        g_foguete.grid[g_vab.cursorY][g_vab.cursorX].tipo = (TipoPeca)g_vab.pecaSelecionada;
    if (paddata.BTN_SQUARE && !pSq)
        g_foguete.grid[g_vab.cursorY][g_vab.cursorX].tipo = PECA_VAZIO;
    if (paddata.BTN_CIRCLE && !pCi)
        InicializarFoguete(&g_foguete);

    if (paddata.BTN_START && !pSt) {
        RecalcularEstatisticas(&g_foguete);
        if (g_foguete.pecasCount > 0) g_modoJogo = MODO_VOO;
    }

    pUp=paddata.BTN_UP; pDown=paddata.BTN_DOWN; pLeft=paddata.BTN_LEFT; pRight=paddata.BTN_RIGHT;
    pL1=paddata.BTN_L1; pR1=paddata.BTN_R1; pX=paddata.BTN_CROSS; pSq=paddata.BTN_SQUARE;
    pCi=paddata.BTN_CIRCLE; pSt=paddata.BTN_START;
}

void AtualizarVAB(void) {
    ProcessarEntradaVAB();
    tiny3d_Clear(0x080C22FF, TINY3D_CLEAR_ALL);
    tiny3d_Project2D();

    DesenharGrade();
    DesenharMenuLateral();
    DesenharEstatisticas();

    for (int l = 0; l < GRID_ROWS; l++) {
        for (int c = 0; c < GRID_COLS; c++) {
            TipoPeca t = g_foguete.grid[l][c].tipo;
            if (t != PECA_VAZIO) {
                float cx = GRID_OFFSET_X + c * CELL_SIZE + CELL_SIZE / 2.0f;
                float cy = GRID_OFFSET_Y + l * CELL_SIZE + CELL_SIZE / 2.0f;
                DesenharPeca(cx, cy, t, 0.85f);
            }
        }
    }

    DesenharCursor();
    tiny3d_Flip();
}
