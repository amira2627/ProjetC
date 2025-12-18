
#ifndef EQUIPEMENT_H_INCLUDED
#define EQUIPEMENT_H_INCLUDED

#include <stdio.h>
#include <gtk/gtk.h>   // <<< OBLIGATOIRE POUR GtkTreeView, GtkWidget !!!



/* ================= STRUCTURES ================= */

typedef struct
{
    char nom_equipement[64];
    char categorie[32];
    int  quantite;
    int  prix;
    char centre[32];
    int  salle;
    char etat[20];
} equipement;

typedef struct
{
    char nom_equipement[64];
    char categorie[32];
    char etat[20];
    int  quantite;
    char date_reservation[20];
    int  duree_reservation;
    char centre[32];
    int  salle;
} reservation;

/* ================= CRUD EQUIPEMENT ================= */

int ajouter(const char *filename, equipement e);
int modifier(const char *filename, equipement e);
int supprimer(const char *filename, const char *nom);
int supprimer_reservation_par_nom(const char *nom);
equipement chercher(const char *filename, const char *nom);
int chercher_reservation_par_nom(const char *nom, reservation *r);

/* ================= RESERVATION ================= */

int reserver_equipement(reservation r);

int est_reserve(char *nom);
int derniere_reservation(char *nom, reservation *res);
int get_reservation_by_nom(const char *nom, reservation *r);
void afficher_treeview2_recherche(GtkTreeView *treeview, const char *nom_recherche);
int quantite_reservee(const char *nom_equipement);
/* ================= TREEVIEW ================= */

void vider_treeview(GtkTreeView *treeview);
void afficher_treeview_admin(GtkTreeView *treeview);
void afficher_treeview_coach(GtkTreeView *treeview);
void afficher_treeview_reservation(GtkTreeView *treeview);
void rechercher_reservation_treeview(GtkTreeView *treeview, const char *nom);
void creer_colonnes_treeview2(GtkTreeView *treeview);
void afficher_treeview2(GtkTreeView *treeview);

/* ================= STATISTIQUES ================= */

int total_equipements();
int total_reserves();
void categorie_plus_reservee(char *res);

#endif





