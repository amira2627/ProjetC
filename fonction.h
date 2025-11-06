#ifndef fonction
#define fonction

#include <stdio.h>
#include <string.h>

typedef struct {
    char type[50];                   // Pilates, Yoga, Dance, Musculation, Boxe, Karate, Cardio
    char description[100];           // Description du cours
    int niveau;                      // 0=Débutant, 1=Intermédiaire, 2=Avancé
    char entraineur[50];             // Nom de l'entraineur
    char jour[20];                   // Jour du cours (Mardi, Jeudi, etc.)
    char horaire[10];                // Horaire du cours (9h30, 19h15, etc.)
    int duree;                       // Durée en minutes
    char salle[20];                  // Nom de la salle
    int type_abonnement;             // 0=Mensuel, 1=Annuel, 2=Payer par séance
    int renouveler_abonnement;       // 0=Non, 1=Oui
} Cours;typedef struct {

    
} Cours_admin;



int ajouter_cours(char *, Cours_admin );
int modifier_cours(char *, int , Cours_admin);
int supprimer_cours(char *, int);
Cours chercher_cours(char *, int);

// Prototypes des fonctions d'interface
int sinscrire(char *, Cours);
int annuler_inscription(char *, int );



#endif 
