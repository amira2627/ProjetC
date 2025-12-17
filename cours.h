#ifndef cours
#define cours

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int ID;
    char Nom_du_cour[32];
    char jour[32];
    char niveau[32];              
    char centre[32];
    int  duree_min;
    int capacite_max;
    int heure;
    float prix;
    int salle;
    

    
} Cours_admin;



int ajouter_cours(char *filename, Cours_admin c );
int modifier_cours(char *, int , Cours_admin);
//int supprimer_cours(char *, int);
Cours_admin  chercher(char *filename , int);
void afficher_cour(GtkWidget *liste);
void vider(GtkWidget *liste);
void supprimer_cour(Cours_admin c);
// Nouvelles fonctions pour la génération d'ID
int generer_id_unique(char *filename);
int id_existe(char *filename, int id);
#endif 

