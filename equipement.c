
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "equipement.h"

/* ================= UTILITAIRES ================= */


int est_reserve(char *nom)
{
    return quantite_reservee(nom) > 0;
}

int derniere_reservation(char *nom, reservation *res)
{
    FILE *f = fopen("reservation.txt","r");
    reservation r;
    int found = 0;
    if (!f) return 0;

    while (fscanf(f,"%s %s %s %d %s %d %s %d",
        r.nom_equipement,r.categorie,r.etat,
        &r.quantite,r.date_reservation,
        &r.duree_reservation,r.centre,&r.salle) != EOF)
    {
        if (!strcmp(r.nom_equipement, nom))
        {
            *res = r;
            found = 1;
        }
    }
    fclose(f);
    return found;
}

/* ================= CRUD ================= */

int ajouter(const char *filename, equipement e)
{
    FILE *f = fopen(filename,"a");
    if (!f) return 0;
    fprintf(f,"%s %s %d %d %s %d\n",
        e.nom_equipement,e.categorie,
        e.quantite,e.prix,
        e.centre,e.salle);
    fclose(f);
    return 1;
}

int modifier(const char *filename, equipement eq)
{
    FILE *f = fopen(filename, "r");
    FILE *tmp = fopen("temp.txt", "w");
    equipement e;
    int found = 0;

    if (!f || !tmp) return 0;

    while (fscanf(f, "%s %s %d %d %s %d",
                  e.nom_equipement,
                  e.categorie,
                  &e.quantite,
                  &e.prix,
                  e.centre,
                  &e.salle) != EOF)
    {
        if (strcmp(e.nom_equipement, eq.nom_equipement) == 0)
        {
            e = eq;   // remplace TOUT sauf etat
            found = 1;
        }

        fprintf(tmp, "%s %s %d %d %s %d\n",
                e.nom_equipement,
                e.categorie,
                e.quantite,
                e.prix,
                e.centre,
                e.salle);
    }

    fclose(f);
    fclose(tmp);

    remove(filename);
    rename("temp.txt", filename);

    return found;
}



int supprimer(const char *filename, const char *nom)
{

    FILE *f = fopen(filename, "r");
    FILE *tmp = fopen("temp.txt", "w");
    equipement e;
    int found = 0;

    if (!f || !tmp) return 0;

    while (fscanf(f, "%s %s %d %d %s %d",
        e.nom_equipement,
        e.categorie,
        &e.quantite,
        &e.prix,
        e.centre,
        &e.salle) != EOF)
    {
        if (strcmp(e.nom_equipement, nom) != 0)
        {
            fprintf(tmp, "%s %s %d %d %s %d\n",
                e.nom_equipement,
                e.categorie,
                e.quantite,
                e.prix,
                e.centre,
                e.salle);
        }
        else
        {
            found = 1;   // équipement trouvé et supprimé
        }
    }

    fclose(f);
    fclose(tmp);

    remove(filename);
    rename("temp.txt", filename);

    return found;
}

int supprimer_reservation_par_nom(const char *nom)
{
    FILE *f = fopen("reservation.txt", "r");
    FILE *tmp = fopen("temp_res.txt", "w");
    reservation r;
    int found = 0;

    if (!f || !tmp) return 0;

    while (fscanf(f, "%s %s %s %d %s %d %s %d",
        r.nom_equipement,
        r.categorie,
        r.etat,
        &r.quantite,
        r.date_reservation,
        &r.duree_reservation,
        r.centre,
        &r.salle) != EOF)
    {
        if (strcmp(r.nom_equipement, nom) != 0)
        {
            fprintf(tmp, "%s %s %s %d %s %d %s %d\n",
                r.nom_equipement,
                r.categorie,
                r.etat,
                r.quantite,
                r.date_reservation,
                r.duree_reservation,
                r.centre,
                r.salle);
        }
        else
            found = 1;
    }

    fclose(f);
    fclose(tmp);

    remove("reservation.txt");
    rename("temp_res.txt", "reservation.txt");

    return found;
}



 equipement chercher(const char *filename, const char *nom)
{
    equipement e;
    FILE *f;

    strcpy(e.nom_equipement, "-1");

    f = fopen(filename, "r");
    if (f == NULL)
        return e;

    while (fscanf(f, "%s %s %d %d %s %d",
                  e.nom_equipement,
                  e.categorie,
                  &e.quantite,
                  &e.prix,
                  e.centre,
                  &e.salle) != EOF)
    {
        if (strcmp(e.nom_equipement, nom) == 0)
        {
            fclose(f);
            return e;
        }
    }

    fclose(f);
    strcpy(e.nom_equipement, "-1");
    return e;
}

int chercher_reservation_par_nom(const char *nom, reservation *r)
{
    FILE *f = fopen("reservation.txt", "r");
    if (!f) return 0;

    while (fscanf(f, "%s %s %d %s %d",
                  r->nom_equipement,
                  r->centre,
                  &r->salle,
                  r->date_reservation,
                  &r->duree_reservation) != EOF)
    {
        if (strcmp(r->nom_equipement, nom) == 0)
        {
            fclose(f);
            return 1; // trouvé
        }
    }

    fclose(f);
    return 0; // non trouvé
}




int get_reservation_by_nom(const char *nom, reservation *r)
{
    FILE *f = fopen("reservation.txt", "r");
    if (!f) return 0;

    while (fscanf(f, "%s %s %s %d %s %d %s %d",
                  r->nom_equipement,
                  r->categorie,
                  r->etat,
                  &r->quantite,
                  r->date_reservation,
                  &r->duree_reservation,
                  r->centre,
                  &r->salle) != EOF)
    {
        if (strcmp(r->nom_equipement, nom) == 0)
        {
            fclose(f);
            return 1;  // trouvé
        }
    }

    fclose(f);
    return 0; // non trouvé
}
void afficher_treeview2_recherche(GtkTreeView *treeview, const char *nom_recherche)
{

    GtkListStore *store;
    GtkTreeIter iter;

    if (!gtk_tree_view_get_model(treeview))
    {
        store = gtk_list_store_new(6,
            G_TYPE_STRING, // nom
            G_TYPE_STRING, // categorie
            G_TYPE_INT,    // quantite
            G_TYPE_STRING, // etat
            G_TYPE_STRING, // centre
            G_TYPE_INT     // salle
        );

        gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));
        g_object_unref(store);

        creer_colonnes_treeview2(treeview);
    }

    store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    gtk_list_store_clear(store);

    FILE *f = fopen("equipement10.txt", "r");
    if (!f) return;

    equipement e;
    reservation r;

    while (fscanf(f, "%s %s %d %d %s %d",
                  e.nom_equipement,
                  e.categorie,
                  &e.quantite,
                  &e.prix,
                  e.centre,
                  &e.salle) == 6)
    {
        /* ignorer les réservés */
        if (get_reservation_by_nom(e.nom_equipement, &r))
            continue;

        /* filtrer par nom */
        if (strcmp(e.nom_equipement, nom_recherche) != 0)
            continue;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, e.nom_equipement,
            1, e.categorie,
            2, e.quantite,
            3, "non réservé",
            4, e.centre,
            5, e.salle,
            -1);
    }

    fclose(f);
}

int quantite_reservee(const char *nom_equipement)
{
    FILE *f = fopen("reservation.txt", "r");
    if (!f) return 0;

    reservation r;
    int total = 0;

    while (fscanf(f, "%s %s %d %s %d",
                  r.nom_equipement,
                  r.centre,
                  &r.salle,
                  r.date_reservation,
                  &r.duree_reservation) == 5)
    {
        if (strcmp(r.nom_equipement, nom_equipement) == 0)
            total += r.quantite;
    }

    fclose(f);
    return total;
}


/* ================= RESERVATION ================= */

int reserver_equipement(reservation r)
{
    FILE *f = fopen("reservation.txt","a");
    if (!f) return 0;
    fprintf(f,"%s %s reserve %d %s %d %s %d\n",
        r.nom_equipement,r.categorie,
        r.quantite,r.date_reservation,
        r.duree_reservation,r.centre,r.salle);
    fclose(f);
    return 1;
}

/* ================= TREEVIEWS ================= */

void vider_treeview(GtkTreeView *treeview)
{
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(treeview));
    if (store) gtk_list_store_clear(store);
}

/* ADMIN */

void creer_colonnes_admin(GtkTreeView *treeview)
{
    

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 0, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Catégorie", renderer, "text", 1, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Quantité", renderer, "text", 2, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Prix", renderer, "text", 3, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("État", renderer, "text", 4, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", 5, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Salle", renderer, "text", 6, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Date réservation", renderer, "text", 7, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Durée", renderer, "text", 8, NULL));
}

void afficher_treeview_admin(GtkTreeView *treeview)
{
   GtkListStore *store;
    GtkTreeIter iter;

    if (!gtk_tree_view_get_model(treeview))
    {
        store = gtk_list_store_new(9,
            G_TYPE_STRING, // 0 nom
            G_TYPE_STRING, // 1 categorie
            G_TYPE_INT,    // 2 quantite
            G_TYPE_INT,    // 3 prix
            G_TYPE_STRING, // 4 etat
            G_TYPE_STRING, // 5 centre
            G_TYPE_INT,    // 6 salle
            G_TYPE_STRING, // 7 date
            G_TYPE_INT     // 8 duree
        );

        gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));
        g_object_unref(store);

        creer_colonnes_admin(treeview);
    }

    store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    gtk_list_store_clear(store);

    FILE *f = fopen("equipement10.txt", "r");
    if (!f) return;

    equipement e;
    reservation r;

    while (fscanf(f, "%s %s %d %d %s %d",
                  e.nom_equipement,
                  e.categorie,
                  &e.quantite,
                  &e.prix,
                  e.centre,
                  &e.salle) == 6)
    {
        char centre_final[30];
        int salle_final;
        char date_final[20];
        int duree_final;
        char etat_final[20];

        if (get_reservation_by_nom(e.nom_equipement, &r))
        {
            strcpy(etat_final, "réservé");
            strcpy(centre_final, r.centre);
            salle_final = r.salle;
            strcpy(date_final, r.date_reservation);
            duree_final = r.duree_reservation;
        }
        else
        {
            strcpy(etat_final, "non réservé");
            strcpy(centre_final, e.centre);
            salle_final = e.salle;
            strcpy(date_final, "-");
            duree_final = 0;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, e.nom_equipement,
            1, e.categorie,
            2, e.quantite,
            3, e.prix,
            4, etat_final,
            5, centre_final,
            6, salle_final,
            7, date_final,
            8, duree_final,
            -1);
    }

    fclose(f);
}

void rechercher_reservation_treeview(GtkTreeView *treeview, const char *nom)
{
    GtkListStore *store = GTK_LIST_STORE(
        gtk_tree_view_get_model(treeview));
    GtkTreeIter iter;
    vider_treeview(treeview);

    FILE *f = fopen("reservation.txt","r");
    reservation r;
    if (!f) return;

    while (fscanf(f,"%s %s %s %d %s %d %s %d",
        r.nom_equipement,r.categorie,r.etat,
        &r.quantite,r.date_reservation,
        &r.duree_reservation,r.centre,&r.salle) != EOF)
    {
        if (!strcmp(r.nom_equipement,nom))
        {
            gtk_list_store_append(store,&iter);
            gtk_list_store_set(store,&iter,
                0,r.nom_equipement,
                1,r.quantite,
                2,r.date_reservation,
                3,r.centre,
                4,r.salle,-1);
        }
    }
    fclose(f);
}
void creer_colonnes_treeview2(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 0, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Catégorie", renderer, "text", 1, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Quantité", renderer, "text", 2, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("État", renderer, "text", 3, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", 4, NULL));

    gtk_tree_view_append_column(treeview,
        gtk_tree_view_column_new_with_attributes("Salle", renderer, "text", 5, NULL));
}

void afficher_treeview2(GtkTreeView *treeview)

{
    GtkListStore *store;
    GtkTreeIter iter;

    if (!gtk_tree_view_get_model(treeview))
    {
        store = gtk_list_store_new(6,
            G_TYPE_STRING, // nom
            G_TYPE_STRING, // categorie
            G_TYPE_INT,    // quantite
            G_TYPE_STRING, // etat
            G_TYPE_STRING, // centre
            G_TYPE_INT     // salle
        );

        gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));
        g_object_unref(store);

        creer_colonnes_treeview2(treeview);
    }

    store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    gtk_list_store_clear(store);

    FILE *f = fopen("equipement10.txt", "r");
    if (!f) return;

    equipement e;
    reservation r;

    while (fscanf(f, "%s %s %d %d %s %d",
                  e.nom_equipement,
                  e.categorie,
                  &e.quantite,
                  &e.prix,      // lu mais NON affiché
                  e.centre,
                  &e.salle) == 6)
    {
        /* 🔴 SI réservé → on ignore */
        if (get_reservation_by_nom(e.nom_equipement, &r))
            continue;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, e.nom_equipement,
            1, e.categorie,
            2, e.quantite,
            3, "non réservé",
            4, e.centre,
            5, e.salle,
            -1);
    }

    fclose(f);
}

/* ================= STATISTIQUES ================= */

int total_equipements()
{
    FILE *f = fopen("equipement10.txt","r");
    equipement e; int c=0;
    if (!f) return 0;
    while (fscanf(f,"%s %s %d %d %s %d",
        e.nom_equipement,e.categorie,
        &e.quantite,&e.prix,
        e.centre,&e.salle) != EOF) c++;
    fclose(f);
    return c;
}

int total_reserves()
{
    FILE *f = fopen("equipement10.txt","r");
    equipement e; int c=0;
    if (!f) return 0;
    while (fscanf(f,"%s %s %d %d %s %d",
        e.nom_equipement,e.categorie,
        &e.quantite,&e.prix,
        e.centre,&e.salle) != EOF)
        if (est_reserve(e.nom_equipement)) c++;
    fclose(f);
    return c;
}

void categorie_plus_reservee(char *res)
{
    FILE *f = fopen("reservation.txt","r");
    reservation r;
    int max=0; strcpy(res,"-");
    if (!f) return;

    int c[7]={0};
    while (fscanf(f,"%s %s %*s %*d %*s %*d %*s %*d",
        r.nom_equipement,r.categorie)!=EOF)
    {
        if(!strcmp(r.categorie,"cardio"))c[0]++;
        else if(!strcmp(r.categorie,"musculation"))c[1]++;
        else if(!strcmp(r.categorie,"yoga"))c[2]++;
        else if(!strcmp(r.categorie,"boxe"))c[3]++;
        else if(!strcmp(r.categorie,"pilates"))c[4]++;
        else if(!strcmp(r.categorie,"karate"))c[5]++;
        else if(!strcmp(r.categorie,"dance"))c[6]++;
    }
    fclose(f);

    const char *noms[]={"cardio","musculation","yoga","boxe","pilates","karate","dance"};
    for(int i=0;i<7;i++)
        if(c[i]>max){max=c[i]; strcpy(res,noms[i]);}
}


