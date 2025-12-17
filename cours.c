/*#include <gtk/gtk.h>
#include <string.h>
#include "cours.h"

int ajouter_cours(char *filename, Cours_admin c)
{  
    FILE *f = fopen(filename, "a");
   
    if(f != NULL)  
    {
        fprintf(f, "%d %s %s %s %s %d %d %d %f %d\n",
                c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                c.duree_min,
                c.capacite_max,
                c.heure,
                c.prix,
                c.salle);
        fclose(f);  
        return 1;  
    }
    else
        return 0;  
}

enum
{
    COLUMN_ID,
    COLUMN_NOM_COUR,
    COLUMN_JOUR,
    COLUMN_NIVEAU,              
    COLUMN_CENTRE,
    COLUMN_DUREE_MIN,
    COLUMN_CAPACITE_MAX,
    COLUMN_HEURE,
    COLUMN_PRIX,
    COLUMN_SALLE,
    NUM_COLUMNS
};

void creer_colonnes(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COLUMN_ID, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom cours", renderer, "text", COLUMN_NOM_COUR, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Jour", renderer, "text", COLUMN_JOUR, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Niveau", renderer, "text", COLUMN_NIVEAU, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", COLUMN_CENTRE, NULL);
    gtk_tree_view_append_column(treeview, column);
   
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Durée (min)", renderer, "text", COLUMN_DUREE_MIN, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Capacité max", renderer, "text", COLUMN_CAPACITE_MAX, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Heure", renderer, "text", COLUMN_HEURE, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prix", renderer, "text", COLUMN_PRIX, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Salle", renderer, "text", COLUMN_SALLE, NULL);
    gtk_tree_view_append_column(treeview, column);
}

void afficher_cour(GtkWidget *liste)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GtkTreeView *treeview = GTK_TREE_VIEW(liste);

    // Vérifier si les colonnes existent déjà
    if (g_list_length(gtk_tree_view_get_columns(treeview)) == 0)
    {
        creer_colonnes(treeview);
    }

    // Créer le store
    store = gtk_list_store_new(NUM_COLUMNS,
                               G_TYPE_INT,    // ID
                               G_TYPE_STRING, // Nom_cour
                               G_TYPE_STRING, // jour
                               G_TYPE_STRING, // niveau
                               G_TYPE_STRING, // centre
                               G_TYPE_INT,    // duree_min
                               G_TYPE_INT,    // capacite_max
                               G_TYPE_INT,    // heure
                               G_TYPE_FLOAT,  // prix
                               G_TYPE_INT);   // salle

    // Variables pour lire le fichier
    int id_temp, duree_temp, capacite_temp, heure_temp, salle_temp;
    float prix_temp;
    char nom_temp[32], jour_temp[32], niveau_temp[32], centre_temp[32];

    FILE *f = fopen("cours.txt", "r");
    if (!f)
    {
        g_print("Erreur : impossible d'ouvrir cours.txt\n");
        gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));
        g_object_unref(store);
        return;
    }

    // Lecture et insertion dans le store
    while (fscanf(f, "%d %31s %31s %31s %31s %d %d %d %f %d",
                  &id_temp, nom_temp, jour_temp, niveau_temp, centre_temp,
                  &duree_temp, &capacite_temp, &heure_temp, &prix_temp, &salle_temp) == 10)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COLUMN_ID, id_temp,
                           COLUMN_NOM_COUR, nom_temp,
                           COLUMN_JOUR, jour_temp,
                           COLUMN_NIVEAU, niveau_temp,
                           COLUMN_CENTRE, centre_temp,
                           COLUMN_DUREE_MIN, duree_temp,
                           COLUMN_CAPACITE_MAX, capacite_temp,
                           COLUMN_HEURE, heure_temp,
                           COLUMN_PRIX, prix_temp,
                           COLUMN_SALLE, salle_temp,
                           -1);
    }

    fclose(f);

    // Attacher le modèle au TreeView
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));

    // Libérer le store (le TreeView garde une référence)
    g_object_unref(store);
}

void supprimer_cour(Cours_admin c)
{
    Cours_admin temp;
    FILE *f = fopen("cours.txt", "r");
    FILE *g = fopen("tmp.txt", "w");

    if (f == NULL || g == NULL)
    {
        if (f) fclose(f);
        if (g) fclose(g);
        return;
    }

    while (fscanf(f, "%d %31s %31s %31s %31s %d %d %d %f %d",
                &temp.ID,
                temp.Nom_du_cour,
                temp.jour,
                temp.niveau,
                temp.centre,
                &temp.duree_min,
                &temp.capacite_max,
                &temp.heure,
                &temp.prix,
                &temp.salle) == 10)
    {
        if (temp.ID != c.ID)
        {
            fprintf(g, "%d %s %s %s %s %d %d %d %f %d\n",
                temp.ID,
                temp.Nom_du_cour,
                temp.jour,
                temp.niveau,
                temp.centre,
                temp.duree_min,
                temp.capacite_max,
                temp.heure,
                temp.prix,
                temp.salle);
        }
    }

    fclose(f);
    fclose(g);

    remove("cours.txt");
    rename("tmp.txt", "cours.txt");
}

void vider(GtkWidget *liste)
{
    GtkTreeModel *model;
    GtkListStore *store;
   
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(liste));
   
    if (model != NULL)
    {
        store = GTK_LIST_STORE(model);
        gtk_list_store_clear(store);
    }
}*/
#include <gtk/gtk.h>
#include <string.h>
#include "cours.h"
#include <stdlib.h>
#include <time.h>
// Fonction pour vérifier si un ID existe déjà dans le fichier
int id_existe(char *filename, int id)
{
    Cours_admin c;
    FILE *f = fopen(filename, "r");
    
    if(f == NULL)
        return 0; // Le fichier n'existe pas encore, donc l'ID n'existe pas
    
    while(fscanf(f, "%d %s %s %s %s %d %d %d %f %d",
                &c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                &c.duree_min,
                &c.capacite_max,
                &c.heure,
                &c.prix,
                &c.salle) != EOF)
    {
        if(c.ID == id)
        {
            fclose(f);
            return 1; // L'ID existe déjà
        }
    }
    
    fclose(f);
    return 0; // L'ID n'existe pas
}
// Fonction pour générer un ID unique (entre 100000 et 999999)
int generer_id_unique(char *filename)
{
    int id;
    static int initialized = 0;
    
    // Initialiser le générateur de nombres aléatoires une seule fois
    if(!initialized)
    {
        srand(time(NULL));
        initialized = 1;
    }
    
    // Générer un ID aléatoire jusqu'à trouver un ID unique
    do {
        // Générer un nombre entre 100000 et 999999 (6 chiffres)
        id = 100000 + rand() % 900000;
    } while(id_existe(filename, id));
    
    return id;
}
int ajouter_cours(char *filename, Cours_admin c)
{  
    FILE *f = fopen(filename, "a");
   
    if(f != NULL)  
    {
        fprintf(f, "%d %s %s %s %s %d %d %d %f %d\n",
                c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                c.duree_min,
                c.capacite_max,
                c.heure,
                c.prix,
                c.salle);
        fclose(f);  
        return 1;  
    }
    else
        return 0;  
}
int modifier_cours(char *filename,  int ID, Cours_admin nouv)
{
    int tr = 0;
    Cours_admin c;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("nouv.txt", "w");
   
    if(f != NULL && f2 != NULL)
    {
        while(fscanf(f, "%d %s %s %s %s %d %d %d %f %d \n",
                &c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                &c.duree_min,
                &c.capacite_max,
                &c.heure,
                &c.prix,
                &c.salle) != EOF)
        {
            if(c.ID==ID)
            {
                fprintf(f2, "%d %s %s %s %s %d %d %d %f %d \n",
                nouv.ID,
                nouv.Nom_du_cour,
                nouv.jour,
                nouv.niveau,
                nouv.centre,
                nouv.duree_min,
                nouv.capacite_max,
                nouv.heure,
                nouv.prix,
                nouv.salle);
                tr = 1;
            }
            else
            {
                fprintf(f2, "%d %s %s %s %s %d %d %d %f %d \n",
                c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                c.duree_min,
                c.capacite_max,
                c.heure,
                c.prix,
                c.salle);
            }
        }
    }
   
    fclose(f);
    fclose(f2);
    remove(filename);
    rename("nouv.txt", filename);
    return tr;
}

  
/*int supprimer_cours(char *filename, int ID)
{
    int tr = 0;
    Cours_admin c;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("nouv.txt", "w");
   
    if(f != NULL && f2 != NULL)
    {
        while(fscanf(f,"%d %s %s %s %s %d %d %d %f %d \n",
                &c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                &c.duree_min,
                &c.capacite_max,
                &c.heure,
                &c.prix,
                &c.salle) != EOF)
        {
            if(c.ID==ID)
            {
                tr = 1;
            }
            else
            {
                fprintf(f2, "%d %s %s %s %s %d %d %d %f %d \n",
                c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                c.duree_min,
                c.capacite_max,
                c.heure,
                c.prix,
                c.salle);
            }
        }
    }
   
    fclose(f);
    fclose(f2);
    remove(filename);
    rename("nouv.txt", filename);
    return tr;
}*/
Cours_admin chercher(char * filename, int ID)
{
    Cours_admin c;
    int tr=0;
    FILE * f=fopen(filename, "r");
    if(f!=NULL)
    {
        while(tr==0 && fscanf(f, "%d %s %s %s %s %d %d %d %f %d \n",
                &c.ID,
                c.Nom_du_cour,
                c.jour,
                c.niveau,
                c.centre,
                &c.duree_min,
               & c.capacite_max,
                &c.heure,
               & c.prix,
               & c.salle) != EOF)
        {
            if(c.ID==ID)
                tr=1;
        }


    }return c;
    fclose(f);
    //if(tr==0)
      //  c.ID=-1;
    //return c;

}
enum
{
    COLUMN_ID,
    COLUMN_NOM_COUR,
    COLUMN_JOUR,
    COLUMN_NIVEAU,              
    COLUMN_CENTRE,
    COLUMN_DUREE_MIN,
    COLUMN_CAPACITE_MAX,
    COLUMN_HEURE,
    COLUMN_PRIX,
    COLUMN_SALLE,
    NUM_COLUMNS
};

void creer_colonnes(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", COLUMN_ID, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom cours", renderer, "text", COLUMN_NOM_COUR, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Jour", renderer, "text", COLUMN_JOUR, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Niveau", renderer, "text", COLUMN_NIVEAU, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", COLUMN_CENTRE, NULL);
    gtk_tree_view_append_column(treeview, column);
   
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Durée (min)", renderer, "text", COLUMN_DUREE_MIN, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Capacité max", renderer, "text", COLUMN_CAPACITE_MAX, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Heure", renderer, "text", COLUMN_HEURE, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prix", renderer, "text", COLUMN_PRIX, NULL);
    gtk_tree_view_append_column(treeview, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Salle", renderer, "text", COLUMN_SALLE, NULL);
    gtk_tree_view_append_column(treeview, column);
}

void afficher_cour(GtkWidget *liste)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GtkTreeView *treeview = GTK_TREE_VIEW(liste);

    g_print("=== Début afficher_cour ===\n");

    // Vérifier si les colonnes existent déjà
    if (g_list_length(gtk_tree_view_get_columns(treeview)) == 0)
    {
        g_print("Création des colonnes...\n");
        creer_colonnes(treeview);
    }
    else
    {
        g_print("Colonnes déjà créées: %d\n", g_list_length(gtk_tree_view_get_columns(treeview)));
    }

    // Créer le store
    store = gtk_list_store_new(NUM_COLUMNS,
                               G_TYPE_INT,    // ID
                               G_TYPE_STRING, // Nom_cour
                               G_TYPE_STRING, // jour
                               G_TYPE_STRING, // niveau
                               G_TYPE_STRING, // centre
                               G_TYPE_INT,    // duree_min
                               G_TYPE_INT,    // capacite_max
                               G_TYPE_INT,    // heure
                               G_TYPE_FLOAT,  // prix
                               G_TYPE_INT);   // salle

    // Variables pour lire le fichier
    int id_temp, duree_temp, capacite_temp, heure_temp, salle_temp;
    float prix_temp;
    char nom_temp[32], jour_temp[32], niveau_temp[32], centre_temp[32];
   
    FILE *f = fopen("cours.txt", "r");
    if (!f)
    {
        g_print("ERREUR: impossible d'ouvrir cours.txt\n");
        g_print("Vérifiez que le fichier existe dans: %s\n", g_get_current_dir());
        gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));
        g_object_unref(store);
        return;
    }

    g_print("Fichier cours.txt ouvert avec succès\n");
    int count = 0;

    // Lecture et insertion dans le store
    while (fscanf(f, "%d %31s %31s %31s %31s %d %d %d %f %d",
                  &id_temp, nom_temp, jour_temp, niveau_temp, centre_temp,
                  &duree_temp, &capacite_temp, &heure_temp, &prix_temp, &salle_temp) == 10)
    {
        count++;
        g_print("Ligne %d: ID=%d, Nom=%s, Jour=%s\n", count, id_temp, nom_temp, jour_temp);
       
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COLUMN_ID, id_temp,
                           COLUMN_NOM_COUR, nom_temp,
                           COLUMN_JOUR, jour_temp,
                           COLUMN_NIVEAU, niveau_temp,
                           COLUMN_CENTRE, centre_temp,
                           COLUMN_DUREE_MIN, duree_temp,
                           COLUMN_CAPACITE_MAX, capacite_temp,
                           COLUMN_HEURE, heure_temp,
                           COLUMN_PRIX, prix_temp,
                           COLUMN_SALLE, salle_temp,
                           -1);
    }

    fclose(f);
    g_print("Total de cours lus: %d\n", count);

    // Attacher le modèle au TreeView
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));

    // Libérer le store (le TreeView garde une référence)
    g_object_unref(store);
   
    g_print("=== Fin afficher_cour ===\n");
}

void supprimer_cour(Cours_admin c)
{
    Cours_admin temp;
    FILE *f = fopen("cours.txt", "r");
    FILE *g = fopen("tmp.txt", "w");

    if (f == NULL || g == NULL)
    {
        if (f) fclose(f);
        if (g) fclose(g);
        return;
    }

    while (fscanf(f, "%d %31s %31s %31s %31s %d %d %d %f %d",
                &temp.ID,
                temp.Nom_du_cour,
                temp.jour,
                temp.niveau,
                temp.centre,
                &temp.duree_min,
                &temp.capacite_max,
                &temp.heure,
                &temp.prix,
                &temp.salle) == 10)
    {
        if (temp.ID != c.ID)
        {
            fprintf(g, "%d %s %s %s %s %d %d %d %f %d\n",
                temp.ID,
                temp.Nom_du_cour,
                temp.jour,
                temp.niveau,
                temp.centre,
                temp.duree_min,
                temp.capacite_max,
                temp.heure,
                temp.prix,
                temp.salle);
        }
    }

    fclose(f);
    fclose(g);

    remove("cours.txt");
    rename("tmp.txt", "cours.txt");
}

void vider(GtkWidget *liste)
{
    GtkTreeModel *model;
    GtkListStore *store;
   
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(liste));
   
    if (model != NULL)
    {
        store = GTK_LIST_STORE(model);
        gtk_list_store_clear(store);
    }
}
