#include <gtk/gtk.h>
#include "modelo.h"

static gint revisarGanador(const gint combinacionesGanadoras[][3], const gint tamano, const gchar jugadorProbado, const char *tablero, gint *bloquesGanadores);
static gboolean tableroLleno(const gchar *tablero);
static void mostrarTableroA(gchar[]);

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

static void mostrarTableroA(gchar tablero[9])
{
    int c = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%c", tablero[c]);
            c++;
        }
        printf("\n");
    }
}

void mostrarHistorial(struct nodoHistorial *historial)
{
    if (!historial)
    {
        printf("Historial vacio\n");
    }
    struct nodoHistorial *temporal = historial;
    while (temporal)
    {
        printf("id:%d\n", temporal->id);
        mostrarTableroA(temporal->tablero);
        temporal = temporal->siguiente;
    }
}

void registrarHistorial(struct nodoHistorial **historial, gchar tablero[9], int *idTablero, int *tamanoHistorial, gboolean turno, estadoJuego estado)
{
    if ((*historial) == NULL)
    {
        (*historial) = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
        (*historial)->id = 1;
        (*historial)->anterior = NULL;
        (*historial)->siguiente = NULL;
        (*historial)->turno = turno;
        (*historial)->estado = JUGANDO;
        memcpy((*historial)->tablero, tablero, sizeof(gchar) * 9);
        (*idTablero) = (*idTablero) + 1;
        (*tamanoHistorial) = (*tamanoHistorial) + 1;
        return;
    }

    struct nodoHistorial *temporal = (*historial);
    struct nodoHistorial *temporalDos = NULL, *temporalEliminar = NULL;

    if ((*idTablero) == (*tamanoHistorial))
    {
        while (temporal)
        {
            if (temporal->siguiente == NULL)
            {
                temporalDos = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
                temporalDos->id = temporal->id + 1;
                memcpy(temporalDos->tablero, tablero, sizeof(gchar) * 9);
                temporalDos->anterior = temporal;
                temporalDos->siguiente = NULL;
                temporalDos->turno = turno;
                temporalDos->estado = estado; 
                temporal->siguiente = temporalDos;
                break;
            }
            else
            {
                temporal = temporal->siguiente;
            }
        }
        (*idTablero) = (*idTablero) + 1;
        (*tamanoHistorial) = (*tamanoHistorial) + 1;
    }
    else if ((*idTablero) < (*tamanoHistorial))
    {
        while (temporal)
        {
            if (temporal->siguiente == NULL)
            {
                do
                {
                    if (temporal->id == (*idTablero))
                    {
                        break;
                    }
                    else
                    {
                        temporalEliminar = temporal;
                        temporal = temporal->anterior;
                        temporal->siguiente = NULL;
                        free(temporalEliminar);
                    }
                } while (temporal);
                temporalDos = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
                temporalDos->id = temporal->id + 1;
                memcpy(temporalDos->tablero, tablero, sizeof(gchar) * 9);
                temporalDos->anterior = temporal;
                temporalDos->siguiente = NULL;
                temporalDos->turno = turno;
                temporalDos->estado = estado;
                temporal->siguiente = temporalDos;
                break;
            }
            else
            {
                temporal = temporal->siguiente;
            }
        }
        (*idTablero) = (*idTablero) + 1;
        (*tamanoHistorial) = (*idTablero);
    }
}

void recuperarTablero(struct nodoHistorial *historial, gchar (*tablero)[9], gboolean *turno, estadoJuego *estado, const int idTablero){
    struct nodoHistorial *temporal = historial;
    if (historial == NULL)
    {
        return;
    }
    while (temporal)
    {
        if (temporal->id == idTablero)
        {
            for (int i = 0; i < 9; i++)
            {
                (*tablero)[i] = temporal->tablero[i];
            }
            (*turno) = temporal->turno;
            (*estado) = temporal->estado;
            break;
        }
        else
        {
            temporal = temporal->siguiente;
        }
    }
}

void limpiarHistorial(struct nodoHistorial **historial, int *idTablero, int *tamanoHistorial)
{
    struct nodoHistorial *temporal = (*historial), *temporalEliminar = NULL;
    if ((*tamanoHistorial) != 1)
    {
        while (temporal)
        {
            if (temporal->siguiente == NULL)
            {
                do
                {
                    if (temporal->id == 1)
                    {
                        break;
                    }
                    else
                    {
                        temporalEliminar = temporal;
                        temporal = temporal->anterior;
                        temporal->siguiente = NULL;
                        free(temporalEliminar);
                    }
                } while (temporal);
                break;
            }
            else
            {
                temporal = temporal->siguiente;
            }
        }
        (*idTablero) = 1;
        (*tamanoHistorial) = 1;
    }
    
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