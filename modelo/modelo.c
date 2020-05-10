#include <gtk/gtk.h>
#include "modelo.h"

static gint revisarGanador(const gint combinacionesGanadoras[][3], const gint tamano, const gchar jugadorProbado, const char *tablero, gint *bloquesGanadores);
static gboolean tableroLleno(const gchar *tablero);

const gint combinacionesGanadorasHorizontales[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
const gint combinacionesGanadorasVerticales[3][3] = {{0, 3, 6}, {1, 4, 7}, {2, 5, 8}};
const gint combinacionesGanadorasDiagonales[2][3] = {{0, 4, 8}, {2, 4, 6}};

estadoJuego obtenerEstadoDelJuego(const gchar *tablero, gint *bloquesGanadores)
{
    estadoJuego estado = JUGANDO;
    gint resultado;

    resultado = revisarGanador(combinacionesGanadorasHorizontales, 3, 'x', tablero, bloquesGanadores);
    if (resultado > -1)
    {
        return GANO_X;
    }
    resultado = revisarGanador(combinacionesGanadorasVerticales, 3, 'x', tablero, bloquesGanadores);
    if (resultado > -1)
    {
        return GANO_X;
    }
    resultado = revisarGanador(combinacionesGanadorasDiagonales, 2, 'x', tablero, bloquesGanadores);
    if (resultado > -1)
    {
        return GANO_X;
    }
    resultado = revisarGanador(combinacionesGanadorasHorizontales, 3, 'o', tablero, bloquesGanadores);
    if (resultado > -1)
    {
        return GANO_O;
    }
    resultado = revisarGanador(combinacionesGanadorasVerticales, 3, 'o', tablero, bloquesGanadores);
    if (resultado > -1)
    {
        return GANO_O;
    }
    resultado = revisarGanador(combinacionesGanadorasDiagonales, 2, 'o', tablero, bloquesGanadores);
    if (resultado > -1)
    {
        return GANO_O;
    }

    if (tableroLleno(tablero))
    {
        estado = EMPATE;
    }

    return estado;
}

static gint revisarGanador(const gint combinacionesGanadoras[][3], const gint tamano, const gchar jugadorProbado, const char *tablero, gint *bloquesGanadores)
{
    gint i, j;
    gint contador = 0;

    for (i = 0; i < tamano; i++)
    {
        contador = 0;
        for (j = 0; j < 3; j++)
        {
            if (jugadorProbado == tablero[combinacionesGanadoras[i][j]])
            {
                bloquesGanadores[j] = combinacionesGanadoras[i][j];
                contador++;
                if (contador == 3)
                {
                    return i;
                }
            }
        }
    }

    return -1;
}

static gboolean tableroLleno(const gchar *tablero)
{
    int i;

    for (i = 0; i < 9; i++)
    {
        if (tablero[i] == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}