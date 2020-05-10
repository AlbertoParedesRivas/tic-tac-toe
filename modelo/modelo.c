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

void registrarHistorial(struct nodoHistorial **historial, gchar tablero[9], int *idTablero)
{
    if ((*historial) == NULL)
    {
        (*historial) = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
        (*historial)->id = 1;
        (*historial)->anterior = NULL;
        (*historial)->siguiente = NULL;
        memcpy((*historial)->tablero, tablero, sizeof(gchar) * 9);
        return;
    }

    struct nodoHistorial *temporal = (*historial);
    struct nodoHistorial *temporalDos = NULL;

    while (temporal)
    {
        if (temporal->siguiente == NULL)
        {
            temporalDos = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
            temporalDos->id = temporal->id + 1;
            memcpy(temporalDos->tablero, tablero, sizeof(gchar) * 9);
            temporalDos->anterior = temporal;
            temporalDos->siguiente = NULL;
            temporal->siguiente = temporalDos;
            break;
        }
        else
        {
            temporal = temporal->siguiente;
        }
    }
    (*idTablero) = temporalDos->id;
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
        if (tablero[i] == '-')
        {
            return FALSE;
        }
    }

    return TRUE;
}