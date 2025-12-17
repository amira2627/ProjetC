#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

/* ----- Membre: fiche / admin ----- */
void on_khayri40_clicked (GtkButton *button, gpointer user_data); /* ajouter */
void on_khayri300_clicked(GtkButton *button, gpointer user_data); /* afficher */
void on_khayri9_clicked  (GtkButton *button, gpointer user_data); /* rechercher */
void on_khayri8_clicked  (GtkButton *button, gpointer user_data); /* supprimer */
void on_khayri7_clicked  (GtkButton *button, gpointer user_data); /* modifier (admin) */
void on_khayri41_clicked (GtkButton *button, gpointer user_data); /* modifier (fiche) */

/* wrappers (si encore présents dans Glade) */
void on_khayri81_clicked(GtkButton *button, gpointer user_data);
void on_khayri80_clicked(GtkButton *button, gpointer user_data);
void on_fiche_modifier_clicked(GtkButton *button, gpointer user_data);

/* ----- Statistiques ----- */
void on_khayri900_clicked(GtkButton *button, gpointer user_data);

/* Optionnel : filtrer la saisie clavier de entry7 (si tu connectes insert-text dans Glade) */
void on_entry7_insert_text(GtkEditable *editable,
                           const gchar *text,
                           gint length,
                           gint *position,
                           gpointer data);

/* ----- Divers ----- */
void on_window_destroy(GtkWidget *widget, gpointer user_data);

void on_treeview1_row_activated(GtkTreeView       *treeview,
                                GtkTreePath       *path,
                                GtkTreeViewColumn *column,
                                gpointer           user_data);

#endif

