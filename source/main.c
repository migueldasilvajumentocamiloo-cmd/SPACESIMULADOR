/**
 * SpaceSim PS3 — Versão de Teste Funcional
 * PSL1GHT + Tiny3D + DualShock 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sysutil/sysutil.h>
#include <io/pad.h>
#include <tiny3d.h>

// Cores no formato RGBA (Tiny3D)
#define COLOR_WHITE   0xFFFFFFFF
#define COLOR_GRAY    0x888888FF
#define COLOR_DARK    0x444444FF
#define COLOR_ORANGE  0xFF8800FF
#define COLOR_YELLOW  0xFFFF00FF
#define COLOR_BLUE    0x2288FFFF
#define COLOR_GREEN   0x22AA44FF

typedef struct {
    float x, y;
    float scale;
    int engineOn;
    float flameHeight;
} Rocket;

Rocket rocket = {
    .x = 424.0f, // Centro horizontal na escala virtual 848x512 do Tiny3D
    .y = 320.0f,
    .scale = 1.0f,
    .engineOn = 0,
    .flameHeight = 0.0f
};

// ==================== PRIMITIVAS TINY3D ====================

void drawRect(float x, float y, float w, float h, u32 color) {
    tiny3d_SetPolygon(TINY3D_QUADS);
    tiny3d_VertexPos(x, y, 0);
    tiny3d_VertexColor(color);
    tiny3d_VertexPos(x + w, y, 0);
    tiny3d_VertexColor(color);
    tiny3d_VertexPos(x + w, y + h, 0);
    tiny3d_VertexColor(color);
    tiny3d_VertexPos(x, y + h, 0);
    tiny3d_VertexColor(color);
    tiny3d_End();
}

void drawTri(float x1, float y1, float x2, float y2, float x3, float y3, u32 color) {
    tiny3d_SetPolygon(TINY3D_TRIANGLES);
    tiny3d_VertexPos(x1, y1, 0);
    tiny3d_VertexColor(color);
    tiny3d_VertexPos(x2, y2, 0);
    tiny3d_VertexColor(color);
    tiny3d_VertexPos(x3, y3, 0);
    tiny3d_VertexColor(color);
    tiny3d_End();
}

// ==================== PEÇAS DO FOGUETE ====================

void drawCapsule(float cx, float cy, float s) {
    float w = 24.0f * s;
    float h = 28.0f * s;
    drawTri(cx, cy - h * 0.7f, cx - w * 0.5f, cy, cx + w * 0.5f, cy, COLOR_WHITE);
    drawRect(cx - w * 0.5f, cy, w, h * 0.3f, COLOR_GRAY);
}

void drawTank(float cx, float cy, float s, float height) {
    float w = 24.0f * s;
    float h = height * s;
    drawRect(cx - w * 0.5f, cy, w, h, COLOR_WHITE);
    drawRect(cx - w * 0.5f, cy, w, 2.0f, COLOR_DARK);
    drawRect(cx - w * 0.5f, cy + h - 2.0f, w, 2.0f, COLOR_DARK);
}

void drawEngine(float cx, float cy, float s) {
    float w = 18.0f * s;
    float h = 14.0f * s;
    drawRect(cx - w * 0.4f, cy, w * 0.8f, h * 0.4f, COLOR_DARK);
    drawTri(cx, cy + h * 0.2f, cx - w * 0.6f, cy + h, cx + w * 0.6f, cy + h, COLOR_GRAY);
}

void drawFlame(float cx, float cy, float s, float height) {
    if (height <= 1.0f) return;
    float w = 14.0f * s;
    float h = height * s;
    drawTri(cx - w * 0.6f, cy, cx + w * 0.6f, cy, cx, cy + h, COLOR_ORANGE);
    drawTri(cx - w * 0.3f, cy, cx + w * 0.3f, cy, cx, cy + h * 0.6f, COLOR_YELLOW);
}

void drawRocket(Rocket *r) {
    float cx = r->x;
    float cy = r->y;
    float s = r->scale;

    drawCapsule(cx, cy - 85.0f * s, s);
    drawTank(cx, cy - 76.0f * s, s, 40.0f);
    drawTank(cx, cy - 36.0f * s, s, 45.0f);
    drawEngine(cx, cy + 9.0f * s, s);

    if (r->engineOn) {
        drawFlame(cx, cy + 23.0f * s, s, r->flameHeight);
    }
}

// ==================== CONTROLE DUALSHOCK 3 ====================

void readPadInput() {
    padInfo padinfo;
    padData paddata;
    static int prevCross = 0;

    ioPadGetInfo(&padinfo);
    if (padinfo.status[0]) {
        ioPadGetData(0, &paddata);

        // Botão X liga/desliga motor
        if (paddata.BTN_CROSS && !prevCross) {
            rocket.engineOn = !rocket.engineOn;
            if (rocket.engineOn && rocket.flameHeight < 20.0f) {
                rocket.flameHeight = 35.0f;
            }
        }
        prevCross = paddata.BTN_CROSS;

        // L2 e R2 ajustam potência
        if (paddata.BTN_R2) {
            rocket.flameHeight += 1.5f;
            if (rocket.flameHeight > 70.0f) rocket.flameHeight = 70.0f;
        }
        if (paddata.BTN_L2) {
            rocket.flameHeight -= 1.5f;
            if (rocket.flameHeight < 10.0f) rocket.flameHeight = 10.0f;
        }

        // Analógico esquerdo ou direcional move o foguete
        float ax = ((float)paddata.ANA_L_H - 128.0f) / 128.0f;
        float ay = ((float)paddata.ANA_L_V - 128.0f) / 128.0f;

        if (fabsf(ax) > 0.15f) rocket.x += ax * 3.0f;
        if (fabsf(ay) > 0.15f) rocket.y += ay * 3.0f;

        if (paddata.BTN_LEFT)  rocket.x -= 2.5f;
        if (paddata.BTN_RIGHT) rocket.x += 2.5f;
        if (paddata.BTN_UP)    rocket.y -= 2.5f;
        if (paddata.BTN_DOWN)  rocket.y += 2.5f;
    }
}

// ==================== LOOP PRINCIPAL ====================

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    tiny3d_Init(1024 * 1024);
    ioPadInit(7);

    while (1) {
        sysUtilCheckCallback();
        readPadInput();

        // Limpa a tela com fundo azul escuro espacial
        tiny3d_Clear(0x050818FF, TINY3D_CLEAR_ALL);
        tiny3d_Project2D();

        // Estrelas fixas
        srand(42);
        for (int i = 0; i < 80; i++) {
            float sx = (float)(rand() % 848);
            float sy = (float)(rand() % 512);
            drawRect(sx, sy, 2.0f, 2.0f, COLOR_WHITE);
        }

        // Chão / Superfície da Terra na base da tela
        drawRect(0.0f, 430.0f, 848.0f, 82.0f, COLOR_GREEN);
        drawRect(360.0f, 422.0f, 128.0f, 8.0f, COLOR_GRAY); // Plataforma de lançamento

        // Desenha o foguete
        drawRocket(&rocket);

        tiny3d_Flip();
    }

    return 0;
}
