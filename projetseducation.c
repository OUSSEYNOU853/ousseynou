#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

DATE saisirDate(void)
void fichierUtilisateur(Utilisateur u);
void menuAdmin();
void menuApprenant();
Etudiant envoyerMessage(Etudiant destinataire, char *expediteur, char *contenu);
int choix;

typedef struct {
    int jour;
    int mois;
    int annee;
    int heure;
    int minute;
    int seconde;
} DATE;

typedef struct {
    char expediteur[50];
    char contenu[200];
    enum Etat{envoye = 1, echec, enCours} Etat;
} Message;

typedef struct {
    char nom[50];
    int messagesNonLus;
    Message boiteMessages[20];
} Etudiant;

typedef struct{
    int id;
    time_t date_presence;
} Presence;

typedef struct{
    int id;
    char matricule[10];
    char email[50];
    char motdepasse[50];
    char nom[20];
    char prenom[50];
    Date date_naissance;
    enum etat{BLOQUE, ACTIF} etat;
    enum fonction{ADMIN=1, APPRENANT} fonction;
    enum referentielle{DEV_WEB=1, DEV_DATA, REF_DIG} referentielle;
    Presence presence;
} Utilisateur;

void afficheDate(DATE a)
{
    printf("La date et l'heure sont : %02d/%02d/%d %02d:%02d:%02d\n", a.jour, a.mois, a.annee, a.heure, a.minute, a.seconde);
}

int main()
{
    DATE dateActuelle = saisirDate();
    afficheDate(dateActuelle);

    return 0;
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

Date saisirDate(){
    Date date;
    printf("Saisissez la date au format j/m/a : ");
    date.j = controlSaisiMinMax(1, 31, "Le jour doit-être entre 1 et 7 \n");
    date.m = controlSaisiMinMax(1, 31, "Le mois doit-être entre 1 et 31 \n");
    date.a = controlSaisiMinMax(1980, 2024, "Le jour doit-être entre 1981 et 2024 \n");
    return date;
}

DATE saisirDate(void)
{
    DATE d;
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
void generatematriculeApprenant(int id, char *nom, char *prenom, char *matricule){
    if(id < 10){
        sprintf(matricule, "matricule-%c%c-00%d", nom[0], prenom[0], id);
    }else{
        sprintf(matricule, "matricule-%c%c-0%d", nom[0], prenom[0], id);
    }
}

void informationUtilisateur(Utilisateur u){
    printf("Id utilisateur : %d \n", u.id);
    printf("Nom utilisateur : %s", u.nom);
    printf("Prénom utilisateur : %s", u.prenom);
    printf("Email utilisateur : %s", u.email);
    printf("Mot de passe utilisateur : %s", u.motdepasse);
    printf("Type utilisateur : %d \n", u.fonction);
    printf("Etat utilisateur : %d \n", u.etat);
    if(u.fonction == 2){
        printf("matricule utilisateur : %s \n", u.matricule);
        printf("referentielle utilisateur : %d \n", u.referentielle);
    }
    printf("Date utilisateur : %02d/%02d/%04d \n", u.datenaissance.jour, u.datenaissance.mois, u.datenaissance.annee);
}

void fichierUtilisateur(Utilisateur u){
    FILE *fichierUtilisateur;
    fichierUtilisateur = fopen("utilisateurs.txt", "a");
    if(fichierUtilisateur == NULL){
        printf("Erreur lors de l'ouverture du fichierUtilisateur.");
        return;
    }
    fprintf(fichierUtilisateur, "id: %d matricule: %s email: %s motdepasse: %s nom: %s prenom: %s Date naissance: %02d/%02d/%04d fonction: %d\n", u.id, u.matricule, u.email, u.motdepasse, u.nom, u.prenom, u.datenaissance.jour, u.datenaissance.mois, u.datenaissance.annee, u.fonction);
    fclose(fichierUtilisateur);
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
        }
    }
    motDePasse[i] = '\0'; 
    putchar('\n'); 
    tcsetattr(STDIN_FILENO, TCSANOW, &ancienTermios);
}

void connexion(){

    char empty[50];
    char empty2[50];
    char email[50];
    char motdepasse[50];
    Utilisateur u;
    printf("Bienvenue sur votre interface de connexion, veuillez vous connecter\n");

    int trouve;

    do {
        int estVide2;
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
                printf("L'email ne doit pas contenir que des espaces, veuillez vous reconnecter\n");
                printf("\033[0m");
            }else{
                break;
            }
        }while (estVide2);
    
    
        int estVide;
        
        do{
            printf("Entrez votre mot de passe : ");
            masquer_mot_de_passe_unix(motdepasse, sizeof(motdepasse));   
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
                printf("Le mot de passe ne doit pas contenir que des espaces, veuillez vous reconnecter\n");
                printf("\033[0m");
            }else{
                break;
            }
        }while(estVide);
    

        FILE *fichierUtilisateur;
        fichierUtilisateur = fopen("utilisateurs.txt", "r"); 
        if (fichierUtilisateur == NULL) {
            printf("Erreur lors de l'ouverture du fichierUtilisateur, veuillez vérifier si vous avez bien saisi");
            return;
        }

        
        trouve = 0;
        int fonction_int;
        while (fscanf(fichierUtilisateur, "%d %s %s %s %s %d %d %d %d", &u.id, u.email, u.motdepasse, u.nom, u.prenom, &u.datenaissance.jour, &u.datenaissance.mois, &u.datenaissance.annee, &fonction_int) != EOF) {
            if (strcmp(empty2, u.email) == 0 && strcmp(empty, u.motdepasse) == 0) {
                printf("Je rentre 2 \n");
                trouve = 1;
                u.fonction = (enum fonction)fonction_int;
                break;
            }
        }
        fclose(fichierUtilisateur);
    
        if (trouve) {
            if(fonction_int == 1){
                menuAdmin();
            }else if(fonction_int == 2){
                menuApprenant();
            }
            printf("Entrez votre choix : ");
            scanf("%d", &choix);

        } else {
            printf("\033[0;31m");
            printf("Email ou mot de passe incorrect.\n");
            printf("\033[0m");
        }
    }while (!trouve);
}

void gestionEtudiants() {
    printf("Vous avez choisi la gestion des étudiants.\n");
    FILE *fichierUtilisateur = fopen("utilisateurs.txt", "w");
    if (fichierUtilisateur != NULL) {
        printf("Liste des étudiants :\n");
    }
    fclose(fichierUtilisateur);
}

void generationfichierUtilisateurs() {
    printf("Vous avez choisi la génération de fichierUtilisateurs.\n");
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
    printf("Vous avez choisi de marquer les présences.\n");
    char nomEtudiant[50];
    printf("Entrez le nom de l'étudiant : ");
    scanf("%s", nomEtudiant);
    printf("Présence de l'étudiant %s marquée.\n", nomEtudiant);
}

// Définition de la fonction envoyerMessage
Etudiant envoyerMessage(Etudiant destinataire, char *expediteur, char *contenu) {
    if (destinataire.messagesNonLus < 10) {
        strcpy(destinataire.boiteMessages[destinataire.messagesNonLus].expediteur, expediteur);
        strcpy(destinataire.boiteMessages[destinataire.messagesNonLus].contenu, contenu);
        destinataire.messagesNonLus++;
        printf("Message envoyé à %s : %s\n", destinataire.nom, contenu);
    } else {
        printf("La boîte de messages de %s est pleine. Impossible d'envoyer le message.\n", destinataire.nom);
    }
    return destinataire;
}

// Définition de la fonction envoyerMessage
    void envoyerDesMessages() {
    printf("Vous avez choisi d'envoyer des messages.\n");
     int nombreMessages;
    printf("Combien de messages voulez-vous envoyer à ? : ");
    scanf("%d", &nombreMessages);
    for (int i = 0; i < nombreMessages; i++) {
        char expediteur[50];
        char contenu[1000];

        printf("Saisissez l'expéditeur du message %d : ", i + 1);
        scanf("%s", expediteur);

        printf("Saisissez le contenu du message %d : ", i + 1);
        fgets(contenu, sizeof(contenu), stdin);

        envoyerMessage(&Etudiant, expediteur, contenu);
    }
}

// Définition de la fonction menuAdmin
void menuAdmin() {
    printf("*******************BIENVENUE DANS LA PLATEFORME ADMINISTRATEUR***********************\n");
    printf("1………..GESTION DES ÉTUDIANTS \n");
    printf("2………..GÉNÉRATION DE fichierUtilisateurS \n");
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
                break;
            default:
                printf("Choix invalide. Veuillez saisir un nombre entre 1 et 5.\n");
        }
    } while (choix != 5);
}

// Définition de la fonction menuApprenant
void menuApprenant(){
    printf("*******************BIENVENUE DANS LA PLATEFORME D'ETUDIANT***********************\n");
    printf("1………..MARQUER MA PRÉSENCE \n");
    printf("2………..NOMBRE DE MINUTES D’ABSENCE \n");
    printf("3………..MES MESSAGES (0) \n");
    printf("4………..DECONNEXION \n");
    printf(" VEUILLEZ FAIRE UN CHOIX (1 - 4) : ");
}
int main(){
    connexion();
}