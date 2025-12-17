#include "entraineur.h"
#include <string.h>
#include <stdio.h>



int ajouter(char *filename, entraineur e)
{
    FILE *f = fopen(filename, "a");
    if (f != NULL)
    {
         fprintf(f, "%d %s %s %s %s %s %s %s %s %s\n",
    e.id_entraineur,
    e.prenom,
    e.nom,
    e.sexe,
    e.telephone,
    e.adresse,
    e.email,
    e.specialite,
    e.mode_entrainement,
    e.username);

        fclose(f);
        return 1;
    }
    else
        return 0;
}


int modifier(char *filename, int id_entraineur, entraineur nouv)
{
    
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("nouv.txt", "w");
    int tr = 0;
    entraineur e;

    if (f != NULL && f2 != NULL)
    {
        while (fscanf(f, "%d %s %s %s %s %s %s %s %s %s",
		&e.id_entraineur,
		e.prenom,
		e.nom,
		e.sexe,
		e.telephone,
		e.adresse,
		e.email,
		e.specialite,
		e.mode_entrainement,
		e.username) ==10)
        {
            if (e.id_entraineur == id_entraineur)
            {
                fprintf(f2, "%d %s %s %s %s %s %s %s %s %s\n", 
			nouv.id_entraineur,
			nouv.prenom,
			nouv.nom,
			nouv.sexe,
			nouv.telephone,
			nouv.adresse,
			nouv.email,
			nouv.specialite, 
			nouv.mode_entrainement,
			nouv.username);
                tr = 1;
            }
            else
                fprintf(f2, "%d %s %s %s %s %s %s %s %s %s\n",
			e.id_entraineur,
    			e.prenom,
    			e.nom,
    			e.sexe,
    			e.telephone,
    			e.adresse,
    			e.email,
    			e.specialite,
    			e.mode_entrainement,
    			e.username);
  	 }
    }
    fclose(f);
    fclose(f2);
    remove(filename);
    rename("nouv.txt", filename);
    return tr;
}


int supprimer(char *filename, int id_entraineur)
{
    int tr = 0;
    entraineur e;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("nouv.txt", "w");

    if (f != NULL && f2 != NULL)
    {
        while (fscanf(f, "%d %s %s %s %s %s %s %s %s %s",
    		&e.id_entraineur,
    		e.prenom,
    		e.nom,
    		e.sexe,
    		e.telephone,
    		e.adresse,
    		e.email,
    		e.specialite,
    		e.mode_entrainement,
    		e.username) ==10)
        {
            if (e.id_entraineur == id_entraineur)
                tr = 1;
            else
                fprintf(f2, "%d %s %s %s %s %s %s %s %s %s\n",
    			e.id_entraineur,
    			e.prenom,
    			e.nom,
    			e.sexe,
    			e.telephone,
    			e.adresse,
    			e.email,
    			e.specialite,
    			e.mode_entrainement,
    			e.username);
        }
    }

    fclose(f);
    fclose(f2);
    remove(filename);
    rename("nouv.txt", filename);
    return tr;
}



entraineur chercher(char *filename, int id_entraineur)
{
    entraineur e;
    int tr = 0;
    FILE *f = fopen(filename, "r");

    if (f != NULL)
    {
        while (tr == 0 && fscanf(f, "%d %s %s %s %s %s %s %s %s %s",
    		&e.id_entraineur,
    		e.prenom,
    		e.nom,
    		e.sexe,
    		e.telephone,
    		e.adresse,
    		e.email,
    		e.specialite,
    		e.mode_entrainement,
    		e.username) ==10)
        {
            if (e.id_entraineur == id_entraineur)
                tr = 1;
        }
    
    fclose(f);
    }

    if (tr == 0)
        e.id_entraineur=-1;
    return e;
}


int inscrire(char *filename, Inscription ins)
{
     FILE *f = fopen(filename, "a");
    if (f != NULL)
    {
         fprintf(f, "%d %s %s %s %s %s %d %d %s %s %02d/%02d/%04d\n",
        ins.id_entraineur,
        ins.nom_cours,
        ins.type_cours,
        ins.niveau,
        ins.jour,
	ins.heure,
        ins.duree,
        ins.capacite,
	ins.centre,
	ins.salle,
        ins.date_inscription.jour_insc,
        ins.date_inscription.mois_insc,
        ins.date_inscription.annee_insc);

        fclose(f);
        return 1;
    }
    else
        return 0;
}


   


void afficher(char *filename, GtkWidget *liste)
{

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    GtkTreeIter iter;

    int id;
    char prenom[30], nom[30], sexe[10], tel[20], adrs[40];
    char email[30], spec[30], mode[30], user[20];

    /* Récupérer le modèle actuel */
    store = gtk_tree_view_get_model(GTK_TREE_VIEW(liste));

    /* Si le treeview n'est pas encore initialisé → créer les colonnes */
    if (store == NULL)
    {
        renderer = gtk_cell_renderer_text_new();

        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "ID", renderer, "text", COL_ID, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Prénom", renderer, "text", COL_PRENOM, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Nom", renderer, "text", COL_NOM, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Sexe", renderer, "text", COL_SEXE, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Téléphone", renderer, "text", COL_TEL, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Adresse", renderer, "text", COL_ADRS, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Email", renderer, "text", COL_EMAIL, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Spécialité", renderer, "text", COL_SPEC, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Mode", renderer, "text", COL_MODE, NULL);
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(liste), -1, "Username", renderer, "text", COL_USER, NULL);
    }

    /* Nouveau modèle */
    store = gtk_list_store_new(COLS,
                               G_TYPE_INT,     // COL_ID
                               G_TYPE_STRING,  // COL_PRENOM
                               G_TYPE_STRING,  // COL_NOM
                               G_TYPE_STRING,  // COL_SEXE
                               G_TYPE_STRING,  // COL_TEL
                               G_TYPE_STRING,  // COL_ADRS
                               G_TYPE_STRING,  // COL_EMAIL
                               G_TYPE_STRING,  // COL_SPEC
                               G_TYPE_STRING,  // COL_MODE
                               G_TYPE_STRING); // COL_USER

    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(liste), GTK_TREE_MODEL(store));
        return;
    }

    /* Lire ligne par ligne */
    while (fscanf(f, "%d %s %s %s %s %s %s %s %s %s",
                  &id, prenom, nom, sexe, tel, adrs,
                  email, spec, mode, user) == 10)
    {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_ID, id,
                           COL_PRENOM, prenom,
                           COL_NOM, nom,
                           COL_SEXE, sexe,
                           COL_TEL, tel,
                           COL_ADRS, adrs,
                           COL_EMAIL, email,
                           COL_SPEC, spec,
                           COL_MODE, mode,
                           COL_USER, user,
                           -1);
    }

    fclose(f);

    /* Associer le modèle au TreeView */
    gtk_tree_view_set_model(GTK_TREE_VIEW(liste), GTK_TREE_MODEL(store));

    /* Libérer la mémoire */
    g_object_unref(store);

}



int id_existe(int id)
{
    FILE *f = fopen("entraineur.txt", "r");
    int id_fichier;

    if(!f) return 0;

    while(fscanf(f, "%d", &id_fichier) != EOF){
        if(id == id_fichier){
            fclose(f);
            return 1; // ID existe
        }
        fscanf(f, "%*[^\n]\n");
    }

    fclose(f);
    return 0;
}

int generer_id_auto(void)
{
    FILE *f = fopen("entraineur.txt", "r");
    int max = 0, id;

    if(f){
        while(fscanf(f, "%d", &id) != EOF){
            if(id > max) max = id;
            fscanf(f, "%*[^\n]\n");
        }
        fclose(f);
    }
    return max + 1;
}



