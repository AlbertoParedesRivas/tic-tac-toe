#include <gtk/gtk.h>
#include "../modelo/modelo.h"

typedef struct
{
    GtkWidget *imagenes[9];
    GtkWidget *labelEstado;
    gboolean turno;
    gchar tablero[9];
    estadoJuego estadoDelJuego;
} componentesApp;

int main(int argc, char *argv[])
{
    GtkBuilder *constructor;
    GtkWidget *ventana;
    componentesApp *componentes = g_slice_new(componentesApp);
    gchar nombreImagenes[] = "imagen1";

    componentes->turno = FALSE;
    componentes->estadoDelJuego = JUGANDO;

    gtk_init(&argc, &argv);

    constructor = gtk_builder_new_from_file("vista/tablero.ui");
    ventana = GTK_WIDGET(gtk_builder_get_object(constructor, "TicTacToe"));

    for (gint i = 0; i < 9; i++)
    {
        nombreImagenes[6] = i + '1';
        componentes->imagenes[i] = GTK_WIDGET(gtk_builder_get_object(constructor, nombreImagenes));
        componentes->tablero[i] = 0;
    }
    componentes->labelEstado = GTK_WIDGET(gtk_builder_get_object(constructor, "estadoJuego"));

    gtk_builder_connect_signals(constructor, componentes);
    g_object_unref(constructor);

    gtk_widget_show(ventana);
    gtk_main();
    g_slice_free(componentesApp, componentes);

    return 0;
}

void botonPresionado(GtkButton *boton, componentesApp *componentes)
{
    gint numeroBoton;
    gint bloquesGanadores[3] = {0};

    if (componentes->estadoDelJuego != JUGANDO)
    {
        return;
    }

    numeroBoton = gtk_widget_get_name(GTK_WIDGET(boton))[0] - '0';
    printf("%d\n", numeroBoton);
    if (componentes->tablero[numeroBoton] == 0)
    {
        gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[numeroBoton]), componentes->turno ? "img/o.png" : "img/x.png");
        componentes->tablero[numeroBoton] = componentes->turno ? 'o' : 'x';

        componentes->estadoDelJuego = obtenerEstadoDelJuego(componentes->tablero, bloquesGanadores);

        if (componentes->estadoDelJuego == GANO_O)
        {
            for (int i = 0; i < 3; i++)
            {
            }
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "Gano O!");
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            for (int i = 0; i < 3; i++)
            {
            }
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "Gano X!");
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "El juego fue un empate!");
        }

        componentes->turno = !componentes->turno;
    }
}

void nuevoJuego(GtkButton *boton, componentesApp *componentes)
{
    for (int i = 0; i < 9; i++)
    {
        componentes->tablero[i] = 0;
    }
    for (int i = 0; i < 9; i++)
    {
        gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/fondoBlanco.png");
    }
    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "Gato");

    componentes->estadoDelJuego = JUGANDO;
}

void on_window_main_destroy()
{
    gtk_main_quit();
}

void mostrarAcercaDe()
{
    GtkBuilder *constructor;
    GtkWidget *ventanaAcercaDe = NULL;

    constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, "vista/acercaDe.ui", NULL);

    ventanaAcercaDe = GTK_WIDGET(gtk_builder_get_object(constructor, "acercaDe"));

    g_object_unref(G_OBJECT(constructor));

    gtk_window_set_modal(GTK_WINDOW(ventanaAcercaDe), TRUE);
    gtk_widget_show_all(ventanaAcercaDe);
}

void mostrarComoJugar()
{
    GtkBuilder *constructor;
    GtkWidget *ventanaComoJugar = NULL;

    constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, "vista/comoJugar.ui", NULL);

    ventanaComoJugar = GTK_WIDGET(gtk_builder_get_object(constructor, "comoJugar"));

    g_object_unref(G_OBJECT(constructor));

    gtk_window_set_modal(GTK_WINDOW(ventanaComoJugar), TRUE);
    gtk_widget_show_all(ventanaComoJugar);
}