#ifndef MODELO_H
#define MODELO_H

#include <string.h>

typedef enum
{
    DETENIDO,
    JUGANDO,
    GANO_O,
    GANO_X,
    EMPATE
} estadoJuego;
struct nodoHistorial
{
    gint id;
    gchar tablero[9];
    struct nodoHistorial *anterior;
    struct nodoHistorial *siguiente;
};

estadoJuego obtenerEstadoDelJuego(const gchar *, gint *);
void registrarHistorial(struct nodoHistorial **, gchar[9], int *);
void mostrarHistorial(struct nodoHistorial *);

#endif // MODELO_H