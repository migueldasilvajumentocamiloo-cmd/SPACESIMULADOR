/**
 * main.c — Loop Principal Oficial PSL1GHT + Tiny3D
 */

#include "game.h"
#include <sysutil/sysutil.h>

ModoJogo g_modoJogo = MODO_VAB;
Foguete g_foguete;
EstadoVAB g_vab;
static int g_rodando = 1;

void InicializarVAB(void);
void AtualizarVAB(void);
void DesenharPeca(float cx, float cy, TipoPeca tipo, float s);

static void sysutil_callback(u64 status, u64 param, void *usrdata) {
    (void)param; (void)usrdata;
    if (status == SYSUTIL_EXIT_GAME) {
        g_rodando = 0;
    }
}

void InicializarFoguete(Foguete* f) {
    memset(f, 0, sizeof(Foguete));
}

void RecalcularEstatisticas(Foguete* f) {
    f->massaTotal = 0.0f;
    f->empuxoTotal = 0.0f;
    f->combustivelTotal = 0.0f;
    f->pecasCount = 0;

    for (int l = 0; l < GRID_ROWS; l++) {
        for (int c = 0; c < GRID_COLS; c++) {
            TipoPeca t = f->grid[l][c].tipo;
            if (t > PECA_VAZIO && t < PECA_MAX) {
                f->massaTotal += tabelaPecas[t].massa;
                f->empuxoTotal += tabelaPecas[t].empuxo;
                f->combustivelTotal += tabelaPecas[t].combustivel;
                f->pecasCount++;
            }
        }
    }
}

static float rocketX = 424.0f;
static float rocketY = 380.0f;
static float velY = 0.0f;
static float velX = 0.0f;
static float combustivelAtual = 0.0f;
static int vooIniciado = 0;

static void DrawRectVoo(float x, float y, float w, float h, u32 cor) {
    tiny3d_SetPolygon(TINY3D_QUADS);
    tiny3d_VertexPos(x, y, 0);         tiny3d_VertexColor(cor);
    tiny3d_VertexPos(x + w, y, 0);     tiny3d_VertexColor(cor);
    tiny3d_VertexPos(x + w, y + h, 0); tiny3d_VertexColor(cor);
    tiny3d_VertexPos(x, y + h, 0);     tiny3d_VertexColor(cor);
    tiny3d_End();
}

static void AtualizarVoo(void) {
    if (!vooIniciado) {
        rocketX = 424.0f;
        rocketY = 360.0f;
        velX = 0.0f;
        velY = 0.0f;
        combustivelAtual = g_foguete.combustivelTotal;
        vooIniciado = 1;
    }

    padInfo padinfo;
    padData paddata;
    ioPadGetInfo(&padinfo);

    int motorLigado = 0;
    if (padinfo.status) {
        ioPadGetData(0, &paddata);

        if (paddata.BTN_CROSS && combustivelAtual > 0.0f && g_foguete.empuxoTotal > 0.0f) {
            motorLigado = 1;
            float aceleracao = (g_foguete.empuxoTotal * 1000.0f) / (g_foguete.massaTotal + 1.0f);
            velY -= aceleracao * 0.008f;
            combustivelAtual -= 0.25f;
            if (combustivelAtual < 0.0f) combustivelAtual = 0.0f;
        }

        if (paddata.BTN_LEFT)  velX -= 0.08f;
        if (paddata.BTN_RIGHT) velX += 0.08f;

        if (paddata.BTN_SELECT) {
            vooIniciado = 0;
            g_modoJogo = MODO_VAB;
            return;
        }
    }

    velY += 0.04f;
    rocketX += velX;
    rocketY += velY;

    if (rocketY > 360.0f) {
        rocketY = 360.0f;
        velY = 0.0f;
        velX *= 0.85f;
    }

    tiny3d_Clear(0x0A1535FF, TINY3D_CLEAR_ALL);
    tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);
    tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
        TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
        TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
    tiny3d_Project2D();

    DrawRectVoo(0, 440, 848, 72, 0x228833FF);
    DrawRectVoo(350, 432, 148, 8, 0x777777FF);

    float escala = 0.7f;
    float passo = CELL_SIZE * escala;
    for (int l = 0; l < GRID_ROWS; l++) {
        for (int c = 0; c < GRID_COLS; c++) {
            TipoPeca t = g_foguete.grid[l][c].tipo;
            if (t != PECA_VAZIO) {
                float px = rocketX + (c - GRID_COLS / 2.0f) * passo;
                float py = rocketY + (l - GRID_ROWS / 2.0f) * passo;
                DesenharPeca(px, py, t, escala);

                if (motorLigado && (t == PECA_MOTOR_LEVE || t == PECA_MOTOR_PESADO)) {
                    DrawRectVoo(px - 4, py + 10, 8, 18, 0xFF8800FF);
                }
            }
        }
    }

    tiny3d_Flip();
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    tiny3d_Init(1024 * 1024);
    ioPadInit(7);
    sysUtilRegisterCallback(0, sysutil_callback, NULL);

    InicializarVAB();

    while (g_rodando) {
        sysUtilCheckCallback();
        if (g_modoJogo == MODO_VAB) {
            AtualizarVAB();
        } else {
            AtualizarVoo();
        }
    }

    ioPadEnd();
    sysUtilUnregisterCallback(0);
    return 0;
}
