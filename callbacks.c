#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "entraineur.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"



int valider_entraineur(char *nom, char *prenom, char *tel)
{
    if(strlen(nom)==0 || strlen(prenom)==0)
        return 0;


// Vérification longueur du téléphone
    if(strlen(tel) != 8)
        return 0;

    // Vérification que tous les caractères sont des chiffres
    for(int i = 0; i < 8; i++){
        if(tel[i] < '0' || tel[i] > '9')
            return 0;
    }

    return 1; // Tout est correct

}




void
on_ntbuttonajou_clicked                (GtkButton       *ntbuttonajou,
                                        gpointer         user_data)
{
entraineur e;

	GtkWidget *entry_nom;
	GtkWidget *entry_prenom;
	GtkWidget *entry_email;
	GtkWidget *entry_tel;
	GtkWidget *entry_adresse;
	GtkWidget *combo_specialite;
	GtkWidget *entry_username;
	GtkWidget *entry_id;
	GtkWidget *radio_femme;
	GtkWidget *radio_homme;
	GtkWidget *radio_individuel;
	GtkWidget *radio_groupe;
	GtkWidget *label_result;



	entry_nom = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entrynom");
	entry_prenom = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entryprenom");
	entry_email = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entryemail");
	entry_tel = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entrytel");
	entry_adresse = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entryadresse");
	combo_specialite = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_combospecialite");
	entry_username = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entryuser");
	entry_id = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_entryid");
	radio_femme = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_radiofemme");
	radio_homme = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_radiohomme");
	radio_individuel = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_radioindiv");
	radio_groupe = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_radiogroupe");
	label_result = lookup_widget(GTK_WIDGET(ntbuttonajou), "nt_labelresult");


	// 🔹 Récupération du texte
   	const char *nom = gtk_entry_get_text(GTK_ENTRY(entry_nom));
    	const char *prenom = gtk_entry_get_text(GTK_ENTRY(entry_prenom));
    	const char *tel = gtk_entry_get_text(GTK_ENTRY(entry_tel));
	const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));

    	// 🔹 Validation des champs
    	if(!valider_entraineur((char*)nom, (char*)prenom, (char*)tel)){
        	gtk_label_set_text(GTK_LABEL(label_result), "Erreur : données invalides ! Nom/Prénom manquant ou téléphone incorrect.!");
        	return;
    	}



	e.id_entraineur = atoi( gtk_entry_get_text(GTK_ENTRY(entry_id)) );
	strcpy(e.nom, gtk_entry_get_text(GTK_ENTRY(entry_nom)));
	strcpy(e.prenom, gtk_entry_get_text(GTK_ENTRY(entry_prenom)));
	strcpy(e.email, gtk_entry_get_text(GTK_ENTRY(entry_email)));
	strcpy(e.telephone, gtk_entry_get_text(GTK_ENTRY(entry_tel)));
	strcpy(e.adresse, gtk_entry_get_text(GTK_ENTRY(entry_adresse)));

	 if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_femme)))
		strcpy(e.sexe, "Femme");
	else
		strcpy(e.sexe, "Homme");
	gchar *text= gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_specialite));
	if (text != NULL)
	{
		strcpy(e.specialite, text);
		g_free(text);
	}
	else
	{
		strcpy(e.specialite, "");
	}
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_individuel)))
		strcpy(e.mode_entrainement, "Individuel");
	else
		strcpy(e.mode_entrainement, "Groupe");
	
	strcpy(e.username, gtk_entry_get_text(GTK_ENTRY(entry_username)));


	int id;

	if(strlen(id_text) == 0) {
    	// Champ vide → générer automatiquement
    	do {
        	id = generer_id_auto();
    	} while(id_existe(id));
	} else {
    	// Champ rempli → vérifier doublon
    	id = atoi(id_text);
    	if(id_existe(id)) {
        	gtk_label_set_text(GTK_LABEL(label_result),
            	"Erreur : ID existe déjà ! Donnez un autre ID.");
        	return;
    		}
	}

	e.id_entraineur = id;

	// Affichage dans le GtkEntry
	char buffer[10];
	sprintf(buffer, "%d", id);
	gtk_entry_set_text(GTK_ENTRY(entry_id), buffer);


	int x = ajouter("entraineur.txt", e);

	if (x == 1)
		gtk_label_set_text(GTK_LABEL(label_result), "Ajout réussi !");
	else
		gtk_label_set_text(GTK_LABEL(label_result), "Erreur d'ajout !");

		
}


void
on_nt_buttonajouter_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{


    GtkWidget *affichage_treeview;   // Fenêtre actuelle (affichage TreeView)
    GtkWidget *ajout_entraineur;    // Fenêtre d'ajout

    // On récupère la fenêtre actuelle (celle du TreeView)
    affichage_treeview = lookup_widget(GTK_WIDGET(button), "affichage_treeview");

    // On récupère la fenêtre d'ajout
    ajout_entraineur = create_ajout_entraineur();  // fonction générée par Glade ou que tu as écrite

    // On affiche la fenêtre d'ajout
    gtk_widget_show(ajout_entraineur);

    // Optionnel : cacher la fenêtre actuelle (ou la détruire si tu préfères)
    gtk_widget_hide(affichage_treeview);

}


void
on_nt_buttonretour_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *affichage_treeview;
    GtkWidget *treeview2;
    
    int x = 1;     // à modifier si nécessaire. Au minimum, il doit exister.

    if (x == 1)   // condition correcte
    {
        // Créer la fenêtre d'affichage
        affichage_treeview = create_affichage_treeview();
        gtk_widget_show(affichage_treeview);

        // Récupérer le treeview dans cette fenêtre
        treeview2 = lookup_widget(affichage_treeview, "treeview2");

        // Charger le fichier dans le treeview
        afficher("entreneur.txt", treeview2);
    }

}


void
on_treeview2_row_activated             (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
    /*GtkTreeIter iter;
    GtkTreeModel *model;
    int id;

    model = gtk_tree_view_get_model(treeview);

    if (gtk_tree_model_get_iter(model, &iter, path))
    {
        gtk_tree_model_get(model, &iter, COL_ID, &id, -1); // lecture ID
        supprimer("entraineur.txt", id); // suppression
        afficher("entraineur.txt", GTK_WIDGET(treeview)); // actualiser
    }*/
}


void
on_nt_buttonmodifier_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
entraineur e;

	GtkWidget *nom;
	GtkWidget *prenom;
	GtkWidget *email;
	GtkWidget *tel;
	GtkWidget *adresse;
	GtkWidget *specialite;
	GtkWidget *username;
	GtkWidget *id;
	GtkWidget *sexe_f;
	GtkWidget *sexe_h;
	GtkWidget *mode_indiv;
	GtkWidget *mode_groupe;
	GtkWidget *label_msg;

    id = lookup_widget(GTK_WIDGET(button), "nt_entryidmod");
    prenom = lookup_widget(GTK_WIDGET(button), "nt_entryprenommod");
    nom = lookup_widget(GTK_WIDGET(button), "nt_entrynommod");
    email = lookup_widget(GTK_WIDGET(button), "nt_entryemailmod");
    tel = lookup_widget(GTK_WIDGET(button),  "nt_entrytelmod");
    adresse = lookup_widget(GTK_WIDGET(button), "nt_entryadressemod");
    specialite = lookup_widget(GTK_WIDGET(button), "nt_combospecmod");
    username = lookup_widget(GTK_WIDGET(button), "nt_entryusermod");

    sexe_h = lookup_widget(GTK_WIDGET(button), "nt_radiohommomod");
    sexe_f = lookup_widget(GTK_WIDGET(button), "nt_radiofemmemod");
    mode_indiv = lookup_widget(GTK_WIDGET(button), "nt_radioindivmod");
    mode_groupe= lookup_widget(GTK_WIDGET(button), "nt_radiogroupemod");
    label_msg = lookup_widget(GTK_WIDGET(button), "nt_labelmsg");

     	e.id_entraineur = atoi( gtk_entry_get_text(GTK_ENTRY(id)) );
	strcpy(e.nom, gtk_entry_get_text(GTK_ENTRY(nom)));
	strcpy(e.prenom, gtk_entry_get_text(GTK_ENTRY(prenom)));
	strcpy(e.email, gtk_entry_get_text(GTK_ENTRY(email)));
	strcpy(e.telephone, gtk_entry_get_text(GTK_ENTRY(tel)));
	strcpy(e.adresse, gtk_entry_get_text(GTK_ENTRY(adresse)));

	 if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sexe_f)))
		strcpy(e.sexe, "Femme");
	else
		strcpy(e.sexe, "Homme");
	gchar *text= gtk_combo_box_get_active_text(GTK_COMBO_BOX(specialite));
	if (text != NULL)
	{
		strcpy(e.specialite, text);
		g_free(text);
	}
	else
	{
		strcpy(e.specialite, "");
	}
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mode_indiv)))
		strcpy(e.mode_entrainement, "Individuel");
	else
		strcpy(e.mode_entrainement, "Groupe");
	
	strcpy(e.username, gtk_entry_get_text(GTK_ENTRY(username)));

	int x = modifier("entraineur.txt", e.id_entraineur, e);

	if (x == 1)
		gtk_label_set_text(GTK_LABEL(label_msg), "modification réussit !");
	else
		gtk_label_set_text(GTK_LABEL(label_msg), "Erreur de modification !");


}



void set_combobox_active_text(GtkComboBox *combo, const char *text)
{
    GtkTreeModel *model = gtk_combo_box_get_model(combo);
    GtkTreeIter iter;
    gboolean valid;
    gchar *value;
    int index = 0;

    valid = gtk_tree_model_get_iter_first(model, &iter);

    while (valid)
    {
        gtk_tree_model_get(model, &iter, 0, &value, -1);

        if (strcmp(value, text) == 0)
        {
            gtk_combo_box_set_active(combo, index);
            return;
        }

        index++;
        valid = gtk_tree_model_iter_next(model, &iter);
    }
}




void
on_ntbuttonmodifier_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
/*GtkWidget *modifier_entraineur;
GtkWidget *affichage_treeview;


affichage_treeview=lookup_widget (GTK_WIDGET(button), "affichage_treeview");
modifier_entraineur=create_modifier_entraineur();

gtk_widget_show(modifier_entraineur);

gtk_widget_hide(affichage_treeview);*/


GtkWidget *treeview = lookup_widget(GTK_WIDGET(button), "treeview2");

    if (treeview == NULL)
    {
        g_print("ERREUR : treeview2 introuvable.\n");
        return;
    }

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (!gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        g_print("Veuillez sélectionner un entraîneur à modifier.\n");
        return;
    }

    // Déclaration des variables
    int id;
    gchar *prenom, *nom, *sexe, *tel, *adrs, *email, *spec, *mode, *user;

    // Lire la ligne sélectionnée
    gtk_tree_model_get(model, &iter,
                       COL_ID, &id,
                       COL_PRENOM, &prenom,
                       COL_NOM, &nom,
                       COL_SEXE, &sexe,
                       COL_TEL, &tel,
                       COL_ADRS, &adrs,
                       COL_EMAIL, &email,
                       COL_SPEC, &spec,
                       COL_MODE, &mode,
                       COL_USER, &user,
                       -1);

    // Ouvrir la fenêtre de modification
    GtkWidget *fenetre_modif = create_modifier_entraineur();
    gtk_widget_show(fenetre_modif);

    // Remplir les champs
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entryidmod")), g_strdup_printf("%d", id));
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entryprenommod")), prenom);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entrynommod")), nom);
    /*gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entrysexemod")), sexe);*/
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entrytelmod")), tel);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entryadressemod")), adrs);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entryemailmod")), email);
    /*gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entryspecmod")), spec);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entrymodemod")), mode);*/
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(fenetre_modif, "nt_entryusermod")), user);

    // 🔵 RadioButton Sexe
        if (strcmp(sexe, "Homme") == 0)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(fenetre_modif, "nt_radiohommemod")), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(fenetre_modif, "nt_radiofemmemod")), TRUE);

        // 🔴 RadioButton Mode
        if (strcmp(mode, "Individuel") == 0)
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(fenetre_modif, "nt_radioindivmod")), TRUE);
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(fenetre_modif, "nt_radiogroupemod")), TRUE);

     //  ComboBox Spécialité
        GtkWidget *combo = lookup_widget(fenetre_modif, "nt_combospecmod");
        set_combobox_active_text(GTK_COMBO_BOX(combo), spec);

    // Libération mémoire
    g_free(prenom);
    g_free(nom);
    g_free(sexe);
    g_free(tel);
    g_free(adrs);
    g_free(email);
    g_free(spec);
    g_free(mode);
    g_free(user);

}


void
on_nt_buttonafficher_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    
    GtkWidget *treeview;

    // Récupération du widget TreeView dans la même fenêtre
    treeview= lookup_widget(button, "treeview2");

    // Appel à la fonction qui remplit le TreeView
    afficher("entraineur.txt", treeview);
}


void
on_nt_buttonrechercher_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry_id = lookup_widget(GTK_WIDGET(button), "nt_entryrechercher");
    GtkWidget *treeview = lookup_widget(GTK_WIDGET(button), "treeview2");

    // Vérification du TreeView
    if (treeview == NULL)
    {
        g_print("ERREUR : 'treeview2' introuvable dans l'interface !\n");
        return;
    }

    // Lire ID saisi
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (id_text == NULL || strlen(id_text) == 0)
    {
        g_print("Veuillez entrer un ID.\n");
        return;
    }

    int id_recherche = atoi(id_text);

    // Récupérer le modèle
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    if (model == NULL)
    {
        g_print("ERREUR : Le TreeView est vide. Appelez afficher() avant de rechercher.\n");
        return;
    }

    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
    gboolean trouve = FALSE;
    int id_ligne;

    // Parcourir les lignes
    while (valid)
    {
        gtk_tree_model_get(model, &iter, 0, &id_ligne, -1); // Colonne 0 = ID (int)

        if (id_ligne == id_recherche)
        {
            GtkTreeSelection *selection =
                gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

            gtk_tree_selection_select_iter(selection, &iter);

            g_print("ID %d trouvé et sélectionné !\n", id_recherche);

            trouve = TRUE;
            break;
        }

        valid = gtk_tree_model_iter_next(model, &iter);
    }

    if (!trouve)
        g_print("ID %d non trouvé dans le TreeView.\n", id_recherche);
}


void
on_nt_buttonsupprimer_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{

    GtkWidget *treeview;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    int id_a_supprimer;

    /* Récupérer le TreeView principal */
    treeview = lookup_widget(GTK_WIDGET(button), "treeview2");
    if (!treeview) {
        g_print("Erreur : TreeView introuvable !\n");
        return;
    }

    /* Récupérer la ligne sélectionnée */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        g_print("Aucune ligne sélectionnée.\n");
        return;
    }

    /* Récupérer l'ID de la ligne sélectionnée (colonne 0) */
    gtk_tree_model_get(model, &iter, 0, &id_a_supprimer, -1);

    /* Ouvrir la fenêtre de confirmation */
    GtkWidget *fenetre_confirm = create_supprimer_entraineur(); // Fenêtre Glade avec Oui/Non

    /* Stocker l'ID et le TreeView dans la fenêtre de confirmation */
    g_object_set_data(G_OBJECT(fenetre_confirm), "id_a_supprimer", GINT_TO_POINTER(id_a_supprimer));
    g_object_set_data(G_OBJECT(fenetre_confirm), "treeview_principal", treeview);

    gtk_widget_show(fenetre_confirm);


}


void
on_nt_buttonoui_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *fenetre_confirm = gtk_widget_get_toplevel(GTK_WIDGET(button));

    /* Récupérer l'ID et le TreeView principal stockés dans la fenêtre de confirmation */
    int id_a_supprimer = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(fenetre_confirm), "id_a_supprimer"));
    GtkWidget *treeview = g_object_get_data(G_OBJECT(fenetre_confirm), "treeview_principal");

    /* Supprimer l'enregistrement dans le fichier */
    supprimer("entraineur.txt", id_a_supprimer);

    /* Fermer la fenêtre de confirmation */
    gtk_widget_destroy(fenetre_confirm);

    /* Rafraîchir le TreeView principal */
    if (treeview)
        afficher("entraineur.txt", treeview);
}


void
on_nt_buttonnon_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *fenetre_confirm = gtk_widget_get_toplevel(GTK_WIDGET(button));
    gtk_widget_destroy(fenetre_confirm);
}


void
on_buttoninscrire_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonannuler_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_nt_button_inscrire_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
Inscription ins;

    // ------------------ Widgets ------------------
    GtkWidget *entry_id          = lookup_widget(button, "nt_entry_id");
    GtkWidget *entry_nom         = lookup_widget(button, "nt_entry_nomcours");

    GtkWidget *radio_privee      = lookup_widget(button, "nt_radio_privee");
    GtkWidget *radio_collectif   = lookup_widget(button, "nt_radio_collectif");

    GtkWidget *combo_niveau      = lookup_widget(button, "nt_combo_niveau");
    GtkWidget *combo_jour        = lookup_widget(button, "nt_combo_jour");
    GtkWidget *combo_heure       = lookup_widget(button, "nt_combo_heure");
    GtkWidget *combo_centre      = lookup_widget(button, "nt_combo_centre");

    GtkWidget *spin_duree        = lookup_widget(button, "nt_spin_duree");
    GtkWidget *spin_capacite     = lookup_widget(button, "nt_spin_capacite");

    // ---- SALLE devient un ENTRY (corrigé) ----
    GtkWidget *entry_salle       = lookup_widget(button, "nt_entry_salle");

    GtkWidget *spin_jour_insc    = lookup_widget(button, "nt_spin_jour_insc");
    GtkWidget *spin_mois_insc    = lookup_widget(button, "nt_spin_mois_insc");
    GtkWidget *spin_annee_insc   = lookup_widget(button, "nt_spin_annee_insc");

    GtkWidget *check_conditions  = lookup_widget(button, "nt_check_conditions");

    // Label d'affichage résultat
    GtkWidget *label_resultat    = lookup_widget(button, "nt_label_resultat");

    // ------------------ Remplir la structure ------------------

    ins.id_entraineur = atoi(gtk_entry_get_text(GTK_ENTRY(entry_id)));
    strcpy(ins.nom_cours, gtk_entry_get_text(GTK_ENTRY(entry_nom)));

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_privee)))
        strcpy(ins.type_cours, "Privee");
    else
        strcpy(ins.type_cours, "Collectif");

    // Niveau : combo box
    gchar *text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_niveau));
	if (text1 != NULL)
	{
    		strcpy(ins.niveau, text1);
    		g_free(text1);
	}
	else
	{
    		strcpy(ins.niveau, "");
	}

    // Jour : combo box
    gchar *text2 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_jour));
	if (text2 != NULL)
	{
    		strcpy(ins.jour, text2);
    		g_free(text2);
	}
	else
	{
    		strcpy(ins.jour, "");
	}


    // Heure : combo box
    gchar *text3 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_heure));
	if (text3 != NULL)
	{
    		strcpy(ins.heure, text3);
    		g_free(text3);
	}
	else
	{
    		strcpy(ins.heure, "");
	}

    ins.duree = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_duree));
    ins.capacite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_capacite));

    // Centre : combo box
    gchar *text4 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_centre));
	if (text4 != NULL)
	{
    		strcpy(ins.centre, text4);
    		g_free(text4);
	}
	else
	{
    		strcpy(ins.centre, "");
	}

    // ----- Salle maintenant Entry -----
    strcpy(ins.salle, gtk_entry_get_text(GTK_ENTRY(entry_salle)));

    // Date
    ins.date_inscription.jour_insc  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_jour_insc));
    ins.date_inscription.mois_insc  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_mois_insc));
    ins.date_inscription.annee_insc = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_annee_insc));

    ins.accepte_condition = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_conditions));

    // ------------------ Enregistrer ------------------
    int ok = inscrire("Inscription.txt", ins);

    // ------------------ Affichage dans le label ------------------
    if (ok == 1)
        gtk_label_set_text(GTK_LABEL(label_resultat), "Inscription réussie ✔️");
    else
        gtk_label_set_text(GTK_LABEL(label_resultat), "Échec de l'inscription ❌");

}


void
on_nt_button_annuler_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *fenetre_inscription;

    // Récupérer la fenêtre parente (celle qui contient le bouton)
    fenetre_inscription = gtk_widget_get_toplevel(button);

    // Fermer la fenêtre
    gtk_widget_destroy(fenetre_inscription);
}



int verifier_entraineur(char *username)
{
    FILE *f;
    char u[50];

    f = fopen("entraineur.txt", "r");
    if (f == NULL)
        return 0;

    while (fscanf(f, "%s", u) != EOF)
    {
        if (strcmp(u, username) == 0)
        {
            fclose(f);
            return 1; // trouvé
        }
    }

    fclose(f);
    return 0; // non trouvé
}


void
on_NT_buttonlogin_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

GtkWidget *entry_user;
    GtkWidget *label_msg;
    GtkWidget *fenetre_auth;
    GtkWidget *fenetre_espace;
    GtkWidget *label_bienvenu;

    char username[50];
    char texte[200];

    // Récupération des widgets
    fenetre_auth = lookup_widget(button, "fenetre_auth");
    entry_user   = lookup_widget(button, "NT_entryusername");
    label_msg    = lookup_widget(button, "NT_labelmsg");

    strcpy(username, gtk_entry_get_text(GTK_ENTRY(entry_user)));

    // Vérification
    if (verifier_entraineur(username))
    {
        // Ouvrir espace entraîneur
        fenetre_espace = create_fenetre_espace_entraineur();
        gtk_widget_show(fenetre_espace);

        // Modifier le label bienvenue
        label_bienvenu = lookup_widget(fenetre_espace, "NT_labelbienvenu");

	sprintf(texte,
        "<span foreground='firebrick' size='16000' weight='bold'>"
        "Bienvenue dans l'espace entraineur\n\tVoulez-vous :"
        "</span>");

	gtk_label_set_use_markup(GTK_LABEL(label_bienvenu), TRUE);
	gtk_label_set_markup(GTK_LABEL(label_bienvenu), texte);


        // Fermer fenêtre auth
        gtk_widget_destroy(fenetre_auth);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(label_msg),
                           "Utilisateur non existant");
    }

}


void
on_NT_buttoninscrire_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *fenetre_inscription_cours;

    fenetre_inscription_cours = create_inscription_cours();
    gtk_widget_show(fenetre_inscription_cours);
}


void
on_NT_buttonreserver_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
/*GtkWidget *fenetre_reservation;

    fenetre_reservation = create_reservation();
    gtk_widget_show(fenetre_reservation);*/
}

