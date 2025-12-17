#ifndef Profil_entraineur_H
#define Profil_entraineur_H
#include <stdio.h>
#include <gtk/gtk.h>
typedef struct {
	int id_entraineur;
	char prenom[40];
	char nom[40];
	char sexe[20];
	char telephone[20];
	char adresse[50];
	char email[50];
	char specialite[40];
	char mode_entrainement[20];
	char username[40];
}entraineur;
typedef struct {
	int jour_insc;
	int mois_insc;
	int annee_insc;
} Date;

typedef struct {
        int id_entraineur;
        char nom_cours[50];
        char type_cours[30];
        char niveau[20];
        char jour[20];
	char heure[20];
        int duree;
        int capacite;
        char centre[50];
        char salle[20];
	Date date_inscription;
	int accepte_condition;
} Inscription;

enum
{
    COL_ID,
    COL_PRENOM,
    COL_NOM,
    COL_SEXE,
    COL_TEL,
    COL_ADRS,
    COL_EMAIL,
    COL_SPEC,
    COL_MODE,
    COL_USER,
    COLS
};


int ajouter(char *,entraineur);
int modifier(char *,int,entraineur);
int supprimer(char *,int);
entraineur chercher(char *,int);
int inscrire(char *,Inscription);
void afficher(char *filename, GtkWidget *liste);

int id_existe(int id);
int generer_id_auto(void);

#endif




