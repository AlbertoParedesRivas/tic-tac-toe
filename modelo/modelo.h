#ifndef MODELO_H
#define MODELO_H

typedef enum
{
    JUGANDO,
    GANO_O,
    GANO_X,
    EMPATE
} estadoJuego;

estadoJuego obtenerEstadoDelJuego(const gchar *gm_board, gint *win_blocks);

#endif // MODELO_H