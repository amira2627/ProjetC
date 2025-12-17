#include <gtk/gtk.h>
#include <glade/glade.h>
#include <stdio.h>
#include <string.h>

#include "callbacks.h"

/* xml global utilisé dans callbacks.c via: extern GladeXML *xml; */
GladeXML *xml = NULL;

/* ---- Connecteur libglade: handler_name -> fonction ---- */
static void connect_glade_signals(const gchar *handler_name,
                                  GObject     *object,
                                  const gchar *signal_name,
                                  const gchar *signal_data,
                                  GObject     *connect_object,
                                  gboolean     after,
                                  gpointer     user_data)
{
    (void)signal_data;
    (void)user_data;

    GCallback cb = NULL;

    /* admin / fiche */
    if      (strcmp(handler_name,"on_khayri40_clicked")==0)  cb = G_CALLBACK(on_khayri40_clicked);
    else if (strcmp(handler_name,"on_khayri300_clicked")==0) cb = G_CALLBACK(on_khayri300_clicked);
    else if (strcmp(handler_name,"on_khayri9_clicked")==0)   cb = G_CALLBACK(on_khayri9_clicked);
    else if (strcmp(handler_name,"on_khayri8_clicked")==0)   cb = G_CALLBACK(on_khayri8_clicked);
    else if (strcmp(handler_name,"on_khayri7_clicked")==0)   cb = G_CALLBACK(on_khayri7_clicked);

    else if (strcmp(handler_name,"on_khayri41_clicked")==0)  cb = G_CALLBACK(on_khayri41_clicked);

    /* compat */
    else if (strcmp(handler_name,"on_khayri81_clicked")==0)  cb = G_CALLBACK(on_khayri81_clicked);
    else if (strcmp(handler_name,"on_khayri80_clicked")==0)  cb = G_CALLBACK(on_khayri80_clicked);
    else if (strcmp(handler_name,"on_fiche_modifier_clicked")==0) cb = G_CALLBACK(on_fiche_modifier_clicked);

    /* stats */
    else if (strcmp(handler_name,"on_khayri900_clicked")==0) cb = G_CALLBACK(on_khayri900_clicked);

    /* treeview */
    else if (strcmp(handler_name,"on_treeview1_row_activated")==0)
        cb = G_CALLBACK(on_treeview1_row_activated);

    /* destroy */
    else if (strcmp(handler_name,"on_window_destroy")==0)
        cb = G_CALLBACK(on_window_destroy);

    if (!cb) return;

    if (connect_object) {
        if (after)
            g_signal_connect_object(object, signal_name, cb, connect_object, G_CONNECT_AFTER);
        else
            g_signal_connect_object(object, signal_name, cb, connect_object, 0);
    } else {
        if (after)
            g_signal_connect_after(object, signal_name, cb, NULL);
        else
            g_signal_connect(object, signal_name, cb, NULL);
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    glade_init();

    xml = glade_xml_new("project3.glade", NULL, NULL);
    if (!xml) {
        g_printerr("Erreur : impossible de charger project3.glade\n");
        return 1;
    }

    glade_xml_signal_autoconnect_full(xml, connect_glade_signals, NULL);

    /* Fenêtres */
    GtkWidget *win_admin = glade_xml_get_widget(xml, "admin gestion");
    if (!win_admin) win_admin = glade_xml_get_widget(xml, "window1");

    GtkWidget *win_fiche = glade_xml_get_widget(xml, "window3");

    if (!win_admin) {
        g_printerr("Erreur : fenêtre admin introuvable (admin gestion/window1)\n");
        return 1;
    }

    gtk_widget_show_all(win_admin);
    if (win_fiche) gtk_widget_hide(win_fiche);

    gtk_main();
    return 0;
}

