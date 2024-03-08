#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

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
    int code;
    time_t date_presence;
} Presence;

typedef struct {
    char expediteur[50];
    char contenu[200];
    enum Etat {envoye = 1, echec, enCours} etat;
} Message;

typedef struct {
    char nom[50];
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
    Presence presence;
} Utilisateur;

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
void afficheDate(DATES a)
{
    printf("La date et l'heure sont : %02d/%02d/%d %02d:%02d:%02d\n", a.jour, a.mois, a.annee, a.heure, a.minute, a.seconde);
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
    printf("Saisissez la date au format j/m/a : ");
    date.jour = saisirInt(1, 31, "Le jour doit être entre 1 et 31 \n");
    date.mois = saisirInt(1, 12, "Le mois doit être entre 1 et 12 \n");
    date.annee = saisirInt(1980, 2024, "L'année doit être entre 1980 et 2024 \n");
    return date;
}
void matriculeApprenant(int id, char *nom, char *prenom, char *matricule){
    if(id < 10){
        sprintf(matricule, "matricule-%c%c-00%d", nom[0], prenom[0], id);
    }else{
        sprintf(matricule, "matricule-%c%c-0%d", nom[0], prenom[0], id);
    }
}
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
void marquerPresence(int code, Presence* presences, int* nombrePresences) {
    presences[*nombrePresences].code = code;
    presences[*nombrePresences].date_presence = time(NULL);
    (*nombrePresences)++;
}
int saisirUtilisateurs(int code) {
    FILE *fichier = fopen("utilisateurs.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier utilisateurs.txt.");
        return 0;
    }

    Utilisateur u;
    while (fscanf(fichier, "%d %s %s %s %s %d %d %d %d %d", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.date_naissance.jour, &u.date_naissance.mois, &u.date_naissance.annee, &u.niveau, &u.code) != EOF) {
        if (u.code == code) {
            fclose(fichier);
            return 1;
        }
    }

    fclose(fichier);
    return 0; 
}
void enregistrerPresence(Utilisateur u) {
    FILE *fichierPresence = fopen("presences.txt", "a");
    if (!fichierPresence) {
        printf("Impossible d'ouvrir le fichier des présences.\n");
    }
    
    fprintf(fichierPresence, "id: %d matricule: %s email: %s motdepasse: %s nom: %s prenom: %s Date naissance: %02d/%02d/%04d niveau: %d code : %d\n", u.id, u.matricule, u.email, u.motdepasse, u.nom, u.prenom, u.date_naissance.jour, u.date_naissance.mois, u.date_naissance.annee, u.niveau, u.code);
    DATES saisirDates(void);
    fclose(fichierPresence);
    printf("Présence de l'étudiant marquée.\n");

}
int estDejaMarque(Presence *presences, int nombrePresences) {
     int code;
    for (int i = 0; i < nombrePresences; i++) {
        if (presences[i].code == code) {
            return 1;
        }
    }
    return 0;
}
void saisirUtilisateur(){
 do {
        printf("Entrez votre email : ");
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
        printf("Entrez votre motdepasse : ");
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
    printf("---------------------------------------------------------------------------------------\n");
    printf("|----------------------Vous avez choisi la gestion des étudiants.---------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    FILE *fichierUtilisateur = fopen("utilisateurs.txt", "w");
    if (fichierUtilisateur != NULL) {
        printf("Liste des étudiants :\n");
    }
    fclose(fichierUtilisateur);
}
void generationfichierUtilisateurs() {
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-----------------Vous avez choisi la génération de fichierUtilisateurs.--------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    FILE *fichierUtilisateur = fopen("utilisateurs.txt", "w");
    if (fichierUtilisateur != NULL) {
        fprintf(fichierUtilisateur, "Données générées.\n");
        fclose(fichierUtilisateur);
        printf("fichierUtilisateur 'utilisateurs.txt' généré avec succès.\n");
    } else {
        printf("Erreur lors de la génération du fichierUtilisateur.\n");
    }
}
void marquerPresences() {
    int quitter = 0;
    Utilisateur u;
    Presence presences[10000];
    int nombrePresences = 0;

    printf("---------------------------------------------------------------------------------------\n");
    printf("|-----------------Vous avez choisi de marquer les présences.--------------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");

    do {
        printf("Saisissez le code de l'étudiant à marquer : ");
        scanf("%d", &u.code);
        trouve = 0;
        FILE *fichierPresence = fopen("presences.txt", "a");
        if (!fichierPresence) {
            printf("Impossible d'ouvrir le fichier des présences.\n");
        }else{
            if (saisirUtilisateurs(u.code) && !estDejaMarque(presences, nombrePresences)) {
                enregistrerPresence(u);
                nombrePresences++;
            } else {
                printf("\033[0;31m");
                if (!saisirUtilisateurs(u.code)) {
                printf("Code incorrect, veuillez ressaisir.\n");
                } else {
                printf("Le code a déjà été marqué aujourd'hui ou n'est pas valide.\n");
                }
                printf("\033[0m");
        }
        }
        
        fprintf(fichierPresence, "id: %d matricule: %s email: %s motdepasse: %s nom: %s prenom: %s Date naissance: %02d/%02d/%04d niveau: %d code : %d\n", u.id, u.matricule, u.email, u.motdepasse, u.nom, u.prenom, u.date_naissance.jour, u.date_naissance.mois, u.date_naissance.annee, u.niveau, u.code);
        DATES saisirDates(void);
        fclose(fichierPresence);
}while (saisirUtilisateurs(u.code) && !estDejaMarque(presences, nombrePresences));
        printf("Tapez 1 pour quitter : ");
        scanf("%d", &quitter);
        void connexion();
}

Etudiant envoyerMessage(char *expediteur, char *contenu) {
    Etudiant e;
    if (e.messagesNonLus < 10) {
        strcpy(e.boiteMessages[e.messagesNonLus].expediteur, expediteur);
        strcpy(e.boiteMessages[e.messagesNonLus].contenu, contenu);
        e.messagesNonLus++;
        printf("Message envoyé à %s : %s\n", e.nom, contenu);
    } else {
        printf("La boîte de messages de %s est pleine. Impossible d'envoyer le message.\n", e.nom);
    }
    return e;
}
void envoyerDesMessages() {
    printf("---------------------------------------------------------------------------------------\n");
    printf("|----------------------Vous avez choisi d'envoyer des messages.-----------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    int nombreMessages;
    printf("Combien de messages voulez-vous envoyer ? : ");
    scanf("%d", &nombreMessages);
    for (int i = 0; i < nombreMessages; i++) {
        char expediteur[50];
        char contenu[1000];

        printf("Saisissez l'expéditeur du message %d : ", i + 1);
        scanf("%s", expediteur);

        printf("Saisissez le contenu du message %d : ", i + 1);
        scanf(" %[^\n]s", contenu);

        envoyerMessage(expediteur, contenu);
    }
}
void menuAdmin() {
    printf("---------------------------------------------------------------------------------------\n");
    printf("|-------------------BIENVENUE DANS LA PLATEFORME ADMINISTRATEUR-----------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("1………..GESTION DES ÉTUDIANTS \n");
    printf("2………..GÉNÉRATION DE FICHIERS ETUDIANTS \n");
    printf("3………..MARQUER LES PRÉSENCES \n");
    printf("4………..ENVOYER DES MESSAGES \n");
    printf("5………..DECONNEXION \n");
    printf(" VEUILLEZ FAIRE UN CHOIX (1 - 5) : ");

    do {
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                gestionEtudiants();
                break;
            case 2:
                generationfichierUtilisateurs();
                break;
            case 3:
               marquerPresences();
                break;
            case 4:
                envoyerDesMessages();
                break;
            case 5:
                printf("Déconnexion en cours...\n");
                printf("deconnexion reussit\n");
                break;
            default:
                printf("Choix invalide. Veuillez saisir un nombre entre 1 et 5.\n");
        }
    } while (choix != 5);
}
void menuApprenant(){
    printf("---------------------------------------------------------------------------------------\n");
    printf("-------------------------BIENVENUE DANS LA PLATEFORME D'ETUDIANT-----------------------\n");
    printf("---------------------------------------------------------------------------------------\n");
    printf("1………..MARQUER MA PRÉSENCE \n");
    printf("2………..NOMBRE DE MINUTES D’ABSENCE \n");
    printf("3………..MES MESSAGES (0) \n");
    printf("4………..DECONNEXION \n");
    printf(" VEUILLEZ FAIRE UN CHOIX (1 - 4) : ");

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
              //  mesMessages();
                break;
            case 4:
                //deconnexion
                break;
            default:
                printf("Choix invalide. Veuillez saisir un nombre entre 1 et 5.\n");
        }

    } while (choix != 4);
}
void connexion(){

    Utilisateur u;
    printf("Bienvenue sur votre interface de connexion \n");

    do {
        saisirUtilisateur();
        estVide2;
        saisirMotdepasse();
        estVide;

        FILE *fichier;
        fichier = fopen("utilisateurs.txt", "r");
        if (fichier == NULL) {
            printf("Erreur lors de l'ouverture du fichier.");
            return;
        }

            trouve = 0;
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
    }while (!trouve);
}

int main(){

    printf("---------------------------------------------------------------------------------------\n");
    printf("|---------------------------------BIENVENUE DANS L'APPLICATION------------------------|\n");
    printf("---------------------------------------------------------------------------------------\n");
    connexion();
    connexion();

}
