/**
 * main.c — Loop Principal do SpaceSim PS3
 */

#include "game.h"
#include <sysutil/sysutil.h>

// Variáveis globais
ModoJogo g_modoJogo = MODO_VAB;
Foguete g_foguete;
EstadoVAB g_vab;

// Funções externas do VAB
void InicializarVAB(void);
void AtualizarVAB(void);
void DesenharPeca(float cx, float cy, TipoPeca tipo, float s);

// Funções de dados do foguete
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

// ==================== MODO DE VOO INICIAL ====================
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

        // Botão X liga os motores se tiver combustível
        if (paddata.BTN_CROSS && combustivelAtual > 0.0f && g_foguete.empuxoTotal > 0.0f) {
            motorLigado = 1;
            float aceleracao = (g_foguete.empuxoTotal * 1000.0f) / (g_foguete.massaTotal + 1.0f);
            velY -= aceleracao * 0.008f;
            combustivelAtual -= 0.25f;
            if (combustivelAtual < 0.0f) combustivelAtual = 0.0f;
        }

        // Controle lateral
        if (paddata.BTN_LEFT)  velX -= 0.08f;
        if (paddata.BTN_RIGHT) velX += 0.08f;

        // SELECT volta para a oficina VAB
        if (paddata.BTN_SELECT) {
            vooIniciado = 0;
            g_modoJogo = MODO_VAB;
            return;
        }
    }

    // Gravidade da Terra
    velY += 0.04f;
    rocketX += velX;
    rocketY += velY;

    // Chão (plataforma)
    if (rocketY > 360.0f) {
        rocketY = 360.0f;
        velY = 0.0f;
        velX *= 0.85f;
    }

    // Desenho do cenário de voo
    tiny3d_Clear(0x0A1535FF, TINY3D_CLEAR_ALL);
    tiny3d_Project2D();

    // Chão e plataforma
    DrawRectVoo(0, 440, 848, 72, 0x228833FF);
    DrawRectVoo(350, 432, 148, 8, 0x777777FF);

    // Desenha exatamente o foguete montado no VAB
    float escala = 0.7f;
    float passo = CELL_SIZE * escala;
    for (int l = 0; l < GRID_ROWS; l++) {
        for (int c = 0; c < GRID_COLS; c++) {
            TipoPeca t = g_foguete.grid[l][c].tipo;
            if (t != PECA_VAZIO) {
                float px = rocketX + (c - GRID_COLS / 2.0f) * passo;
                float py = rocketY + (l - GRID_ROWS / 2.0f) * passo;
                DesenharPeca(px, py, t, escala);

                // Chama nos motores ligados
                if (motorLigado && (t == PECA_MOTOR_LEVE || t == PECA_MOTOR_PESADO)) {
                    DrawRectVoo(px - 4, py + 10, 8, 18, 0xFF8800FF);
                }
            }
        }
    }

    tiny3d_Flip();
}

// ==================== INÍCIO DO JOGO ====================
int main(int argc, char **argv) {
    (void)argc; (void)argv;

    tiny3d_Init(1024 * 1024);
    ioPadInit(7);
    InicializarVAB();

    while (1) {
        sysUtilCheckCallback();
        if (g_modoJogo == MODO_VAB) {
            AtualizarVAB();
        } else {
            AtualizarVoo();
        }
    }

    return 0;
}
