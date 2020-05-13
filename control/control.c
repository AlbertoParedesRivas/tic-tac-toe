/*
* @file control.c
*
* @brief Este archivo contiene las funciones que interactuan directamente
* con la vista del programa del juego de gato. Se obtienen los nombres de
* los participantes y se puede jugar solo o con otra persona, tambien se puede
* guardar la partida en un archivo asi como recuperarla.
* 
* @authors  Perla Hernández Cardoso
*           Areli Rabner Concha
* @date 27/04/2020
*/

#include <gtk/gtk.h>
#include "../modelo/modelo.h"

typedef struct
{
    GtkWidget *imagenes[9];
    GtkWidget *labelEstado;
    GtkWidget *ventanaPrincipal;
    GtkWidget *ventanaInicial;
    GtkWidget *ventanaNombres;
    GtkWidget *ventanaNombre;
    GtkWidget *campoJugadorX;
    GtkWidget *campoJugadorXC;
    GtkWidget *campoJugadorO;
    GtkWidget *ventanaCargar;
    GtkWidget *ventanaGuardar;
    gboolean turno;
    gboolean computadora;
    gboolean inicial;
    gchar *textoEstado;
    gchar *jugadorX;
    gchar *jugadorO;
    gchar tablero[9];
    estadoJuego estadoDelJuego;
    struct nodoHistorial *historial;
    int idTablero;
    int tamanoHistorial;
} componentesApp;

void mostrarVentanaInicial(componentesApp *);
void mostrarCargarPartida(GtkMenuItem *, componentesApp *);
void mostrarGuardarPartida(GtkMenuItem *, componentesApp *);
void guardarNombres(GtkButton *, componentesApp *);
void guardarNombre(GtkButton *, componentesApp *);
void nuevoJuegoPersona(GtkButton *, componentesApp *);
void nuevoJuegoComputadora(GtkButton *, componentesApp *);
void mostrarJugadaAnterior(GtkButton *, componentesApp *);
void mostrarJugadaSiguiente(GtkButton *, componentesApp *);
void botonPresionado(GtkButton *, componentesApp *);
void mostrarAcercaDe(GtkMenuItem *, componentesApp *);
void mostrarComoJugar(GtkMenuItem *, componentesApp *);
void mostrarCampoNombreVsJugador();
void nuevoJuego(componentesApp *);
void mostrarTablero(componentesApp *);
void guardarPartida(componentesApp *, gchar *);
void cargarPartida(componentesApp *, gchar *);

/*
* En la función main se inicializan las vistas y las estructuras que guardan información
* que se recopila en la interfaz y el historial.
* @author   Perla Hernández Cardoso
            Areli Rabner Concha
* @param    *aplicacion     Apuntador al objeto aplicacion
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   int
*/
static void activate(GtkApplication *aplicacion, componentesApp *componentes)
{
    GtkWidget *ventana;
    GtkWidget *contenedorPrincipal;
    GtkWidget *contenedorContenido;
    GtkWidget *menuBar;
    GtkWidget *menuJugar;
    GtkWidget *menuArchivo;
    GtkWidget *menuAyuda;
    GtkWidget *submenuJugar;
    GtkWidget *submenuArchivo;
    GtkWidget *submenuAyuda;
    GtkWidget *itemJugarPvP;
    GtkWidget *itemJugarPvE;
    GtkWidget *itemGuardar;
    GtkWidget *itemCargar;
    GtkWidget *itemAcercaDe;
    GtkWidget *itemComoJugar;
    GtkWidget *estadoJuego;
    GtkWidget *gridTablero;
    GtkWidget *gridBotones;
    GtkWidget *botones[9];
    GtkWidget *imagenX;
    GtkWidget *imagenO;
    GtkWidget *imagenFondo[9];
    GtkWidget *botonJugadaAnterior;
    GtkWidget *botonJugadaSiguiente;
    gchar nombre = '0';

    // Inicializando ventana
    ventana = gtk_application_window_new(aplicacion);
    gtk_window_set_default_size(GTK_WINDOW(ventana), 640, 480);

    // Inicializando contenedores
    contenedorPrincipal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    contenedorContenido = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(contenedorContenido), 10);

    // Inicializando opciones de menu
    menuBar = gtk_menu_bar_new();

    menuJugar = gtk_menu_item_new_with_mnemonic("_Juego");
    submenuJugar = gtk_menu_new();
    itemJugarPvP = gtk_menu_item_new_with_label("Jugador contra Jugador");
    itemJugarPvE = gtk_menu_item_new_with_label("Jugador contra Computadora");
    gtk_menu_shell_append(GTK_MENU_SHELL(submenuJugar), itemJugarPvP);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenuJugar), itemJugarPvE);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuJugar), submenuJugar);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuJugar);
    g_signal_connect(itemJugarPvP, "activate", G_CALLBACK(nuevoJuegoPersona), componentes);
    g_signal_connect(itemJugarPvE, "activate", G_CALLBACK(nuevoJuegoComputadora), componentes);

    menuArchivo = gtk_menu_item_new_with_mnemonic("_Archivo");
    submenuArchivo = gtk_menu_new();
    itemGuardar = gtk_menu_item_new_with_label("Guardar partida");
    itemCargar = gtk_menu_item_new_with_label("Cargar partida");
    gtk_menu_shell_append(GTK_MENU_SHELL(submenuArchivo), itemGuardar);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenuArchivo), itemCargar);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuArchivo), submenuArchivo);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuArchivo);
    g_signal_connect(itemCargar, "activate", G_CALLBACK(mostrarCargarPartida), componentes);
    g_signal_connect(itemGuardar, "activate", G_CALLBACK(mostrarGuardarPartida), componentes);

    menuAyuda = gtk_menu_item_new_with_mnemonic("_Ayuda");
    submenuAyuda = gtk_menu_new();
    itemComoJugar = gtk_menu_item_new_with_label("¿Cómo jugar?");
    itemAcercaDe = gtk_menu_item_new_with_label("Acerca de");
    gtk_menu_shell_append(GTK_MENU_SHELL(submenuAyuda), itemComoJugar);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenuAyuda), itemAcercaDe);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuAyuda), submenuAyuda);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), menuAyuda);
    g_signal_connect(itemComoJugar, "activate", G_CALLBACK(mostrarComoJugar), componentes);
    g_signal_connect(itemAcercaDe, "activate", G_CALLBACK(mostrarAcercaDe), componentes);

    // Inicializando tablero
    gridTablero = gtk_grid_new();
    gridBotones = gtk_grid_new();
    estadoJuego = gtk_label_new("Juego de Gato");
    botonJugadaAnterior = gtk_button_new_with_label("Jugada anterior");
    botonJugadaSiguiente = gtk_button_new_with_label("Jugada siguiente");
    g_signal_connect(botonJugadaAnterior, "clicked", G_CALLBACK(mostrarJugadaAnterior), componentes);
    g_signal_connect(botonJugadaSiguiente, "clicked", G_CALLBACK(mostrarJugadaSiguiente), componentes);
    for (gint i = 0; i < 9; i++)
    {
        imagenFondo[i] = gtk_image_new_from_file("img/fondoBlanco.png");
        componentes->imagenes[i] = imagenFondo[i];
        botones[i] = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(botones[i]), imagenFondo[i]);
        g_signal_connect(botones[i], "clicked", G_CALLBACK(botonPresionado), componentes);
        componentes->tablero[i] = '-';
    }
    gtk_widget_set_name(botones[0], "0");
    gtk_widget_set_name(botones[1], "1");
    gtk_widget_set_name(botones[2], "2");
    gtk_widget_set_name(botones[3], "3");
    gtk_widget_set_name(botones[4], "4");
    gtk_widget_set_name(botones[5], "5");
    gtk_widget_set_name(botones[6], "6");
    gtk_widget_set_name(botones[7], "7");
    gtk_widget_set_name(botones[8], "8");
    gtk_grid_attach(GTK_GRID(gridTablero), botones[0], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[1], 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[2], 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[3], 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[4], 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[5], 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[6], 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[7], 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(gridTablero), botones[8], 2, 2, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridTablero), TRUE);
    gtk_grid_attach(GTK_GRID(gridBotones), botonJugadaAnterior, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridBotones), botonJugadaSiguiente, 1, 0, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridBotones), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(gridBotones), 20);

    // Agregando al contenedorContenido
    gtk_box_pack_start(GTK_BOX(contenedorContenido), estadoJuego, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedorContenido), gridTablero, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedorContenido), gridBotones, FALSE, FALSE, 0);

    // Agregando al contenedorPrincipal y ventana
    gtk_box_pack_start(GTK_BOX(contenedorPrincipal), menuBar, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(contenedorPrincipal), contenedorContenido, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(ventana), contenedorPrincipal);

    // Inicializando componentes
    componentes->labelEstado = estadoJuego;
    componentes->ventanaPrincipal = ventana;
    registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);

    gtk_widget_show_all(ventana);
    mostrarVentanaInicial(componentes);
}

/*
* En la función main se inicializan las vistas y las estructuras que guardan información
* que se recopila en la interfaz y el historial.
* @author   Perla Hernández Cardoso
* @param    *argv[]    Arreglo de cadenas donde se guardan los argumentos del programa
* @param    argc       Variable que contiene el numero de argumentos del programa
* @return   int
*/
int main(int argc, char *argv[])
{
    GtkApplication *aplicacion;
    componentesApp *componentes = g_slice_new(componentesApp);
    int estado;

    // Inicializando datos
    componentes->idTablero = 0;
    componentes->tamanoHistorial = 0;
    componentes->turno = FALSE;
    componentes->computadora = FALSE;
    componentes->estadoDelJuego = DETENIDO;
    componentes->inicial = TRUE;

    aplicacion = gtk_application_new("ibero.gtk.gato", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(aplicacion, "activate", G_CALLBACK(activate), componentes);
    estado = g_application_run(G_APPLICATION(aplicacion), argc, argv);
    g_object_unref(aplicacion);
    g_slice_free(componentesApp, componentes);
    return estado;
}

/*
* Esta funcion se ejecuta cuando se da clic en los botones del tablero del juego de gato para registrar un turno.
* Se registra el turno en la variable de tablero, se actualiza la estructura de historial y se actualiza la interfaz
* con el turno registrado. También se encuentra el bloque de código que genera el turno automático cuando solo está
* jugando una persona.
* @author   Areli Rabner Concha
* @param    *boton          Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   void
*/
void botonPresionado(GtkButton *boton, componentesApp *componentes)
{
    gint numeroBoton;
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->estadoDelJuego != JUGANDO)
    {
        return;
    }

    numeroBoton = gtk_widget_get_name(GTK_WIDGET(boton))[0] - '0';
    if (componentes->tablero[numeroBoton] == '-')
    {
        // Se actualiza el estado del juego indicando a quien le toca tirar
        componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorX : componentes->jugadorO, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        // Se actualiza el en la variable que lo maneja
        componentes->tablero[numeroBoton] = componentes->turno ? 'o' : 'x';
        // Se registra el tablero actualizado en el historial y se muestra el tablero actualizado
        componentes->turno = !componentes->turno;
        componentes->estadoDelJuego = obtenerEstadoDelJuego(componentes->tablero);
        registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
        // Se obtiene el estado del juego y se muestra en la interfaz si alguien gano, perdio o si fue empate
        if (componentes->estadoDelJuego == GANO_O)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
        }
        // Seccion que genera el turno de la computadora
        if (componentes->estadoDelJuego == JUGANDO)
        {
            if (componentes->computadora)
            {
                componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorX : componentes->jugadorO, NULL);
                gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
                numeroBoton = generarTiro(componentes->tablero);
                componentes->tablero[numeroBoton] = componentes->turno ? 'o' : 'x';
                componentes->turno = !componentes->turno;
                componentes->estadoDelJuego = obtenerEstadoDelJuego(componentes->tablero);
                registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
                if (componentes->estadoDelJuego == GANO_O)
                {
                    componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
                    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
                }
                else if (componentes->estadoDelJuego == GANO_X)
                {
                    componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
                    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
                }
                else if (componentes->estadoDelJuego == EMPATE)
                {
                    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
                }
            }
        }
        mostrarTablero(componentes);
    }
}
/*
* Función que se ejecuta al momento de que el usuario elige un nuevo juego contra otra persona,
* se oculta la ventana inicial y se muestra la ventana donde se capturan los nombres de las personas
* que jugarán.
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void nuevoJuegoPersona(GtkButton *boton, componentesApp *componentes)
{
    componentes->computadora = FALSE;
    gtk_widget_hide(componentes->ventanaInicial);
    GtkWidget *ventanaNombres;
    GtkWidget *contenedor;
    GtkWidget *instrucciones;
    GtkWidget *jugadorX;
    GtkWidget *jugadorO;
    GtkWidget *campoX;
    GtkWidget *campoO;
    GtkWidget *botonAceptar;
    GtkWidget *gridX;
    GtkWidget *gridO;
    GtkWidget *gridBoton;

    ventanaNombres = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(ventanaNombres), 440, 200);
    gtk_window_set_modal(GTK_WINDOW(ventanaNombres), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(ventanaNombres), 10);

    contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gridX = gtk_grid_new();
    gridO = gtk_grid_new();
    gridBoton = gtk_grid_new();

    instrucciones = gtk_label_new("Ingrese el nombre de los participantes");

    jugadorX = gtk_label_new("Jugador \'X\'");
    jugadorO = gtk_label_new("Jugador \'O\'");
    campoX = gtk_entry_new();
    campoO = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(gridX), jugadorX, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridX), campoX, 1, 0, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridX), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(gridX), 20);
    gtk_grid_attach(GTK_GRID(gridO), jugadorO, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridO), campoO, 1, 0, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridO), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(gridO), 20);

    botonAceptar = gtk_button_new_with_label("Aceptar");
    g_signal_connect(botonAceptar, "clicked", G_CALLBACK(guardarNombres), componentes);
    gtk_grid_attach(GTK_GRID(gridBoton), botonAceptar, 1, 0, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridBoton), TRUE);

    gtk_box_pack_start(GTK_BOX(contenedor), instrucciones, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), gridX, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), gridO, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), gridBoton, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ventanaNombres), contenedor);
    componentes->ventanaNombres = ventanaNombres;
    componentes->campoJugadorX = campoX;
    componentes->campoJugadorO = campoO;
    gtk_widget_show_all(ventanaNombres);
}
/*
* Función que se ejecuta al momento de que el usuario elige un nuevo juego contra la computadora,
* se oculta la ventana inicial y se muestra la ventana donde se captura el nombre de la persona
* que jugará
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void nuevoJuegoComputadora(GtkButton *boton, componentesApp *componentes)
{
    gchar computadora[] = "computadora";
    componentes->computadora = TRUE;
    componentes->jugadorO = g_strdup(computadora);
    gtk_widget_hide(componentes->ventanaInicial);
    GtkWidget *ventanaNombres;
    GtkWidget *contenedor;
    GtkWidget *instrucciones;
    GtkWidget *jugadorX;
    GtkWidget *jugadorO;
    GtkWidget *campoX;
    GtkWidget *campoO;
    GtkWidget *botonAceptar;
    GtkWidget *gridX;
    GtkWidget *gridO;
    GtkWidget *gridBoton;

    ventanaNombres = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(ventanaNombres), 440, 200);
    gtk_window_set_modal(GTK_WINDOW(ventanaNombres), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(ventanaNombres), 10);

    contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gridX = gtk_grid_new();
    gridBoton = gtk_grid_new();

    instrucciones = gtk_label_new("Ingrese el nombre del participante");

    jugadorX = gtk_label_new("Jugador \'X\'");
    campoX = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(gridX), jugadorX, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridX), campoX, 1, 0, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridX), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(gridX), 20);

    botonAceptar = gtk_button_new_with_label("Aceptar");
    g_signal_connect(botonAceptar, "clicked", G_CALLBACK(guardarNombre), componentes);
    gtk_grid_attach(GTK_GRID(gridBoton), botonAceptar, 1, 0, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridBoton), TRUE);

    gtk_box_pack_start(GTK_BOX(contenedor), instrucciones, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), gridX, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), gridBoton, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ventanaNombres), contenedor);

    componentes->ventanaNombre = ventanaNombres;
    componentes->campoJugadorXC = campoX;
    gtk_widget_show_all(ventanaNombres);
}
/*
* Función que recupera los nombres de los campos de la interfaz para guardarlos en la
* estructura que guarda los datos del programa. Después inicia un nuevo juego.
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void guardarNombres(GtkButton *boton, componentesApp *componentes)
{
    if (gtk_entry_get_text_length(GTK_ENTRY(componentes->campoJugadorX)) > 0 && gtk_entry_get_text_length(GTK_ENTRY(componentes->campoJugadorO)) > 0)
    {
        componentes->jugadorX = g_strdup(gtk_entry_get_text(GTK_ENTRY(componentes->campoJugadorX)));
        componentes->jugadorO = g_strdup(gtk_entry_get_text(GTK_ENTRY(componentes->campoJugadorO)));
        gtk_widget_hide(componentes->ventanaNombres);
        nuevoJuego(componentes);
    }
}
/*
* Función que recupera el nombre del campo de la interfaz para guardarlo en la
* estructura que guarda los datos del programa. Después inicia un nuevo juego.
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void guardarNombre(GtkButton *boton, componentesApp *componentes)
{
    if (gtk_entry_get_text_length(GTK_ENTRY(componentes->campoJugadorXC)) > 0)
    {
        componentes->jugadorX = g_strdup(gtk_entry_get_text(GTK_ENTRY(componentes->campoJugadorXC)));
        gtk_widget_hide(componentes->ventanaNombre);
        nuevoJuego(componentes);
    }
}
/*
* Función que inicializa el tablero y los componentes de la interfaz para iniciar un
* nuevo juego.
* @author   Perla Hernández Cardoso
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void nuevoJuego(componentesApp *componentes)
{
    const gchar cadena[] = "Turno de ";
    for (int i = 0; i < 9; i++)
    {
        componentes->tablero[i] = '-';
    }
    for (int i = 0; i < 9; i++)
    {
        gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/fondoBlanco.png");
    }
    componentes->turno = FALSE;
    componentes->textoEstado = g_strconcat(cadena, componentes->jugadorX, NULL);
    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    limpiarHistorial(&componentes->historial, &componentes->idTablero, &componentes->tamanoHistorial);
    componentes->estadoDelJuego = JUGANDO;
}
/*
* Función que muestra en pantalla el tablero activo en el historial
* @author   Areli Rabner Concha
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarTablero(componentesApp *componentes)
{
    struct nodoHistorial *historial = componentes->historial;
    int idTablero = componentes->idTablero;
    if (!historial)
    {
        printf("Historial vacio\n");
    }
    while (historial)
    {
        if (historial->id == idTablero)
        {
            for (int i = 0; i < 9; i++)
            {
                if (historial->tablero[i] == 'x')
                {
                    gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/x.png");
                }
                if (historial->tablero[i] == 'o')
                {
                    gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/o.png");
                }
                if (historial->tablero[i] == '-')
                {
                    gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/fondoBlanco.png");
                }
            }
            break;
        }
        else
        {
            historial = historial->siguiente;
        }
    }
}
/*
* Función que se ejecuta para mostrar jugadas anteriores del historial de partida.
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarJugadaAnterior(GtkButton *boton, componentesApp *componentes)
{
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->idTablero != 1)
    {
        // Sección de código que se encarga de regresar dos jugadas en el historial si se está jugando contra la computadora
        if (componentes->computadora)
        {
            if (componentes->idTablero == componentes->tamanoHistorial)
            {
                if ((componentes->tamanoHistorial % 2) == 0)
                {
                    componentes->idTablero--;
                }
                else
                {
                    componentes->idTablero -= 2;
                }
            }
            else
            {
                componentes->idTablero -= 2;
            }
        }
        else
        {
            componentes->idTablero--;
        }
        recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
        //Sección que actualiza el label de estado en la interfaz
        componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorO : componentes->jugadorX, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        if (componentes->estadoDelJuego == GANO_O)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
        }
    }
    mostrarTablero(componentes);
}
/*
* Función que se ejecuta para mostrar jugadas siguientes del historial de partida.
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarJugadaSiguiente(GtkButton *boton, componentesApp *componentes)
{
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->idTablero < componentes->tamanoHistorial)
    {
        // Sección de código que se encarga de regresar dos jugadas en el historial si se está jugando contra la computadora
        if (componentes->computadora)
        {
            if (componentes->idTablero == componentes->tamanoHistorial - 1)
            {
                componentes->idTablero++;
            }
            else
            {
                componentes->idTablero += 2;
            }
        }
        else
        {
            componentes->idTablero++;
        }
        //Sección que actualiza el label de estado en la interfaz
        recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
        componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorO : componentes->jugadorX, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        if (componentes->estadoDelJuego == GANO_O)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
        }
    }
    mostrarTablero(componentes);
}
/*
* Función que abre la ventana que muestra la vista de acerca de.
* @author   Areli Rabner Concha
* @param    *boton          Apuntador al boton agregado
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarAcercaDe(GtkMenuItem *boton, componentesApp *componentes)
{
    GtkWidget *ventanaInfo;
    GtkWidget *contenedor;
    GtkWidget *titulo;
    GtkWidget *desarrolladores;
    GtkWidget *clase1;
    GtkWidget *clase2;

    ventanaInfo = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(ventanaInfo), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(ventanaInfo), 10);

    contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    titulo = gtk_label_new("Juego de Gato\n");
    desarrolladores = gtk_label_new("Desarrollado por: \nPerla Hernández Cardoso \nAreli Rabner Concha\n");
    clase1 = gtk_label_new("Programación Aplicada");
    clase2 = gtk_label_new("Semestre primavera 2020");

    gtk_box_pack_start(GTK_BOX(contenedor), titulo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), desarrolladores, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), clase1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), clase2, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ventanaInfo), contenedor);

    gtk_widget_show_all(ventanaInfo);
}
/*
* Función que abre la ventana que muestra la vista de como jugar.
* @author   Areli Rabner Concha
* @param    *boton         Apuntador al objeto widget al que se le hizo focus
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarComoJugar(GtkMenuItem *boton, componentesApp *componentes)
{
    GtkWidget *ventanaInfo;
    GtkWidget *contenedor;
    GtkWidget *vistaTexto;
    GtkWidget *ventanaScrolleable;
    GtkTextBuffer *buffer;
    GtkWidget *titulo;
    GtkTextIter iter;
    const gchar *text;

    ventanaInfo = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(ventanaInfo), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(ventanaInfo), 10);

    contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    vistaTexto = gtk_text_view_new();

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(vistaTexto));

    gtk_text_buffer_set_text(buffer, "Este juego de gato tiene dos modos de juego: contra otra persona y contra la computadora. Estos pueden ser cambiados en la sección del menú “Juego”. Al seleccionar un nuevo juego se tienen que registrar el nombre de los participantes.\n\nEl juego tiene opción de guardado y cargado de partidas. Estos se encuentran en el menú de Archivo. \n\nEl juego tiene un sistema de historial. Puedes navegar entre las jugadas de la partida y reanudar el juego en cualquiera de ellas, pero ten en cuenta que si alteras una de las jugadas, las que se encontraban después de estas serán eliminadas.", -1);
    titulo = gtk_label_new("Juego de Gato\n");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(vistaTexto), GTK_WRAP_WORD);

    gtk_box_pack_start(GTK_BOX(contenedor), titulo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), vistaTexto, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ventanaInfo), contenedor);

    gtk_widget_show_all(ventanaInfo);
}
/*
* Función que muestra la ventana inicial al iniciar el programa.
* @author   Perla Hernández Cardoso
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarVentanaInicial(componentesApp *componentes)
{
    GtkWidget *ventanaInicial;
    GtkWidget *contenedor;
    GtkWidget *bienvenida;
    GtkWidget *pregunta;
    GtkWidget *botonJugador;
    GtkWidget *botonComputadora;
    GtkWidget *gridOpciones;

    ventanaInicial = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(ventanaInicial), 440, 200);
    gtk_window_set_modal(GTK_WINDOW(ventanaInicial), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(ventanaInicial), 10);

    contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gridOpciones = gtk_grid_new();

    bienvenida = gtk_label_new("Bienvenido");
    pregunta = gtk_label_new("¿Contra quién deseas jugar?");
    botonJugador = gtk_button_new_with_label("Contra otro jugador");
    botonComputadora = gtk_button_new_with_label("Contra la computadora");
    g_signal_connect(botonJugador, "clicked", G_CALLBACK(nuevoJuegoPersona), componentes);
    g_signal_connect(botonComputadora, "clicked", G_CALLBACK(nuevoJuegoComputadora), componentes);

    gtk_grid_attach(GTK_GRID(gridOpciones), botonJugador, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gridOpciones), botonComputadora, 0, 1, 1, 1);
    gtk_grid_set_column_homogeneous(GTK_GRID(gridOpciones), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(gridOpciones), 20);

    gtk_box_pack_start(GTK_BOX(contenedor), bienvenida, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), pregunta, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contenedor), gridOpciones, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ventanaInicial), contenedor);
    componentes->ventanaInicial = ventanaInicial;
    gtk_widget_show_all(ventanaInicial);
}
/*
* Función que muetra el selector de archivo y recupera la dirección del archivo que contiene la partida
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarCargarPartida(GtkMenuItem *boton, componentesApp *componentes)
{
    gchar *nombreArchivo = NULL;

    GtkWidget *ventanaCargar;
    GtkFileChooserAction accion = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint respuesta;

    ventanaCargar = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(componentes->ventanaPrincipal), accion, ("_Cancelar"), GTK_RESPONSE_CANCEL, ("_Abrir"), GTK_RESPONSE_ACCEPT, NULL);

    respuesta = gtk_dialog_run(GTK_DIALOG(ventanaCargar));
    if (respuesta == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(ventanaCargar);
        nombreArchivo = gtk_file_chooser_get_filename(chooser);
        cargarPartida(componentes, nombreArchivo);
        g_free(nombreArchivo);
    }

    gtk_widget_destroy(ventanaCargar);
}
/*
* Función que muetra el selector de archivo y recupera la dirección del archivo nuevo para guardar la partida
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarGuardarPartida(GtkMenuItem *boton, componentesApp *componentes)
{
    gchar *nombreArchivo = NULL;

    GtkWidget *ventanaGuardar;
    GtkFileChooser *selector;
    GtkFileChooserAction accion = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint respuesta;

    ventanaGuardar = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(componentes->ventanaPrincipal), accion, ("_Cancelar"), GTK_RESPONSE_CANCEL, ("_Guardar"), GTK_RESPONSE_ACCEPT, NULL);
    selector = GTK_FILE_CHOOSER(ventanaGuardar);

    gtk_file_chooser_set_do_overwrite_confirmation(selector, TRUE);

    respuesta = gtk_dialog_run(GTK_DIALOG(ventanaGuardar));
    if (respuesta == GTK_RESPONSE_ACCEPT)
    {
        nombreArchivo = gtk_file_chooser_get_filename(selector);
        guardarPartida(componentes, nombreArchivo);
        g_free(nombreArchivo);
    }

    gtk_widget_destroy(ventanaGuardar);
}
/*
* Función que guarda los datos de la partida en un archivo
* @author   Perla Hernández Cardoso
* @param    *componentes        Estructura que contiene las variables principales del programa
* @param    *direccionArchivo   Apuntador a la cadena de la ruta del archivo
* @return   void
*/
void guardarPartida(componentesApp *componentes, gchar *direccionArchivo)
{
    FILE *guardado;
    struct nodoHistorial *temporal = componentes->historial;
    guardado = fopen(direccionArchivo, "w");
    if (guardado == NULL)
    {
        printf("Error guardando\n");
        return;
    }

    fprintf(guardado, "%s\n", componentes->jugadorX);
    fprintf(guardado, "%s\n", componentes->jugadorO);

    while (temporal)
    {
        if (temporal->id != 1)
        {
            fprintf(guardado, "%d\n", temporal->turno);
            fprintf(guardado, "%d\n", (int)temporal->estado);
            for (int i = 0; i < 9; i++)
            {
                fprintf(guardado, "%c", temporal->tablero[i]);
            }
            fprintf(guardado, "\n");
            temporal = temporal->siguiente;
        }
        else
        {
            temporal = temporal->siguiente;
        }
    }

    fclose(guardado);
}
/*
* Función que carga los datos de la partida de un archivo
* @author   Areli Rabner Concha
* @param    *componentes        Estructura que contiene las variables principales del programa
* @param    *direccionArchivo   Apuntador a la cadena de la ruta del archivo
* @return   void
*/
void cargarPartida(componentesApp *componentes, gchar *direccionArchivo)
{
    FILE *archivo;
    char computadora[] = "computadora\0";
    archivo = fopen(direccionArchivo, "r");
    char buffer[128], tablero[9], numero[2];
    int contador = 0;
    limpiarHistorial(&componentes->historial, &componentes->idTablero, &componentes->tamanoHistorial);
    // Ciclo que recupera una linea del archivo de guardado y almacena los datos en sus respectivas variables
    while (fgets(buffer, sizeof(buffer), archivo))
    {
        buffer[strlen(buffer) - 1] = '\0';
        if (contador == 0)
        {
            componentes->jugadorX = g_strdup(buffer);
        }
        else if (contador == 1)
        {
            componentes->jugadorO = g_strdup(buffer);
            if (g_strcmp0(computadora, componentes->jugadorO) == 0)
            {
                componentes->computadora = TRUE;
            }
            else
            {
                componentes->computadora = FALSE;
            }
        }
        else if (contador == 2)
        {
            numero[0] = buffer[0];
            numero[1] = '\0';
            componentes->turno = atoi(numero);
        }
        else if (contador == 3)
        {
            numero[0] = buffer[0];
            numero[1] = '\0';
            componentes->estadoDelJuego = atoi(numero);
        }
        else if (contador == 4)
        {
            for (int k = 0; k < 9; k++)
            {
                tablero[k] = buffer[k];
            }
            registrarHistorial(&componentes->historial, tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
        }
        if (contador >= 4)
        {
            contador = 2;
        }
        else
        {
            contador++;
        }
    }
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    //Muestra la última jugada del historial y determina el estado del juego.
    recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
    componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorO : componentes->jugadorX, NULL);
    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    if (componentes->estadoDelJuego == GANO_O)
    {
        componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    }
    else if (componentes->estadoDelJuego == GANO_X)
    {
        componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    }
    else if (componentes->estadoDelJuego == EMPATE)
    {
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
    }
    mostrarTablero(componentes);
    fclose(archivo);
}