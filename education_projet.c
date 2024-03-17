#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#define TAILLE_MAX 100

void connexion();
void menuAdmin();
void envoyerDesMessages();
void mesMessages();
void menuApprenant();
void sousMenuGenerationFichiers();

const char adminpass[] = "venir1";
int choix;
int trouve;
int estVide;
int estVide2;
time_t date_presence;
char empty[50];
char empty2[50];
char email[50];
char motdepasse[50];

typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

typedef struct {
    int jour;
    int mois;
    int annee;
    int heure;
    int minute;
    int seconde;
} DATES;

typedef struct {
    char expediteur[50];
    char contenu[200];
    enum Etat {envoye = 1, echec, enCours} etat;
   time_t date_presence;
} Message;

typedef struct {
    char cordonnes[100];
    int messagesNonLus;
    Message boiteMessages[20];
} Etudiant;

typedef struct {
    int id;
    int code;
    char matricule[10];
    char email[50];
    char motdepasse[50];
    char nom[20];
    char prenom[50];
    Date date_naissance;
    enum etat {BLOQUE, ACTIF} etat;
    enum niveau {ADMIN=1, APPRENANT} niveau;
    enum referentielle {DEV_WEB=1, DEV_DATA, REF_DIG} referentielle;
} Utilisateur;
typedef struct {
    int code;
    time_t date_presence;
    Utilisateur u;
} Presence;

DATES saisirDates(void)
{
       DATES d;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    d.jour = tm_info->tm_mday;
    d.mois = tm_info->tm_mon + 1;  
    d.annee = tm_info->tm_year + 1900;
    d.heure = tm_info->tm_hour;
    d.minute = tm_info->tm_min;
    d.seconde = tm_info->tm_sec;

    return d;
}
void afficheDate()
{
    DATES a;
        printf("\033[0;33m");
    printf("La date et l'heure sont ==> %02d/%02d/%d %02d:%02d:%02d\n", a.jour, a.mois, a.annee, a.heure, a.minute, a.seconde);
        printf("\033[0m");
     saisirDates();
}

int saisirInt(int min, int max, char message[])
{
    int x;
    do{
        puts("message");
        scanf("%d", &x);
    }while(x < min || x > max);
    return x;
}
Date saisirDate() {
    Date date;
        printf("\033[0;32m");
    printf("Saisissez la date au format j/m/a ==> ");
        printf("\033[0m");
    date.jour = saisirInt(1, 31, "Le jour doit être entre 1 et 31 \n");
    date.mois = saisirInt(1, 12, "Le mois doit être entre 1 et 12 \n");
    date.annee = saisirInt(1980, 2024, "L'année doit être entre 1980 et 2024 \n");
    return date;
}
/*void matriculeApprenant(){
    Utilisateur u;
    if(u.id < 10){
        sprintf(u.matricule, "matricule-%c%c-00%d", u.nom[0], u.prenom[0], u.id);
    }else{
        sprintf(u.matricule, "matricule-%c%c-0%d", u.nom[0], u.prenom[0], u.id);
    }
}*/
void informationUtilisateur(Utilisateur u) {
    printf("Id utilisateur : %d\n", u.id);
    printf("Nom utilisateur : %s\n", u.nom);
    printf("Prénom utilisateur : %s\n", u.prenom);
    printf("Email utilisateur : %s\n", u.email);
    printf("Mot de passe utilisateur : %s\n", u.motdepasse);
    printf("Type utilisateur : %d\n", u.niveau);
    printf("Etat utilisateur : %d\n", u.etat);
    if(u.niveau == 2) {
        printf("Matricule etudiant : %s\n", u.matricule);
        printf("code etudiant : %d\n", u.code);
        printf("Referentielle etudiant : %d\n", u.referentielle);
    }
    printf("Date utilisateur : %02d/%02d/%04d\n", u.date_naissance.jour, u.date_naissance.mois, u.date_naissance.annee);
}
int saisirUtilisateurs() {
    int code;
    FILE *fichier = fopen("utilisateurs.txt", "r");
    if (fichier == NULL) {
        printf("\033[0;31m");
        printf("Erreur lors de l'ouverture du fichier utilisateurs.txt.");
        printf("\033[0m");
        return 0;
    }

    Utilisateur u;
    while (fscanf(fichier, "%d %s %s %s %s %d %d %d %d %d", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee,(int *)&u.niveau, &u.code) != EOF) {
        if (u.code == code) {
            fclose(fichier);
            return 1;
        }
    }

    fclose(fichier);
    return 0; 
}
void afficherPresences(int code, Presence* presences, int nombrePresences) {
    int i;
        printf("\033[0;32m");
    printf("Vos présences ==> \n");
        printf("\033[0m");
    for (i = 0; i < nombrePresences; i++) {
        if (presences[i].code == code) {
            printf("%s", ctime(&presences[i].date_presence));
        }
    }
}
void saisirUtilisateur(){
 do {
        printf("\033[0;32m");
        printf("Entrez votre email ==> ");
        printf("\033[0m");
        fgets(email, sizeof(email), stdin);
        strcpy(empty2, email);
        empty2[strcspn(empty2, "\n")] = '\0';
        estVide2=1;
        for (int i=0; i<strlen(empty2); i++) {
            if(!isspace(empty2[i])){
                estVide2 = 0;
                break;
            }
        }
        if(estVide2){
            printf("\033[0;31m");
            printf("L'email ne pas contenir que des espaces \n");
            printf("\033[0m");
        }else{
            break;
        }
    }while (estVide2);
}
void saisieMotDePasseSecret(char *motDePasse, size_t longueurMax) {
    struct termios ancienTermios, nouveauTermios;
    tcgetattr(STDIN_FILENO, &ancienTermios);
    nouveauTermios = ancienTermios;
    nouveauTermios.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &nouveauTermios);

    size_t i = 0;
    int c;
    while (i < longueurMax - 1) {
        c = getchar();
        if (c == '\n' || c == '\r') {
            break;
        } else if ((c == '\b' || c == 127) && i > 0) {
            motDePasse[--i] = '\0'; 
            fputs("\b \b", stdout); 
        } else if (c >= 32 && c < 127) { 
            motDePasse[i++] = (char)c;
            printf("*");
        }
    }
    motDePasse[i] = '\0'; 
    
    putchar('\n'); 
    tcsetattr(STDIN_FILENO, TCSANOW, &ancienTermios);
}
void saisirMotdepasse(){
 do{
        printf("\033[0;32m");
        printf("Entrez votre motdepasse ==> ");
        printf("\033[0m");
        saisieMotDePasseSecret(motdepasse, sizeof(motdepasse));   
        strcpy(empty, motdepasse);
        empty[strcspn(empty, "\n")] = '\0';
        estVide=1;
        for (int i=0; i<strlen(empty); i++) {
            if(!isspace(empty[i])){
                estVide = 0;
                break;
            }
        }
        if(estVide){
            printf("\033[0;31m");
            printf("Le mot de passe ne doit pas contenir des espaces \n");
            printf("\033[0m");
        }else{
            break;
        }
    }while(estVide);
}
void gestionEtudiants() {
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|----------------------Vous avez choisi la gestion des étudiants.---------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    printf("\n");
    FILE *fichier = fopen("utilisateurs.txt", "r");
    
    if (fichier != NULL) {
        Utilisateur u;

        printf("\nDÉTAILS DE TOUS LES ÉTUDIANTS : \n");
        printf("\n");
        printf("\033[0m");

        printf("***********************************************************************************************************\n");
        printf("\033[0;32m");
        printf("\n| %-10s | %-20s | %-21s | %-18s | %-10s | %-10s |\n", "ID", "Nom", "Prénom", "Date Naiss.", "Niveau", "Code");
        printf("\033[0m");
        printf("***********************************************************************************************************\n");

        while (fscanf(fichier, "%d %49s %49s %49s %49s %d %d %d %d %d", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee, (int *)&u.niveau, &u.code) == 10) {
            if (u.niveau == 2) {
                printf("\033[0;32m");
                printf("| %-10d | %-20s | %-20s | %02d/%02d/%-12d | %-10d | %-10d |\n", u.id, u.nom, u.prenom, u.date_naissance.jour, u.date_naissance.mois, u.date_naissance.annee, u.niveau, u.code);
                printf("\033[0m");
                printf("-----------------------------------------------------------------------------------------------------------\n");
            }
        }

        fclose(fichier);
    } else {
        printf("\033[0;31m");
        printf("Erreur lors de l'ouverture du fichier.\n");
        printf("\033[0m");
    }

    printf("\033[0;32m");
    printf("\ntapez sur 'Q' puis sur Enter pour quitter...==> ");
    printf("\033[0m");
    char quitter;
    do {
        scanf(" %c", &quitter); 
    } while (quitter != 'Q' && quitter != 'q');
    menuAdmin();
}
void generationfichiers() {
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|---------------------Vous avez choisi la génération de fichiers.---------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    FILE *fichierPresence = fopen("presences.txt", "r");
    FILE *fichiergenere = fopen("presencesgenere.txt", "a");
    if (fichierPresence != NULL) {
        Presence presences[100]; 
        int nbPresences = 0;
        printf("\nListe des apprenants présents : \n");
        printf("\033[0m");

        printf("*");
        for (int i = 0; i < 77; i++) {
            printf("*");
        }
        printf("*\n");

        printf("\033[0;32m");
        printf("| %-9s | %-19s | %-19s  | %-19s |\n", "Code", "Nom", "Prénom", "Date");
        printf("\033[0m");

        printf("*");
        for (int i = 0; i < 77; i++) {
            printf("*");
        }
        printf("*\n");

        for (int i = 0; i < nbPresences; i++) {
            printf("\033[0;32m");
            printf("| %-9d | %-19s | %-19s | %-19ld |\n", presences[i].code, presences[i].u.nom, presences[i].u.prenom, presences->date_presence);
            printf("\033[0m");

            printf("-------------------------------------------------------------------------------\n");
        }
        fclose(fichierPresence);
        fclose(fichiergenere);
    } else {
        printf("\033[0;31m");
        printf("Erreur lors de la génération du fichier.\n");
        printf("\033[0m");
    }
    printf("\033[0;32m");
    printf("\nAppuyez sur 'Q' puis sur Enter pour quitter...==> ");
    printf("\033[0m");
    char quitter;
    do {
        scanf(" %c", &quitter);
    } while (quitter != 'Q' && quitter != 'q');
        sousMenuGenerationFichiers();
}
void genererFichierApprenantsParDate() {
    Presence presences[100];
    Date d;
    int nbPresences = 0;
    char date[20];
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|----------------Vous avez choisi la génération de fichier par date.------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    printf("Veuillez saisir la date au format YYYY-MM-DD ==> ");
    scanf("%s", date);
    printf("\033[0m");

    FILE *fichierPresence = fopen("presences.txt", "r");
    FILE *presenceParDateFp = fopen("presence_par_date.txt", "w");

    if (fichierPresence != NULL && presenceParDateFp != NULL) {
        Utilisateur u;
        int i;
        printf("\033[0;32m");
        while (fscanf(fichierPresence, "%d %s %s %ld\n", &presences[i].u.code, presences[i].u.nom, presences[i].u.prenom, &presences->date_presence)!= EOF) {
            printf("\nListe des apprenants présents du %d %d %d: \n", d.jour, d.mois, d.annee);
            printf("\033[0m");

            printf("*");
            for (int i = 0; i < 77; i++) {
                printf("*");
            }
            printf("*\n");

        printf("| %-9s | %-19s | %-19s  | %-19s |\n", "Code", "Nom", "Prénom", "Date");

        printf("*");
        for (int i = 0; i < 77; i++) {
            printf("*");
        }
        printf("*\n");

        for (int i = 0; i < nbPresences; i++) {
          printf("| %-9d | %-19s | %-19s | %-19ld |\n", presences[i].code, presences[i].u.nom, presences[i].u.prenom, presences->date_presence);

            printf("-------------------------------------------------------------------------------\n");
        }
        }

        fclose(fichierPresence);
        fclose(presenceParDateFp);
        printf("\033[0;32m");
        printf("Fichier généré avec succès.\n");
        printf("\033[0m");
    } else {
        printf("\033[0;31m");
        printf("Erreur lors de l'ouverture des fichiers.\n");
        printf("\033[0m");
    }
    printf("\033[0;32m");
    printf("\nAppuyez sur 'Q' puis sur Enter pour quitter...==> ");
    printf("\033[0m");
    char quitter;
    do {
        scanf(" %c", &quitter);
    } while (quitter != 'Q' && quitter != 'q');
    sousMenuGenerationFichiers();
}
void sousMenuGenerationFichiers() {
    int choix;
    system("clear");
    do {
        printf("\033[0;33m");
        printf("---------------------------------------------------------------------------------------\n");
        printf("|--------------------------GENERATION DE FICHIERS ETUDIANTS.---------------------------|\n");
        printf("---------------------------------------------------------------------------------------\n");
        printf("\033[0;34m");
        printf("1. Générer le fichier de tous les apprenants\n");
        printf("2. Générer le fichier des apprenants présents par date\n");
        printf("3. Retour\n\n");
        printf("\033[0;32m");
        printf("CHOIX==>   ");
        printf("\033[0m");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                generationfichiers();
                break;
            case 2:
                genererFichierApprenantsParDate();
                break;
            case 3:
                printf("Retour au menu précédent.\n");
                menuAdmin();
                break;
            default:
                printf("\033[0;31m");
                printf("Choix invalide.\n");
                printf("\033[0m");

        }
    } while (choix != 3);
}
void marquerPresences() {
    Utilisateur u;
    char listeApprenants[TAILLE_MAX][100];
    char prenom[30];
    char prenomFichier[20];
    struct tm *temps_info;
    time_t now;
    char line[100];
    char saisirCodes[10];
    int nbrEtudiant = 0; 

    system("clear");
    printf("\033[0;33m");
    printf("----------------------------------------------------------------------------------------\n");
    printf("|-----------------Vous avez choisi de marquer les présences.--------------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0m");

    do{
        printf("\033[0;32m");
        printf("Entrez le code étudiant (Q pour annuler)==> ");
        scanf("%s", saisirCodes);
        printf("\033[0m");

        FILE *fichier = fopen("utilisateurs.txt", "r");
        FILE *fichierPresence = fopen("presences.txt", "a+");
        int codeValid = 0;
        if (fichier == NULL && fichierPresence == NULL){
            printf("\033[0;31m");
            printf("Fichier liste étudiants introuvable...");
            printf("\033[0m");
        }
    
        while (sscanf(line, "%d %s %s %s %s %d %d %d %d %d", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee,(int *)&u.niveau, &u.code) != EOF){
            if (u.code == saisirCodes[nbrEtudiant]) {
                nbrEtudiant += 7;
                break;
            }
        }
        fclose(fichier);

        for (int i = 0; i < nbrEtudiant; i+=7){
            if (u.code == saisirCodes[i]){
                now = time(NULL);
                temps_info = localtime(&now);
                char date[100];

                sprintf(date, "%02d/%02d/%02d", temps_info->tm_mday, temps_info->tm_mon + 1, temps_info->tm_year + 1900);
                rewind(fichierPresence);
                fseek(fichierPresence, 0, SEEK_SET);

                int estEnregistrer = 0;

                while (fgets(line, sizeof(line), fichierPresence) != NULL){
                    char dateFichier[50];
                    sscanf(line, "%d %s %s %s %s %d %d %d %d %d %hhd", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee,(int *)&u.niveau, &u.code, dateFichier);
                    if (strcmp(prenomFichier, u.prenom) == 0 && strcmp(dateFichier, date) == 0)
                    {
                        estEnregistrer = 1;
                        break;
                    }
                }
                if (estEnregistrer){
                    printf("\033[0;32m");
                    printf("L'étudiant %s est déjà enregistré pour aujourd'hui.\n", u.prenom);
                    printf("\033[0m");
                }
 
                int dejaEnregistrer = 0;
                
                while (fgets(line, sizeof(line), fichierPresence) != NULL){
                    char dateFichier[50];
                    sscanf(line, "%d %s %s %s %s %d %d %d %d %d %hhd", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee,(int *)&u.niveau, &u.code, dateFichier);
                    if (u.code == saisirCodes[nbrEtudiant] && strcmp(dateFichier, date) == 0){
                        dejaEnregistrer = 1;
                        break;
                    }
                }

                if(dejaEnregistrer){
                    printf("\033[0;32m");
                    printf("étudiant déja enregistré\n");
                    printf("\033[0m");
                }
                else {
                    FILE *fichierPresence = fopen("presences.txt", "a");
                    printf("Présence enregistrée pour %d (code: %d) à %02d:%02d:%02d\n", prenom[i + 1], saisirCodes[i], temps_info->tm_hour, temps_info->tm_min, temps_info->tm_sec);
                    fprintf(fichierPresence, "%d %s %s %s %s %d %d %d %d %d %02d/%02d/%02d,%02d:%02d\n",u.id, u.email, u.motdepasse, u.nom, u.prenom, u.date_naissance.jour, u.date_naissance.mois, u.date_naissance.annee,u.niveau, u.code, temps_info->tm_mday, temps_info->tm_mon + 1, temps_info->tm_year + 1900, temps_info->tm_hour, temps_info->tm_min);
                    fclose(fichierPresence);
                }
            }
        }if (!codeValid){
            printf("\033[0;31m");
            printf("Code étudiant invalide\n");
            printf("\033[0m");
        }
        char motdepasse[50];
        do {
            printf("Entrez votre motdepasse ==> ");
            saisieMotDePasseSecret(motdepasse, sizeof(motdepasse));
        }while (strcmp(u.motdepasse, motdepasse) != 0);
        system("clear");
    } while (1);
}

void envoyerMessageATous(Etudiant *etudiants, int nombreEtudiants) {
    Utilisateur u;
    char message[200];
     printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-----------------Vous avez choisi envoyez message à tous les apprenants.-------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
     printf("\033[0;32m");
    printf("Entrez le message ==> ");
    fgets(message, sizeof(message), stdin);
    printf("\033[0m");

    FILE *fichier = fopen("utilisateurs.txt", "r");
    FILE *messagesFp1 = fopen("messages.txt", "a");

    for (int i = 0; i < nombreEtudiants; i++) {
        strcpy(etudiants[i].boiteMessages[etudiants[i].messagesNonLus].expediteur, "Administrateur");
        strcpy(etudiants[i].boiteMessages[etudiants[i].messagesNonLus].contenu, message);
        etudiants[i].boiteMessages[etudiants[i].messagesNonLus].etat = envoye;
        etudiants[i].messagesNonLus++;

        fprintf(messagesFp1, "Expéditeur: Administrateur, Destinataire: %s %s, Contenu: %s\n", u.nom, u.prenom, message);
    }

    fclose(fichier);
    fclose(messagesFp1);

    printf("\033[0;32m");
    printf("Message envoyé avec succès à tous les apprenants.\n");
    printf("\033[0m");
    envoyerDesMessages();
}

void envoyerMessageParReferentiel(Etudiant *etudiants, int nombreEtudiants) {
    char message[200];
    Utilisateur u;
    char referentielle[50];
    printf("\033[0;34m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-----------------Vous avez choisi envoyer message par referentielle.-----------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    printf("Saisissez le référentielle (dev web = 1, dev data = 2, ref dig = 3) ==> ");
    fgets(referentielle, sizeof(referentielle), stdin);
    printf("Entrez le message ==> ");
    fgets(message, sizeof(message), stdin);
    printf("\033[0m");

    FILE *fichier = fopen("utilisateurs.txt", "r");
    FILE *messagesFp2 = fopen("messages.txt", "a");

    for (int i = 0; i < nombreEtudiants; i++) {
        if (u.referentielle == referentielle[i]) {
            strcpy(etudiants[i].boiteMessages[etudiants[i].messagesNonLus].expediteur, "Administrateur");
            strcpy(etudiants[i].boiteMessages[etudiants[i].messagesNonLus].contenu, message);
            etudiants[i].boiteMessages[etudiants[i].messagesNonLus].etat = envoye;
            etudiants[i].messagesNonLus++;

            fprintf(messagesFp2, "Expéditeur: Administrateur, Destinataire: %s %s, Contenu: %s\n", u.nom, u.prenom, message);
        }
    }

    fclose(fichier);
    fclose(messagesFp2);

    printf("\033[0;32m");
    printf("Message envoyé avec succès au référentiel %s.\n", referentielle);
    printf("\033[0m");
    envoyerDesMessages();
}

void envoyerMessageAuxSelectionnes(Etudiant *etudiants, int nombreEtudiants) {
    Utilisateur u;
    int nombreCodes;
    int codes[nombreCodes];
    char message[200];
    printf("\033[0;34m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|---------Vous avez choisi envoyer des messages aux apprenants selectionnés.----------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    printf("Saisissez le nombre d'étudiants à sélectionner ==> ");
    scanf("%d", &nombreCodes);
    printf("Saisissez les codes des étudiants à sélectionner ==>\n");
    for (int i = 0; i < nombreCodes; i++) {
        scanf("%d", &codes[i]);
    }
    printf("Entrez le message ==> ");
    fgets(message, sizeof(message), stdin);
    printf("\033[0m");

    FILE *fichier = fopen("utilisateurs.txt", "r");
    FILE *messagesFp3 = fopen("messages.txt", "a");

    for (int i = 0; i < nombreEtudiants; i++) {
        for (int j = 0; j < nombreCodes; j++) {
            if (u.code == codes[j]) {
                strcpy(etudiants[i].boiteMessages[etudiants[i].messagesNonLus].expediteur, "Administrateur");
                strcpy(etudiants[i].boiteMessages[etudiants[i].messagesNonLus].contenu, message);
                etudiants[i].boiteMessages[etudiants[i].messagesNonLus].etat = envoye;
                etudiants[i].messagesNonLus++;

                fprintf(messagesFp3, "Expéditeur: Administrateur, Destinataire: %s %s, Contenu: %s\n", u.nom, u.prenom, message);
                break;
            }
        }
    }
    fclose(fichier);
    fclose(messagesFp3);

    printf("\033[0;32m");
    printf("Message envoyé avec succès aux étudiants sélectionnés.\n");
    printf("\033[0m");
    envoyerDesMessages();
}
    
void envoyerDesMessages() {
    system("clear");
    Etudiant tableauEtudiants[100];
    int nombreEtudiants = 100;
    int choix;
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|----------------------Vous avez choisi d'envoyer des messages.-----------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;34m");
    printf("1………..ENVOYER MESSAGE POUR TOUS LES ÉTUDIANTS \n");
    printf("2………..ENVOYER UN MESSAGE À UN RÉFÉRENTIEL \n");
    printf("3………..ENVOYER MESSAGE À DES ÉTUDIANTS SÉLECTIONNÉS \n");
    printf("4………..QUITTER \n");
    printf("\033[0;32m");
    printf("VEUILLEZ FAIRE UN CHOIX (1 - 4) ==> ");
    printf("\033[0m");

    scanf("%d", &choix);

    switch (choix) {
        case 1:
            envoyerMessageATous(tableauEtudiants, nombreEtudiants);
            break;
        case 2:
            envoyerMessageParReferentiel(tableauEtudiants, nombreEtudiants);
            break;
        case 3: {
            envoyerMessageAuxSelectionnes(tableauEtudiants, nombreEtudiants);
            break;
        }
        case 4:
            printf("\033[0;32m");
            printf("Quitter.\n");
            printf("\033[0m");
            menuAdmin();
            break;
        default:
            printf("\033[0;31m");
            printf("Choix invalide.\n");
            printf("\033[0m");
            break;
    }
}
void menuAdmin() {
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-------------------BIENVENUE DANS LA PLATEFORME ADMINISTRATEUR-----------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;34m");
    printf("1………..GESTION DES ÉTUDIANTS \n");
    printf("2………..GÉNÉRATION DE FICHIERS ETUDIANTS \n");
    printf("3………..MARQUER LES PRÉSENCES \n");
    printf("4………..ENVOYER DES MESSAGES \n");
    printf("5………..DECONNEXION \n");
    printf("\033[0;32m");
    printf(" VEUILLEZ FAIRE UN CHOIX (1 - 5) ==> ");
    printf("\033[0m");

    do {
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                gestionEtudiants();
                break;
            case 2:
               sousMenuGenerationFichiers();
                break;
            case 3:
               marquerPresences();
                break;
            case 4:
                envoyerDesMessages();
                break;
            case 5:
                printf("\033[0;32m");
                printf("Déconnexion en cours...\n");
                printf("deconnexion reussit\n");
                printf("\033[0m");
                connexion();
                break;
            default:
                printf("\033[0;31m");
                printf("Choix invalide. Veuillez saisir un nombre entre 1 et 5.\n");
                printf("\033[0m");
        }
    } while (choix != 5);
}
void lireMessage(Etudiant *etudiants, int nombreEtudiants) {
    int index;
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-----------------------Vous avez choisi de lire un message.--------------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    printf("Entrez l'index du message à lire ==> ");
    scanf("%d", &index);
    getchar();
    printf("\033[0m");

    if (index >= 1 && index <= nombreEtudiants) {
        printf("\033[0;32m");
        printf("\nLecture du message %d==>\n", index);
        printf("De : %s\n", etudiants[index - 1].boiteMessages[0].expediteur);
        printf("Message ==> %s\n", etudiants[index - 1].boiteMessages[0].contenu);
        printf("\033[0;31m");
    } else {
        printf("\033[0;31m");
        printf("Index de message invalide.\n");
        printf("\033[0m");
    }

    char quitter;
    printf("tapez sur Q pour quitter pour retourner ==> ");
    do {
        scanf(" %c", &quitter);
    } while (quitter != 'Q' && quitter != 'q');
    mesMessages();
}


void consulterBoiteMessages() {
    Etudiant e;
    system("clear||cls");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|---------------Vous avez choisi de consulter votre boîte de messages.----------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;32m");
    if (e.messagesNonLus > 0) {
        printf("Boîte de messages :\n");
        printf("\033[0m");
        for (int i = 0; i < e.messagesNonLus; i++) {
            printf("%d. Expéditeur ==> %s\n, Contenu ==> %s\n, Etat ==> \n ", i + 1, e.boiteMessages[i].expediteur, e.boiteMessages[i].contenu);
            switch (e.boiteMessages[i].etat) {
                case enCours:
                    printf("\033[0;30m");
                    printf("En cours");
                    printf("\033[0m");
                    break;
                case envoye:
                    printf("\033[0;32m");
                    printf("Envoyé");
                    printf("\033[0m");
                    break;
                case echec:
                    printf("\033[0;31m");
                    printf("Échec");
                    printf("\033[0m");
                    break;
            }
            printf("\n");
        }
    } else {
        printf("\033[0;31m");
        printf("Boîte de messages vide.\n");
        printf("\033[0m");
    }
    mesMessages();
}
void mesMessages() {
    Etudiant tableauEtudiants[100];
    int nombreEtudiants = 100;
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|----------------------Vous avez choisi d'envoyer des messages.-----------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;34m");
    printf("1………..TOUS MES MESSAGES NON LUES \n");
    printf("2………..BOITE DE RECEPTION DES MESSAGES \n");
    printf("3………..QUITTER \n");
    printf("\033[0;32m");
    printf(" VEUILLEZ FAIRE UN CHOIX (1 - 3) ==> ");
    printf("\033[0m");

    do {
        scanf("%d", &choix);

        switch (choix) {
            case 1:
               lireMessage(tableauEtudiants, nombreEtudiants);
                break;
            case 2:
               consulterBoiteMessages();
                break;
            case 3:
                printf("\033[0;32m");
                printf("vous avez quitter, à bientôt");
                printf("vous êtes redirigés dans le menu Apprenants");
                printf("\033[0m");
                menuApprenant();
                break;
            default:
                printf("\033[0;31m");
                printf("Choix invalide. Veuillez saisir un nombre entre 1 et 3.\n");
                printf("\033[0m");
        }
    } while (choix != 3);
}
void menuApprenant(){
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("-------------------------BIENVENUE DANS LA PLATEFORME D'ETUDIANT-----------------------\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0;34m");
    printf("1………..MARQUER MA PRÉSENCE \n");
    printf("2………..NOMBRE DE MINUTES D’ABSENCE \n");
    printf("3………..MES MESSAGES (0) \n");
    printf("4………..DECONNEXION \n");
    printf("\033[0;32m");
    printf(" VEUILLEZ FAIRE UN CHOIX (1 - 4) ==> ");
    printf("\033[0m");

    do {
        scanf("%d", &choix);

        switch (choix) {
            case 1:
               // marquezPresence();
                break;
            case 2:
               // nombreMinutesAbsence();
                break;
            case 3:
              mesMessages();
                break;
            case 4:
                printf("\033[0;32m");
                printf("Déconnexion en cours...\n");
                printf("deconnexion reussit\n");
                printf("\033[0m");
                connexion();
                break;
            default:
                printf("\033[0;31m");
                printf("Choix invalide. Veuillez saisir un nombre entre 1 et 4.\n");
                printf("\033[0m");
        }
    } while (choix != 4);
}
void connexion(){

    Utilisateur u;
    system("clear");
    printf("\033[0;33m");
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-------------------BIENVENUE DANS L'APPLICATION SONATEL ACADEMY----------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("\033[0m");

    do {
        trouve = 0;
        saisirUtilisateur();
        estVide2;
        saisirMotdepasse();
        estVide;

        FILE *fichier;
        fichier = fopen("utilisateurs.txt", "r");
        if (fichier == NULL) {
            printf("\033[0;31m");
            printf("Erreur lors de l'ouverture du fichier.");
            printf("\033[0m");
            return;
        }
        int niveau_int;
        while (fscanf(fichier, "%d %s %s %s %s %d %d %d %d %d", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee, &niveau_int, &u.code) != EOF) {
            if (strcmp(empty2, u.email) == 0 && strcmp(empty, u.motdepasse) == 0) {
                printf("J'entre 2 \n");
                trouve = 1;
                u.niveau = (enum niveau)niveau_int;
                break;
            }
        }
        fclose(fichier);
    
        if (trouve) {
            if(niveau_int == 1){
                menuAdmin();
            }else{
                menuApprenant();
            }
            printf("Connexion réussie.\n");
        } else {
            printf("\033[0;31m");
            printf("Email ou mot de passe incorrect.\n");
            printf("\033[0m");
        }
    }while (trouve != 1);
}

int main(){
    connexion();
}
