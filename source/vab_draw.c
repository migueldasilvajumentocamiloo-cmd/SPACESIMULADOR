/**
 * vab_draw.c — Desenhos do Modo de Construção (VAB)
 */

#include "game.h"

extern EstadoVAB g_vab;
extern Foguete g_foguete;
extern const DadosPeca tabelaPecas[PECA_MAX];

#define COR_GRID_LINHA  0x334466FF
#define COR_GRID_FUNDO  0x121833FF
#define COR_CURSOR      0xFFFF00FF
#define COR_SELECIONADO 0x22AA44FF
#define COR_TEXTO       0xFFFFFFFF
#define COR_BARRA_FUNDO 0x333344FF
#define COR_BARRA_VALOR 0x00CC44FF
#define COR_BARRA_ALTA  0xFF5500FF

static u32 g_corAtual = 0xFFFFFFFF;

static void SetCor(u32 cor) { g_corAtual = cor; }

static void DrawRect(float x, float y, float w, float h) {
    tiny3d_SetPolygon(TINY3D_QUADS);
    tiny3d_VertexPos(x, y, 0);         tiny3d_VertexColor(g_corAtual);
    tiny3d_VertexPos(x + w, y, 0);     tiny3d_VertexColor(g_corAtual);
    tiny3d_VertexPos(x + w, y + h, 0); tiny3d_VertexColor(g_corAtual);
    tiny3d_VertexPos(x, y + h, 0);     tiny3d_VertexColor(g_corAtual);
    tiny3d_End();
}

static void DrawTri(float x1, float y1, float x2, float y2, float x3, float y3) {
    tiny3d_SetPolygon(TINY3D_TRIANGLES);
    tiny3d_VertexPos(x1, y1, 0); tiny3d_VertexColor(g_corAtual);
    tiny3d_VertexPos(x2, y2, 0); tiny3d_VertexColor(g_corAtual);
    tiny3d_VertexPos(x3, y3, 0); tiny3d_VertexColor(g_corAtual);
    tiny3d_End();
}

static void DrawLine(float x1, float y1, float x2, float y2) {
    tiny3d_SetPolygon(TINY3D_LINES);
    tiny3d_VertexPos(x1, y1, 0); tiny3d_VertexColor(g_corAtual);
    tiny3d_VertexPos(x2, y2, 0); tiny3d_VertexColor(g_corAtual);
    tiny3d_End();
}

void DesenharGrade(void) {
    SetCor(COR_GRID_FUNDO);
    DrawRect(GRID_OFFSET_X, GRID_OFFSET_Y, GRID_COLS * CELL_SIZE, GRID_ROWS * CELL_SIZE);
    SetCor(COR_GRID_LINHA);
    for (int c = 0; c <= GRID_COLS; c++) {
        float x = GRID_OFFSET_X + c * CELL_SIZE;
        DrawLine(x, GRID_OFFSET_Y, x, GRID_OFFSET_Y + GRID_ROWS * CELL_SIZE);
    }
    for (int l = 0; l <= GRID_ROWS; l++) {
        float y = GRID_OFFSET_Y + l * CELL_SIZE;
        DrawLine(GRID_OFFSET_X, y, GRID_OFFSET_X + GRID_COLS * CELL_SIZE, y);
    }
}

void DesenharPeca(float cx, float cy, TipoPeca tipo, float s) {
    if (tipo <= PECA_VAZIO || tipo >= PECA_MAX) return;
    SetCor(tabelaPecas[tipo].cor);

    switch (tipo) {
        case PECA_CAPSULA:
            DrawRect(cx - 10*s, cy - 2*s, 20*s, 14*s);
            DrawTri(cx - 10*s, cy - 2*s, cx + 10*s, cy - 2*s, cx, cy - 14*s);
            SetCor(0x44AAFFFF);
            DrawRect(cx - 4*s, cy + 1*s, 8*s, 5*s);
            break;
        case PECA_TANQUE_PEQUENO:
            DrawRect(cx - 10*s, cy - 14*s, 20*s, 28*s);
            SetCor(0x666666FF);
            DrawRect(cx - 10*s, cy - 8*s, 20*s, 2*s);
            DrawRect(cx - 10*s, cy + 6*s, 20*s, 2*s);
            break;
        case PECA_TANQUE_GRANDE:
            DrawRect(cx - 12*s, cy - 14*s, 24*s, 28*s);
            SetCor(0x444444FF);
            DrawRect(cx - 12*s, cy - 9*s, 24*s, 2*s);
            DrawRect(cx - 12*s, cy - 2*s, 24*s, 2*s);
            DrawRect(cx - 12*s, cy + 5*s, 24*s, 2*s);
            break;
        case PECA_MOTOR_LEVE:
            DrawRect(cx - 8*s, cy - 10*s, 16*s, 10*s);
            DrawTri(cx, cy - 2*s, cx - 7*s, cy + 12*s, cx + 7*s, cy + 12*s);
            break;
        case PECA_MOTOR_PESADO:
            DrawRect(cx - 11*s, cy - 10*s, 22*s, 10*s);
            DrawTri(cx, cy - 2*s, cx - 10*s, cy + 13*s, cx + 10*s, cy + 13*s);
            break;
        case PECA_SEPARADOR:
            DrawRect(cx - 12*s, cy - 4*s, 24*s, 8*s);
            SetCor(0xFF4400FF);
            DrawRect(cx - 12*s, cy - 1*s, 24*s, 2*s);
            break;
        case PECA_PARAQUEDAS:
            DrawTri(cx - 11*s, cy + 2*s, cx + 11*s, cy + 2*s, cx, cy - 11*s);
            SetCor(0xCCCCCCFF);
            DrawRect(cx - 6*s, cy + 2*s, 12*s, 8*s);
            break;
        case PECA_PERNA_POUSO:
            DrawRect(cx - 3*s, cy - 12*s, 6*s, 22*s);
            DrawRect(cx - 9*s, cy + 8*s, 18*s, 4*s);
            break;
        default: break;
    }
}

void DesenharMenuLateral(void) {
    SetCor(0x161D3AFF);
    DrawRect(6, 10, 84, SCREEN_H - 20);
    for (int p = PECA_CAPSULA; p < PECA_MAX; p++) {
        float y = 20.0f + (p - 1) * 54.0f;
        SetCor(p == g_vab.pecaSelecionada ? COR_SELECIONADO : 0x222C55FF);
        DrawRect(12.0f, y - 2, 72, 48);
        DesenharPeca(48.0f, y + 22, (TipoPeca)p, 0.85f);
    }
}

void DesenharCursor(void) {
    float x = GRID_OFFSET_X + g_vab.cursorX * CELL_SIZE;
    float y = GRID_OFFSET_Y + g_vab.cursorY * CELL_SIZE;
    SetCor(COR_CURSOR);
    DrawRect(x, y, CELL_SIZE, 2);
    DrawRect(x, y + CELL_SIZE - 2, CELL_SIZE, 2);
    DrawRect(x, y, 2, CELL_SIZE);
    DrawRect(x + CELL_SIZE - 2, y, 2, CELL_SIZE);
}

static unsigned char PegarSegmentos(int d) {
    switch (d) {
        case 0: return 0x3F;
        case 1: return 0x06;
        case 2: return 0x5B;
        case 3: return 0x4F;
        case 4: return 0x66;
        case 5: return 0x6D;
        case 6: return 0x7D;
        case 7: return 0x07;
        case 8: return 0x7F;
        case 9: return 0x6F;
        default: return 0x00;
    }
}

static void DesenharDigito(float x, float y, int d, float t) {
    unsigned char s = PegarSegmentos(d);
    SetCor(COR_TEXTO);
    if (s & 1)  DrawRect(x+2, y, t-4, 2);
    if (s & 2)  DrawRect(x+t-2, y+2, 2, t*0.45f);
    if (s & 4)  DrawRect(x+t-2, y+t*0.5f, 2, t*0.45f);
    if (s & 8)  DrawRect(x+2, y+t-2, t-4, 2);
    if (s & 16) DrawRect(x, y+t*0.5f, 2, t*0.45f);
    if (s & 32) DrawRect(x, y+2, 2, t*0.45f);
    if (s & 64) DrawRect(x+2, y+t*0.45f, t-4, 2);
}

void DesenharNumero(float x, float y, float valor, int casas) {
    int v = (int)(valor + 0.5f);
    if (v < 0) v = 0;
    for (int i = casas - 1; i >= 0; i--) {
        DesenharDigito(x + i * 14.0f, y, v % 10, 11.0f);
        v /= 10;
    }
}

void DesenharEstatisticas(void) {
    RecalcularEstatisticas(&g_foguete);
    float rx = SCREEN_W - 140.0f;
    SetCor(0x161D3AFF);
    DrawRect(rx - 10, 10, 140, SCREEN_H - 20);

    SetCor(COR_BARRA_FUNDO); DrawRect(rx, 38, 120, 16);
    float m = g_foguete.massaTotal / 3000.0f; if (m > 1.0f) m = 1.0f;
    SetCor(COR_BARRA_VALOR); DrawRect(rx + 1, 39, 118 * m, 14);
    DesenharNumero(rx, 20, g_foguete.massaTotal, 4);

    float twr = g_foguete.massaTotal > 0.1f ? (g_foguete.empuxoTotal * 1000.0f) / (g_foguete.massaTotal * 9.81f) : 0.0f;
    SetCor(COR_BARRA_FUNDO); DrawRect(rx, 90, 120, 16);
    float t = twr / 2.5f; if (t > 1.0f) t = 1.0f;
    SetCor(twr >= 1.0f ? COR_BARRA_VALOR : COR_BARRA_ALTA);
    DrawRect(rx + 1, 91, 118 * t, 14);
    DesenharNumero(rx, 72, twr * 10.0f, 2);

    SetCor(COR_BARRA_FUNDO); DrawRect(rx, 142, 120, 16);
    float c = g_foguete.combustivelTotal / 600.0f; if (c > 1.0f) c = 1.0f;
    SetCor(0x2288FFFF); DrawRect(rx + 1, 143, 118 * c, 14);
    DesenharNumero(rx, 124, g_foguete.combustivelTotal, 4);
}
