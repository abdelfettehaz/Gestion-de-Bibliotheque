#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
struct date{
    short jj;
    short mm;
    short yy;
};
struct adherant{
    char nom[20];
    char prenom[20];
    int id;
    char addresse[20];
    char type[20];
    char date_entree[10];
    char date_fin[10];
};
typedef struct Element Element;
struct Element
{
    struct adherant adh;
    Element *suivant;
};
typedef struct File File;
struct File
{
    Element *premier;
};
struct localisation{
    int salle;
    int rayon;
};
struct articles{
    char nom_auteur[20];
    char date_pub[10];
};
struct livres{
    char nom_auteur[20];
    char nom_editeur[20];
    char date_edit[10];
};
struct magazines {
    int frequence;
};
struct nombre_doc{
    int nb;
};
struct documents{
    char titre[20];
    struct localisation loc;
    int code;
    int nbexemp;
    char type[20];
    struct articles art;
    struct livres liv;
    struct magazines mag;
};
struct emprunt {
    int id;
    int code_doc;
    char nom[20];
    char prenom[20];
    char addresse[20];
    char date_prend[10];
    char date_donne[10];
};
int nb=-1;
int nbad=-1;
int nbe=-1;
struct documents t[];
struct adherant ta[];
struct emprunt te[];
File* fille = NULL;
// Function Prototypes
void headMessage();
void userMenu();
void ajoutUser();
void modifierUser();
void listUser();
void rechercherUser();
void SupprimerUser();
void menu();
void documentMenu();
void empruntMenu();
void endScreen();
void ajoutemp();
void emp_aff();
int emprun_effectue();
int est_vide();
void supprimer();
void recherche();
void documentslist();
void modifier();
void rechercher();
void Sleep();
void accessible();
int valid();
//////////////////////
void slice(const char*str,char*result,size_t start,size_t end){
    strncpy(result,str+start,end-start);
}
void initFile(File **f) {
    *f = malloc(sizeof(File));
    (*f)->premier = NULL;
}
int check_document(int code) {
    FILE *file = fopen("documents.txt", "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier documents.txt\n");
        return -1;
    }
    FILE *tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        printf("Erreur: Impossible de créer le fichier temporaire.\n");
        fclose(file);
        return -1;
    }
    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char titre[50], type[20], nom_auteur[50], nom_editeur[50], date[20];
        int salle, rayon, code_read, nbexemp, frequence;
        if (sscanf(line, "%49s %d %d %d %d %19s", titre, &salle, &rayon, &code_read, &nbexemp, type) != 6) {
            fprintf(tempFile, "%s", line);
            continue;
        }
        if (code_read == code && nbexemp > 0) {
            nbexemp--;
            found = 1;
        }
        if (strcmp(type, "article") == 0) {
            if (sscanf(line, "%*s %*d %*d %*d %*d %*s %49s %19s", nom_auteur, date) == 2) {
                fprintf(tempFile, "%-20s %d %d %d %d %-10s %s %s\n",
                        titre, salle, rayon, code_read, nbexemp, type, nom_auteur, date);
            }
        } else if (strcmp(type, "livres") == 0) {
            if (sscanf(line, "%*s %*d %*d %*d %*d %*s %49s %49s %19s", nom_auteur, nom_editeur, date) == 3) {
                fprintf(tempFile, "%-20s %d %d %d %d %-10s %s %s %s\n",
                        titre, salle, rayon, code_read, nbexemp, type, nom_auteur, nom_editeur, date);
            }
        } else if (strcmp(type, "magazines") == 0) {
            if (sscanf(line, "%*s %*d %*d %*d %*d %*s %d", &frequence) == 1) {
                fprintf(tempFile, "%-20s %d %d %d %d %-10s %d\n",
                        titre, salle, rayon, code_read, nbexemp, type, frequence);
            }
        }
        else {
            fprintf(tempFile, "%s", line);
        }
    }
    fclose(file);
    fclose(tempFile);
    remove("documents.txt");
    rename("temp.txt", "documents.txt");
    return found;
}
int calcul(struct adherant valeur,struct adherant x){
    if (valid(x.date_fin) && valid(valeur.date_fin)){
        int month1=(valeur.date_fin[0]-'0')*10+(valeur.date_fin[1]-'0');
        int month2=(x.date_fin[0]-'0')*10+(x.date_fin[1]-'0');
        int day1=(valeur.date_fin[3]-'0')*10+(valeur.date_fin[4]-'0');
        int day2=(x.date_fin[3]-'0')*10+(x.date_fin[4]-'0');
        int year1=(valeur.date_fin[6]-'0')*1000+(valeur.date_fin[7]-'0')*1000+(valeur.date_fin[8]-'0')*10+(valeur.date_fin[9]-'0');
        int year2=(x.date_fin[6]-'0')*1000+(x.date_fin[7]-'0')*1000+(x.date_fin[8]-'0')*10+(x.date_fin[9]-'0');
        if (year1==year2 && month1==month2 && day1>day2){
            return 1;
        }
        else if (year1==year2 && month1>month2){
            return 1;
        }
        else if (year1>year2){
            return 1;
        }
        else{
            return 0;
        }
    }
}
int est_vide(File*F){
    if (F->premier==NULL){
        return 1;
    }
    else{
        return 0;
    }
}
void enfiler(File *fille,struct adherant valeur ){
    Element *nouveau = malloc(sizeof(*nouveau));
    if (fille == NULL || nouveau == NULL){
        exit(EXIT_FAILURE);
    }
    nouveau->adh = valeur;
    nouveau->suivant = NULL;
    if (fille->premier != NULL){
        Element *elementActuel = fille->premier;
        while (elementActuel->suivant != NULL){
            elementActuel = elementActuel->suivant;
        }
        elementActuel->suivant = nouveau;
    }
    else{
        fille->premier = nouveau;
    }
}
struct adherant defiler(File *fille){
    if (fille == NULL)
    {
        exit(EXIT_FAILURE);
    }
    struct adherant valeur;
    if (fille->premier != NULL)
    {
        Element *elementDefile = fille->premier;
        valeur = elementDefile->adh;
        fille->premier = elementDefile->suivant;
        free(elementDefile);
    }
    return valeur;
}
int rech(File* f, struct adherant valeur) {
    if (f == NULL) {
        return 0;
    }
    File temp = {NULL};
    int found = 0;
    struct adherant current;
    while (!est_vide(f)) {
        current = defiler(f);
        if (current.id == valeur.id) {
            found = 1;
        }
        enfiler(&temp, current);
    }
    while (!est_vide(&temp)) {
        enfiler(f, defiler(&temp));
    }
    return found;
}
void add(File*F,struct adherant valeur){
    File*F1;
    initFile(&F1);
    struct adherant y;
    if (est_vide(F)){
        enfiler(F,valeur);
    }
    else{
        while (!est_vide(F)){
            y=defiler(F);
            if (calcul(valeur,y)){
                enfiler(F1,y);
            }
            else{
                enfiler(F1,valeur);
                enfiler(F1,y);
            }
        }
        if (!rech(F1,valeur)){
            enfiler(F1,valeur);
        }
        while (!est_vide(F1)){
            y=defiler(F1);
            if (!rech(F,y)){
                enfiler(F,y);
            }
        }
    }
}
void accessible(int x) {
    FILE *fichier;
    char email[100];
    char motDePasse[100];
    char address[100];
    char password[100];
    int found = 0;
    fichier = fopen("adress et mot de pass.txt", "r");
    printf("\n\t\t\t ======================================================");
    printf("\n\t\t\t|                                                      |");
    printf("\n\t\t\t|           ///// Entrez votre email/////              |");
    printf("\n\t\t\t|                                                      |");
    printf("\n\t\t\t ======================================================");
    printf("\n\t\t\t-> ");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = 0;
    printf("\n\t\t\t ======================================================");
    printf("\n\t\t\t|                                                      |");
    printf("\n\t\t\t|       ///// Entrez votre mot de passe /////          |");
    printf("\n\t\t\t|                                                      |");
    printf("\n\t\t\t ======================================================");
    printf("\n\t\t\t-> ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    while (fscanf(fichier, "%s %s", email, motDePasse) == 2) {
        if (strcmp(email, address) == 0 && strcmp(motDePasse, password) == 0) {
            found = 1;
            break;
        }
    }
    if (found) {
        printf("\n\t\t\t        votre cordonne a ete accepte.....    ");
        Sleep(1000);
        system("cls");
        fflush(stdin);
        menu();
    } else {
        printf("\n\t\t\t    votre cordonne est incorrecte......   ");
        if (x==3){
            system("cls");
        }
        else{
            printf("\n\t\t\t    essayer une autre fois.....");
            Sleep(1000);
            system("cls");
            fflush(stdin);
            accessible(x+1);
        }
    }
    fclose(fichier);
}
void headMessage(){
    system("cls");
    printf("\t\t\t===========================================================================");
    printf("\n\t\t\t============                                                   ============");
    printf("\n\t\t\t============        Bienvenue dans la bibliotheque             ============");
    printf("\n\t\t\t============                                                   ============");
    printf("\n\t\t\t===========================================================================\n");
    printf("\n\t\t\t===========================================================================\n");
    printf("                                  realise par abdelfetteh azzouz/mohamed hechem ghorbel                                 ");
    printf("\n\t\t\t============================================================================\n");
    Sleep(2500);
    system("cls");
    fflush(stdin);
    accessible(1);
}
void menu(){
    system("cls");
    fflush(stdin);
    int number;
    printf("----------------------------------\n");
    printf(">>> Library Management System <<< \n");
    printf("----------------------------------\n\n");
    printf(" =====================================\n");
    printf("|                                     |\n");
    printf("|    > 1. Management d'Adherants      |\n");
    printf("|    > 2. Management des Documents    |\n");
    printf("|    > 3. Management des Emprunts     |\n");
    printf("|                                     |\n");
    printf(" =====================================\n\n");
    printf("     > Choisissez votre choix : ");
    scanf("%d",&number);
    fflush(stdin);
    switch(number)
    {
        case 1:
            userMenu();
            break;
        case 2:
            documentMenu();
            break;
        case 3:
            empruntMenu();
            break;
        default:
            printf("\n>>> Invaild Input! <<<");
            Sleep(1000);
            fflush(stdin);
            menu();
    }
}
void empruntMenu(){
    int number;
    system("cls");
    printf("-----------------------------------------------\n");
    printf(">>> Library Management System - User Panel <<< \n");
    printf("-----------------------------------------------\n\n\n");
    printf("  ====================================\n");
    printf(" |                                    |\n");
    printf(" |      > 1. Ajouter un emprunt       |\n");
    printf(" |      > 2. List emprunt             |\n");
    printf(" |      > 3. Ouvrir Main Menu         |\n");
    printf(" |      > 4. Fermer Le Programme...   |\n");
    printf(" |                                    |\n");
    printf("  ====================================\n\n");
    printf(" > Entrer votre choix : ");
    scanf("%d",&number);
    fflush(stdin);
    switch(number)
    {
        case 1:
            ajoutemp();
            break;
        case 2:
            emp_aff();
            break;
        case 3:
            menu();
            break;
        case 4:
            endScreen();
            break;
        default:
            printf("Invaild Input!");
            Sleep(20);
            fflush(stdin);
            empruntMenu();
    }
}
void ajoutemp() {
    system("cls");
    struct emprunt emp;
    char rep[3];
    FILE *file = fopen("emprunt.txt", "a");
    if (!file) {
        printf("Erreur: Impossible d ouvrir le fichier emprunt.txt\n");
        return;
    }
    do {
        printf("Donner l identifiant de l emprunteur: ");
        scanf("%d", &emp.id);
        printf("Donner le code du document emprunte: ");
        scanf("%d", &emp.code_doc);
        int check = check_document(emp.code_doc);
        if (check == -1) {
            printf("Erreur: Lecture de fichier echouee.\n");
            fclose(file);
            empruntMenu();
        } else if (check == 0) {
            fclose(file);
            printf("Le document n existe pas ou n a plus d exemplaires disponibles.\n");
            Sleep(2000);
            empruntMenu();
        }
        printf("Donner le nom de l emprunteur: ");
        scanf("%s", emp.nom);
        printf("Donner le prenom de l emprunteur: ");
        scanf("%s", emp.prenom);
        printf("Donner l adresse de l'emprunteur: ");
        scanf("%s", emp.addresse);
        printf("Donner la date de prise (mm/jj/aaaa): ");
        scanf("%s", emp.date_prend);
        while (!valid(emp.date_prend)) {
            printf("Date invalide. Reessayer: ");
            scanf("%s", emp.date_prend);
        }
        printf("Donner la date de retour (mm/jj/aaaa): ");
        scanf("%s", emp.date_donne);
        while (!valid(emp.date_donne)) {
            printf("Date invalide. Reessayer: ");
            scanf("%s", emp.date_donne);
        }
        fprintf(file, "%d %d %s %s %s %s %s\n", emp.id, emp.code_doc, emp.nom, emp.prenom, emp.addresse, emp.date_prend, emp.date_donne);
         do{
             printf("  Voulez-Vous Ajouter Un Nouveau Adherant ? :  ");
             scanf("%s",&rep);
         }
         while((strcmp(rep,"oui")!=0) &&strcmp(rep,"non")!=0);
    }
    while(strcmp(rep,"non")!=0);
    printf("\n > Voulez-vous retourner au Menu precedant ? : ");
    scanf("%s",rep);
    fclose(file);
    printf("Documents sauvegardes avec succes dans le fichier documents.txt.\n");
    if(strcmp(rep,"oui")==0){
        system("cls");
         empruntMenu();
    }
    else{
        endScreen();
    }
}
void userMenu(){
    system("cls");
    fflush(stdin);
    int number;
    printf("-----------------------------------------------\n");
    printf(">>> Library Management System - User Panel <<< \n");
    printf("-----------------------------------------------\n\n\n");
    printf("  ====================================\n");
    printf(" |                                    |\n");
    printf(" |      > 1. Ajouter un Adherant      |\n");
    printf(" |      > 2. Modifier Adherant        |\n");
    printf(" |      > 3. List Adherant            |\n");
    printf(" |      > 4. Rechercher Adherant      |\n");
    printf(" |      > 5. Supprimer Adherant       |\n");
    printf(" |      > 6. Ouvrir Main Menu         |\n");
    printf(" |      > 7. Fermer Le Programme...   |\n");
    printf(" |                                    |\n");
    printf("  ====================================\n\n");
    printf("> Entrer votre choix : ");
    scanf("%d",&number);
    fflush(stdin);
    switch(number)
    {
        case 1:
            ajoutUser();
            break;
        case 2:
            modifierUser();
            break;
        case 3:
            listUser();
            break;
        case 4:
            rechercherUser();
            break;
        case 5:
            SupprimerUser();
            break;
        case 6:
            menu();
            break;
        case 7:
            endScreen();
            break;
        default:
            printf("Invaild Input!");
            Sleep(2000);
            fflush(stdin);
            userMenu();
    }
}
void ajoutUser(){
    char rep[20];
    char identifiant[20];
    system("cls");
    fflush(stdin);
    do{
            fflush(stdin);
            struct adherant valeur;
         do{
            printf("  Donner le nom de l'Adherant :  ");
            scanf("%s",&valeur.nom);
         }
         while(strlen(valeur.nom)<0);
         do{
            printf("  Donner le prenom de l'Adherant :   ");
            scanf("%s",&valeur.prenom);         }
         while(strlen(valeur.prenom)<0);
         do{
            printf("  Donner l'idantifiant :   ");
            scanf("%d",&valeur.id);
            sprintf(identifiant,"%d",valeur.id);
             if (strlen(identifiant)!=8){
                printf(" <<l identifiant doit etre de 8 chiffres >>\n");}
         }
         while(strlen(identifiant)!=8);
         fflush(stdin);
         do{

            printf("  Donner l'addresse de l'Adherant  :   ");
            gets(valeur.addresse);
         }
         while(strlen(valeur.addresse)<0);
         do{
                fflush(stdin);
                printf("  Donner le type de l'Adherant :   ");
                scanf("%s",&valeur.type);
         }
         while(strcmp(valeur.type,"etudiant")!=0 && strcmp(valeur.type,"enseignant")!=0 && strcmp(valeur.type,"visiteur")!=0);
            if(strcmp(valeur.type,"etudiant")==0){
                printf("     donner le date en d'entree (MM/JJ/AAAA):  ");
                scanf("%s",&valeur.date_entree);
                while (!valid(valeur.date_entree)){
                    printf("     essayer une autre fois :  ");
                    scanf("%s",&valeur.date_entree);
                }

                do {
                    printf("     Donner la date de sortie (MM/JJ/AAAA) : ");
                    scanf("%s", valeur.date_fin);
                    if (!valid(valeur.date_fin)) continue;
                    if (comparer_dates(valeur.date_fin, valeur.date_entree) <= 0) continue;
                    break;
                } while (1);
            }
            else if(strcmp(valeur.type,"enseignant")==0){
                printf("     donner le date en d'entree (MM/JJ/AAAA):  ");
                scanf("%s",&valeur.date_entree);
                while (!valid(valeur.date_entree)){
                    printf("     essayer une autre fois :  ");
                    scanf("%s",&valeur.date_entree);
                }
                    do {

                        printf("     Donner la date de sortie (MM/JJ/AAAA) : ");
                        scanf("%s", valeur.date_fin);
                        if (!valid(valeur.date_fin)) continue;
                        if (comparer_dates(valeur.date_fin, valeur.date_entree) <= 0) continue;
                        break;
                    } while (1);
            }
            else if(strcmp(valeur.type,"visiteur")==0){
                printf("     donner le date en d'entree (MM/JJ/AAAA):  ");
                scanf("%s",&valeur.date_entree);
                while (!valid(valeur.date_entree)){
                    printf("     essayer une autre fois (MM/JJ/AAAA):  ");
                    scanf("%s",&valeur.date_entree);
                    }
                do {

                    printf("     Donner la date de sortie (MM/JJ/AAAA) : ");
                    scanf("%s", valeur.date_fin);
                    if (!valid(valeur.date_fin)) continue;
                    if (comparer_dates(valeur.date_fin, valeur.date_entree) <= 0) continue;
                    break;
                } while (1);
        }
         add(fille,valeur);
         do{
             printf("  Voulez-Vous Ajouter Un Nouveau Adherant ? :  ");
             scanf("%s",&rep);
         }
         while((strcmp(rep,"oui")!=0) &&strcmp(rep,"non")!=0);
    }
    while(strcmp(rep,"non")!=0);
    printf("\n > Voulez-vous retourner au Menu precedant ? : ");
    scanf("%s",rep);
    if(strcmp(rep,"oui")==0){
        system("cls");
        userMenu();
    }
    else{
        endScreen();
    }
}
void modifierUser(){
    File*F1;
    initFile(&F1);
    int choix;
    int id;
    char identifiant[20];
    int i=0;
    system("cls");
    printf("\n> <<<               MENU DE MODIFICATION               >>>: \n");
    printf("    |===========================================|\n");
    printf("    |   >1.<< Donner l'Identification >>        |\n");
    printf("    |   >2.<< Retourner au Menu Precedante >>   |\n");
    printf("    |===========================================|\n\n");
    printf("     >Votre Choix : ");
    scanf("%d",&choix);
    switch(choix){
        case 1:
            printf("\n Donner l'identifiant de l'adherant a modifier ");
            scanf("%d",&id);
            int test=0;
            fflush(stdin);
            struct adherant valeur;
            while(!est_vide(fille)){
               valeur=defiler(fille);
               if(valeur.id==id){
                    printf("\n Adherant Trouve ! \n");
                    fflush(stdin);
                    do{
                      printf("  Donner le nouveau nom de l'Adherant :  ");
                      gets(valeur.nom);
                      }
                    while(strlen(valeur.nom)>10);
                    do{
                       printf("  Donner le nouveau prenom de l'Adherant :   ");
                       scanf("%s",&valeur.prenom);
                       }
                    while(0>valeur.prenom>100);
                    do{
                        printf("  Donner l'idantifiant :   ");
                        scanf("%d",&valeur.id);
                        sprintf(identifiant, "%d", valeur.id);
                        if (strlen(identifiant)!=8){
                            printf(" <<l identifiant doit etre de 8 chiffres >>\n");
                        }
                    }
                    while(strlen(identifiant)!=8);
                    fflush(stdin);
                    do{
                        printf("  Donner l'addresse de l'Adherant  :   ");
                        gets(valeur.addresse);
                    }
                    while(strlen(valeur.addresse)<0);
                    fflush(stdin);
                    do{
                       printf("  Donner le nouveau type de l'Adherant :   ");
                       scanf("%s",&valeur.type);
                       }
                    while(strcmp(valeur.type,"etudiant")!=0 && strcmp(valeur.type,"enseignant")!=0 && strcmp(valeur.type,"visiteur")!=0);
                    if(strcmp(valeur.type,"etudiant")==0){
                        printf("     donner le date en d'entree :  ");
                        scanf("%s",&valeur.date_entree);
                        while (!valid(valeur.date_entree)){
                            printf("     essayer une autre fois :  ");
                            scanf("%s",&valeur.date_entree);
                        }
                        printf("     donner le date de sortie :  ");
                        scanf("%s",&valeur.date_fin);
                        while (!valid(valeur.date_fin)){
                            printf("     essayer une autre fois :  ");
                            scanf("%s",&valeur.date_fin);
                        }
                    }
                    else if(strcmp(valeur.type,"enseignant")==0){
                        printf("     donner le date en d'entree :  ");
                        scanf("%s",&valeur.date_entree);
                        while (!valid(valeur.date_entree)){
                            printf("     essayer une autre fois :  ");
                            scanf("%s",&valeur.date_entree);
                        }
                        printf("     donner le date de sortie :  ");
                        scanf("%s",&valeur.date_fin);
                        while (!valid(valeur.date_fin)){
                            printf("     essayer une autre fois :  ");
                            scanf("%s",&valeur.date_fin);
                        }
                    }
                    else if(strcmp(valeur.type,"visiteur")==0){
                        printf("     donner le date en d'entree :  ");
                        scanf("%s",&valeur.date_entree);
                        while (!valid(valeur.date_entree)){
                            printf("     essayer une autre fois :  ");
                            scanf("%s",&valeur.date_entree);
                        }
                        printf("     donner le date de sortie :  ");
                        scanf("%s",&valeur.date_fin);
                        while (!valid(valeur.date_fin)){
                            printf("     essayer une autre fois :  ");
                            scanf("%s",&valeur.date_fin);
                        }
                    }
                    test=1;
                    add(F1,valeur);
                }
                else{
                    add(F1,valeur);
                }
            }
         while (!est_vide(F1)){
            valeur=defiler(F1);
            enfiler(fille,valeur);
         }
         system("cls");
         if(test==1){
            printf("\n La modification effectue avec succes... ");
            printf("\n retourner au menu principale ...");
            Sleep(3000);
            system("cls");
            modifierUser();
         }
         else{
            printf("\n l'adherant n'existe pas ...\n");
            Sleep(2000);
            printf("\n retourner au menu principale ...");
            Sleep(1500);
            modifierUser();
            break;
         }
         case 2:
             printf("\n retourner au menu principale ... ");
             Sleep(2000);
             system("cls");
             userMenu();
         default:
            printf("Choix invalide. Retour au menu principal...\n");
            Sleep(1500);
            system("cls");
            userMenu();
            break;
    }
}
void listUser(){
    char ktiba[10];
    system("cls");
    File*F1;
    initFile(&F1);
    struct adherant valeur;

    printf("\n> <<<               LIST D ADHERANT              >>>: \n");
    printf("nom        | prenom       | l'identifiant | Addresse       | type         | date d'entrer | date de sortir\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");


    while (!est_vide(fille)){
            valeur=defiler(fille);
            enfiler(F1,valeur);
            printf(" %-10s|", valeur.nom);
            printf(" %-13s|", valeur.prenom);
            printf(" %-14d|", valeur.id);
            printf(" %-15s|", valeur.addresse);
            printf(" %-13s|", valeur.type);
            slice(valeur.date_entree, ktiba, 0, 10);
            printf(" %-13s|", ktiba);
            printf(" %-10s", valeur.date_fin);
            printf("\n");
    }
    fille=F1;
    char rep[3];
    printf("\n\n***Voulez vous revenir au Menu Precedante");
    scanf("%s",&rep);
    if(strcmp(rep,"oui")==0){
        system("cls");
        fflush(stdin);
        userMenu();
    }
}
void rechercherUser(){
    char ktiba[10];
    File*F1;
    initFile(&F1);
    File*F2;
    initFile(&F2);
    struct adherant valeur;
    fflush(stdin);
    int j=0;
    int id;
    char rep[3];
    int choix;
    int test=0;
    system("cls");
    printf("\n> <<<               MENU DE RECHERCHE              >>>: \n");
    printf("    |===========================================|\n");
    printf("    |   >1.<< Donner l'Identification >>        |\n");
    printf("    |   >2.<< Retourner au Menu Precedante >>   |\n");
    printf("    |===========================================|\n\n");
    printf("     >Votre Choix : ");
    scanf("%d",&choix);
    switch (choix) {
        case 1:
            printf("\n>> Donner l'identifiant de l'adherant a rechercher : ");
            scanf("%d",&id);
            while(!est_vide(fille)){
                valeur=defiler(fille);
                enfiler(F1,valeur);
                if (valeur.id==id){
                    enfiler(F2,valeur);
                    test=1;
                }
            }
            fille=F1;
            if(test==1){
                printf("L'Adherant existe \n");
                printf("Voulez-vous l'afficher ? => ");
                scanf("%s",&rep);
                if(strcmp(rep,"oui")==0){
                    system("cls");
                    valeur=defiler(F2);
                    printf("\n> <<<               LIST D'ADHERANT              >>>: \n");
                    printf("nom        | prenom       | l'identifiant | Addresse       | type         | date d'entrer | date de sortir\n");
                    printf("-----------------------------------------------------------------------------------------------------------\n");
                    printf(" %-10s|", valeur.nom);
                    printf(" %-13s|", valeur.prenom);
                    printf(" %-14d|", valeur.id);
                    printf(" %-15s|", valeur.addresse);
                    printf(" %-13s|", valeur.type);
                    slice(valeur.date_entree, ktiba, 0, 10);
                    printf(" %-14s|", ktiba);
                    printf(" %-10s", valeur.date_fin);
                }
            }
            else{
                printf("\n\n l'adherant n'existe pas.... \t");
            }
            printf("\nRetour au menu precedent........\n");
            Sleep(2000);
            system("cls");
            rechercherUser();
            break;
        case 2:
            printf("Retour au menu principal...\n");
            Sleep(5);
            system("cls");
            userMenu();
            break;
        default:
            printf("Choix invalide. Retour au menu principal...\n");
            Sleep(3);
            system("cls");
            userMenu();
            break;
    }
}
void SupprimerUser(){
    File*F1;
    initFile(&F1);
    struct adherant valeur;
    system("cls");
    int choix;
    char rep[3];
    int id;
    int test=0;
    printf("\n> <<<               MENU DE SUPRISSION              >>>: \n");
    printf("    |===========================================|\n");
    printf("    |   >1.<< Donner l'Identification >>        |\n");
    printf("    |   >2.<< Retourner au Menu Precedante >>   |\n");
    printf("    |===========================================|\n\n");
    printf("     >Votre Choix : ");
    scanf("%d",&choix);
    switch(choix){
        case 1:
            printf("\n>> Donner l'identifiant de l'Adherant a Supprimer  : ");
            scanf("%d",&id);
            while(!est_vide(fille)){
               valeur=defiler(fille);
               enfiler(F1,valeur);
                if (valeur.id==id){
                    test=1;
                }
            }
            if(test==1){
                printf("L'Adherant existe \n");
                printf("Voulez Vous le Supprimer ? : ");
                scanf("%s",&rep);
                if(strcmp(rep,"oui")==0){
                    system("cls");
                    while(!est_vide(F1)){
                        valeur=defiler(F1);
                        if (valeur.id!=id){
                            enfiler(fille,valeur);
                        }
                    }
                    printf("\nL'Adherant Ayant l'identifiant %d est en cours de Suppression ... ",id);
                    Sleep(2000);
                    system("cls");
                    printf("\n La suppression est effectue avec success ...");
                    printf("\n retourner au menu précédante... ");
                    Sleep(2000);
                    userMenu();
                }
                else{
                    printf("\n Suppression annulee. Retour au menu precedante");
                    Sleep(1500);
                    system("cls");
                    SupprimerUser();
                }
            }
            else{
                printf("L'Adherant avec l'identifiant %d n'existe pas\n",id);
                Sleep(2000);
                system("cls");
                SupprimerUser();
            }
            break;
        case 2:
            printf("Retour au menu principal...\n");
            Sleep(5);
            system("cls");
            userMenu();
            break;
        default:
            printf("Choix invalide. Retour au menu principal...\n");
            Sleep(3);
            system("cls");
            userMenu();
            break;
    }
}
void ajout_documment() {
    struct documents valeur;
    char rep[3];
    FILE *file = fopen("documents.txt", "a");
    if (file == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier documents.txt\n");
        return;
    }
    do{
    fflush(stdin);
    system("cls");
    do {
        printf("Donner le titre de document : ");
        fgets(valeur.titre, sizeof(valeur.titre), stdin);
        valeur.titre[strcspn(valeur.titre, "\n")] = '\0';
    } while (strlen(valeur.titre) > 20);
    do {
        printf("Donner la salle du document (1-100): ");
        scanf("%d", &valeur.loc.salle);
    } while (valeur.loc.salle < 1 || valeur.loc.salle > 100);
    do {
        printf("Donner le rayon du document (1-100): ");
        scanf("%d", &valeur.loc.rayon);
    } while (valeur.loc.rayon < 1 || valeur.loc.rayon > 100);
    do {
        printf("Donner le code du document: ");
        scanf("%d", &valeur.code);
    } while (valeur.code < 0);
    do {
        printf("Donner le nombre d exemplaires du document: ");
        scanf("%d", &valeur.nbexemp);
    } while (valeur.nbexemp < 0);
    do {
        printf("Donner le type de document (article/livres/magazines): ");
        scanf("%s", valeur.type);
    }while (strcmp(valeur.type, "article") != 0 &&
                strcmp(valeur.type, "livres") != 0 &&
                strcmp(valeur.type, "magazines") != 0);
    if (strcmp(valeur.type, "article") == 0) {
        printf("Donner le nom de l auteur: ");
        scanf("%s", valeur.art.nom_auteur);
        do {
            printf("Donner la date de publication (mm/jj/aaaa): ");
            scanf("%s", valeur.art.date_pub);
        }
        while (valid(valeur.art.date_pub) == 0);
        fprintf(file, "%s %d %d %d %d %s %s %s\n",
                valeur.titre, valeur.loc.salle, valeur.loc.rayon,
                valeur.code, valeur.nbexemp, valeur.type,
                valeur.art.nom_auteur, valeur.art.date_pub);
    } else if (strcmp(valeur.type, "livres") == 0) {
        printf("Donner le nom de l auteur: ");
        scanf("%s", valeur.liv.nom_auteur);
        printf("Donner le nom de l editeur: ");
        scanf("%s", valeur.liv.nom_editeur);
        do {
            printf("Donner la date d edition (mm/jj/aaaa): ");
            scanf("%s", valeur.liv.date_edit);
        } while (valid(valeur.liv.date_edit) == 0);
        fprintf(file, "%s %d %d %d %d %s %s %s %s\n",
                valeur.titre, valeur.loc.salle, valeur.loc.rayon,
                valeur.code, valeur.nbexemp, valeur.type,
                valeur.liv.nom_auteur, valeur.liv.nom_editeur,
                valeur.liv.date_edit);
    } else if (strcmp(valeur.type, "magazines") == 0) {
        printf("Donner la frequence du magazine: ");
        scanf("%d", &valeur.mag.frequence);
        fprintf(file, "%s %d %d %d %d %s %d\n",
                valeur.titre, valeur.loc.salle, valeur.loc.rayon,
                valeur.code, valeur.nbexemp, valeur.type,
                valeur.mag.frequence);
    }
    do{
        printf("  Voulez-Vous Ajouter Un Nouveau documment ? :  ");
        scanf("%s",&rep);
    }
    while((strcmp(rep,"oui")!=0) &&strcmp(rep,"non")!=0);
    }
    while(strcmp(rep,"non")!=0);
    fclose(file);
    printf("\n > Voulez-vous retourner au Menu precedant ? : ");
    scanf("%s",rep);
    if(strcmp(rep,"oui")==0){
        system("cls");
        documentMenu();
    }
    else{
        endScreen();
    }
}
void documentMenu(){
    int number;
    system("cls");
    printf("-----------------------------------------------\n");
    printf(">>> Library Management System - User Panel <<< \n");
    printf("-----------------------------------------------\n\n\n");
    printf("  ====================================\n");
    printf(" |                                    |\n");
    printf(" |      > 1. Ajouter un documment     |\n");
    printf(" |      > 2. Modifier documment       |\n");
    printf(" |      > 3. List documment           |\n");
    printf(" |      > 4. Rechercher documment     |\n");
    printf(" |      > 5. Supprimer documment      |\n");
    printf(" |      > 6. Ouvrir Main Menu         |\n");
    printf(" |      > 7. Fermer Le Programme...   |\n");
    printf(" |                                    |\n");
    printf("  ====================================\n\n");
    printf(" > Entrer votre choix : ");
    scanf("%d",&number);
    fflush(stdin);
    switch(number)
        {
        case 1:
            ajout_documment();
            break;
        case 2:
            modifier_documment();
            break;
        case 3:
            documentslist();
            break;
        case 4:
            rechercher_documment();
            break;
        case 5:
            supprimer_documment();
            break;
        case 6:
            menu();
            break;
        case 7:
            endScreen();
            break;
            default:
            printf("Invaild Input!");
            Sleep(2000);
         }
}
void endScreen(){
    system("cls");
    fflush(stdin);
    char etoile[5];
    do{
             printf("donner nous votre evaluation /5 en etoile ");
             scanf("%s",&etoile);
         }
    while((strcmp(etoile,"*")!=0) &&strcmp(etoile,"**")!=0 && (strcmp(etoile,"***")!=0) &&strcmp(etoile,"*****")!=0 && (strcmp(etoile,"*****")!=0));
    printf("merci pour votre attention !!!!");
    exit(0);
}
int comparer_dates(char *date, char *date2) {
    int y=(date[6]-'0')*1000+(date[7]-'0')*1000+(date[8]-'0')*10+(date[9]-'0');
    int y2=(date2[6]-'0')*1000+(date2[7]-'0')*1000+(date2[8]-'0')*10+(date2[9]-'0');
    int m=(date[0]-'0')*10+(date[1]-'0');
    int m2=(date2[0]-'0')*10+(date2[1]-'0');
    int j=(date[3]-'0')*10+(date[4]-'0');
    int j2=(date2[3]-'0')*10+(date2[4]-'0');
    if ( y<y2 ) {
        return -1;
    } else
    {
     if (m < m2) {
        return -1;
     } else {

    if (j < j2) {
        return -1;
    }
     }
     }
    return 1;
}
int valid(char *date){
    if(strlen(date)!=10){
        return 0;
    }
    int month=(date[0]-'0')*10+(date[1]-'0');
    if(month<1 || month > 12){
        return 0;
    }
    if(date[2]!='/' || date[5]!='/'){
        return 0;
    }
    int day=(date[3]-'0')*10+(date[4]-'0');
    if(day <1 || day >31){
        return 0;
    }
    int year=(date[6]-'0')*1000+(date[7]-'0')*1000+(date[8]-'0')*10+(date[9]-'0');
    if(year <1 || year >9999){
        return 0;
    }
    return 1;
}
void modifier_documment() {
    system("cls");
    int choix;
    printf("\n> <<<               MENU DE MODIFICATION               >>>: \n");
    printf("    |===========================================|\n");
    printf("    |   >1.<< Code >>                           |\n");
    printf("    |   >2.<< Retourner au menu precedent  >>   |\n");
    printf("    |===========================================|\n\n");
    printf("     >Votre Choix : ");
    scanf("%d",&choix);
    switch(choix){
        case 1:{
            char rep[3];
            int code, found = 0;
            FILE *file = fopen("documents.txt", "r");
            if (!file) {
             printf("Erreur : Impossible d ouvrir le fichier documents.txt\n");
            return;
            }
                FILE *tempFile = fopen("temp.txt", "w");
                if (!tempFile) {
                    printf("Erreur : Impossible de creer le fichier temporaire\n");
                    fclose(file);
                    return;
                }
                printf("\n>> Donner le code du document a modifier : ");
                scanf("%d", &code);
                char type[20], titre[50], nom_auteur[50], nom_editeur[50], date[20];
                int salle, rayon, code_read, nbexemp, frequence;
                while (fscanf(file, "%49s %d %d %d %d %19s", titre, &salle, &rayon, &code_read, &nbexemp, type) == 6) {
                    if (code == code_read) {
                        found = 1;
                        printf("\nLe document avec le code %d a ete trouve.\n", code_read);
                        printf("\n--- Details actuels ---\n");
                        Sleep(1500);
                        system("cls");
                        if (strcmp(type, "article") == 0) {
                            fscanf(file, "%49s %19s", nom_auteur, date);
                            printf("Titre : %s, Salle : %d, Rayon : %d, Exemplaires : %d, Type : %s, Auteur : %s, Date : %s\n",
                                   titre, salle, rayon, nbexemp, type, nom_auteur, date);
                        } else if (strcmp(type, "livres") == 0) {
                            fscanf(file, "%49s %49s %19s", nom_auteur, nom_editeur, date);
                            printf("Titre : %s, Salle : %d, Rayon : %d, Exemplaires : %d, Type : %s, Auteur : %s, Editeur : %s, Date : %s\n",
                                   titre, salle, rayon, nbexemp, type, nom_auteur, nom_editeur, date);
                        } else if (strcmp(type, "magazines") == 0) {
                            fscanf(file, "%d", &frequence);
                            printf("Titre : %s, Salle : %d, Rayon : %d, Exemplaires : %d, Type : %s, Frequence : %d\n",
                                   titre, salle, rayon, nbexemp, type, frequence);
                        }
                        printf("\nEntrez les nouveaux details du document (Code inchange):\n");
                        printf("Titre : ");
                        scanf("%s", titre);
                        printf("Salle : ");
                        scanf("%d", &salle);
                        printf("Rayon : ");
                        scanf("%d", &rayon);
                        printf("Nombre d'exemplaires : ");
                        scanf("%d", &nbexemp);
                        printf("Type (article, livres, magazines) : ");
                        scanf("%s", type);
                        if (strcmp(type, "article") == 0) {
                            printf("Auteur : ");
                            scanf("%s", nom_auteur);
                            printf("Date (JJ/MM/AAAA) : ");
                            scanf("%s", date);
                            fprintf(tempFile, "%s %d %d %d %d %s %s %s\n", titre, salle, rayon, code_read, nbexemp, type, nom_auteur, date);
                        } else if (strcmp(type, "livres") == 0) {
                            printf("Auteur : ");
                            scanf("%s", nom_auteur);
                            printf("Editeur : ");
                            scanf("%s", nom_editeur);
                            printf("Date d'édition (JJ/MM/AAAA) : ");
                            scanf("%s", date);
                            fprintf(tempFile, "%s %d %d %d %d %s %s %s %s\n", titre, salle, rayon, code_read, nbexemp, type, nom_auteur, nom_editeur, date);
                        } else if (strcmp(type, "magazines") == 0) {
                            printf("Frequence : ");
                            scanf("%d", &frequence);
                            fprintf(tempFile, "%s %d %d %d %d %s %d\n", titre, salle, rayon, code_read, nbexemp, type, frequence);
                        }
                    } else {
                        fprintf(tempFile, "%s %d %d %d %d %s ", titre, salle, rayon, code_read, nbexemp, type);
                        if (strcmp(type, "article") == 0) {
                            fscanf(file, "%49s %19s", nom_auteur, date);
                            fprintf(tempFile, "%s %s\n", nom_auteur, date);
                        } else if (strcmp(type, "livres") == 0) {
                            fscanf(file, "%49s %49s %19s", nom_auteur, nom_editeur, date);
                            fprintf(tempFile, "%s %s %s\n", nom_auteur, nom_editeur, date);
                        } else if (strcmp(type, "magazines") == 0) {
                            fscanf(file, "%d", &frequence);
                            fprintf(tempFile, "%d\n", frequence);
                        }
                    }
                }
                fclose(file);
                fclose(tempFile);
                if (found) {
                    remove("documents.txt");
                    rename("temp.txt", "documents.txt");
                    printf("\nLe document a ete modifie avec succes.\n");
                    Sleep(2000);
                    modifier_documment();
                }
                else {
                    remove("temp.txt");
                    printf("\nLe document avec le code %d n existe pas.\n", code);
                    printf("\n\n***retourner au Menu Preincpale");
                    Sleep(2000);
                    modifier_documment();
                }
         }
         case 2:
             system("cls");
             printf("\n retourner au menu principale ... ");
             Sleep(2000);
             system("cls");
             documentMenu();
         default:
            printf("Choix invalide. Retour au menu principal...\n");
            Sleep(1500);
            system("cls");
            modifier_documment();
            break;
    }
}
void rechercher_documment() {
    char rep[3];
    FILE *file = fopen("documents.txt", "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier documents.txt\n");
        return;
    }
    int code, found = 0;
    char type[20], titre[50], nom_auteur[50], nom_editeur[50], date[20];
    int salle, rayon, code_read, nbexemp, frequence;
    system("cls");
    printf("\n> <<<               MENU DE RECHERCHE              >>>: \n");
    printf("    |===========================================|\n");
    printf("    |   >1.<< Code >>                           |\n");
    printf("    |   >2.<< Retourner au menu precedent  >>   |\n");
    printf("    |===========================================|\n\n");
    printf("     >Votre Choix : ");
    int choix;
    scanf("%d", &choix);
    switch (choix) {
        case 1:
            system("cls");
            printf("\n>> Donner le code du document a rechercher : ");
            scanf("%d", &code);

            while (fscanf(file, "%49s %d %d %d %d %19s", titre, &salle, &rayon, &code_read, &nbexemp, type) == 6) {
                if (code == code_read) {
                    found =found+1;
                printf("\nLe Document Ayant le Code  %d est exist \n", code);
                printf("Titre               | Salle | Rayon | Code   | Exemplaires | Type       | Details\n");
                printf("-------------------------------------------------------------------------------------\n");
                if (strcmp(type, "article") == 0) {
                    if (fscanf(file, "%49s %19s", nom_auteur, date) == 2) {
                        printf("%-20s | %-5d | %-5d | %-6d | %-11d | %-10s | Auteur: %s, Pub: %s\n",
                               titre, salle, rayon, code, nbexemp, type, nom_auteur, date);

                    }
                } else if (strcmp(type, "livres") == 0) {
                    if (fscanf(file, "%49s %49s %19s", nom_auteur, nom_editeur, date) == 3) {
                        printf("%-20s | %-5d | %-5d | %-6d | %-11d | %-10s | Auteur: %s, Editeur: %s, Edition: %s\n",
                               titre, salle, rayon, code, nbexemp, type, nom_auteur, nom_editeur, date);
                    }
                } else if (strcmp(type, "magazines") == 0) {
                    if (fscanf(file, "%d", &frequence) == 1) {
                        printf("%-20s | %-5d | %-5d | %-6d | %-11d | %-10s | Frequence: %d\n",
                               titre, salle, rayon, code, nbexemp, type, frequence);
                }

                }
            }
            }

            if (found==0) {
                fclose(file);
                system("cls");
                printf("\n\t\t\t  ***********Le document n'existe pas.....*************");
                printf("\n\t\t\t  ***********retourner au menu Prcedente.....*************");
                printf("\n\t\t\t                        Attendez.....");
                Sleep(2500);
                system("cls");
                rechercher_documment();
            }
          else{
            fclose(file);
            Sleep(2500);
            rechercher_documment();
          }
         case 2:
             system("cls");
             system("cls");
             printf("\n retourner au menu principale ... ");
             Sleep(2000);
             system("cls");
             documentMenu();
         default:
            printf("Choix invalide. Retour au menu principal...\n");
            Sleep(1500);
            system("cls");
            rechercher_documment();
            break;
  }
  }
void supprimer_documment() {

    int code, found = 0;
    FILE *file = fopen("documents.txt", "r");
    if (!file) {
        printf("Erreur : Impossible d ouvrir le fichier documents.txt\n");
        return;
    }
    FILE *tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        printf("Erreur : Impossible de creer le fichier temporaire\n");
        fclose(file);
        return;
    }
    printf("\n>> Donner le code du document a supprimer : ");
    scanf("%d", &code);
    char type[20], titre[50], nom_auteur[50], nom_editeur[50], date[20];
    int salle, rayon, code_read, nbexemp, frequence;
    while (fscanf(file, "%49s %d %d %d %d %19s", titre, &salle, &rayon, &code_read, &nbexemp, type) == 6) {
        if (code == code_read) {
            found = 1;
            printf("\nLe document avec le code %d a ete supprime.\n", code);
            if (strcmp(type, "article") == 0) {
                fscanf(file, "%49s %19s", nom_auteur, date);
            } else if (strcmp(type, "livres") == 0) {
                fscanf(file, "%49s %49s %19s", nom_auteur, nom_editeur, date);
            } else if (strcmp(type, "magazines") == 0) {
                fscanf(file, "%d", &frequence);
            }
            continue;
        }
        fprintf(tempFile, "%s %d %d %d %d %s ", titre, salle, rayon, code_read, nbexemp, type);
        if (strcmp(type, "article") == 0) {
            fscanf(file, "%49s %19s", nom_auteur, date);
            fprintf(tempFile, "%s %s\n", nom_auteur, date);
        } else if (strcmp(type, "livres") == 0) {
            fscanf(file, "%49s %49s %19s", nom_auteur, nom_editeur, date);
            fprintf(tempFile, "%s %s %s\n", nom_auteur, nom_editeur, date);
        } else if (strcmp(type, "magazines") == 0) {
            fscanf(file, "%d", &frequence);
            fprintf(tempFile, "%d\n", frequence);
        }
    }
    fclose(file);
    fclose(tempFile);
    if (!found) {
        printf("\nAucun document avec le code %d n a ete trouve.\n", code);
    } else {
        remove("documents.txt");
        rename("temp.txt", "documents.txt");
    }
    printf("\nRetour au menu precedent...\n");
    Sleep(2500);
    system("cls");
    documentMenu();
}
void documentslist() {
    FILE *file = fopen("documents.txt", "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier documents.txt\n");
        return;
    }
    system("cls");
    char ligne[256];
    char titre[50], type[20], nom_auteur[50], date[20], nom_editeur[50];
    int salle, rayon, code, nbexemp, frequence;
    printf("\n--- Liste des Documents ---\n");
    printf("Titre               | Salle | Rayon | Code   | Exemplaires | Type        |          Details\n");
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    while (fgets(ligne, sizeof(ligne), file)) {
        int salle, rayon, code, nbexemp, frequence;
        char titre[50], type[20], nom_auteur[50], nom_editeur[50], date[20];
        if (sscanf(ligne, "%49s %d %d %d %d %19s", titre, &salle, &rayon, &code, &nbexemp, type) != 6) {
            printf("Erreur : Ligne mal formatee -> %s", ligne);
            continue;
        }
        if (strcmp(type, "article") == 0) {
            if (sscanf(ligne, "%49s %d %d %d %d %19s %49s %19s",
                       titre, &salle, &rayon, &code, &nbexemp, type, nom_auteur, date) == 8) {
                printf("%-20s | %-5d | %-5d | %-6d | %-11d | %-10s | Auteur: %s, Pub: %s\n",
                       titre, salle, rayon, code, nbexemp, type, nom_auteur, date);
            } else {
                printf("Erreur : Article mal formaté -> %s", ligne);
            }
        } else if (strcmp(type, "livres") == 0) {
            if (sscanf(ligne, "%49s %d %d %d %d %19s %49s %49s %19s",
                       titre, &salle, &rayon, &code, &nbexemp, type, nom_auteur, nom_editeur, date) == 9) {
                printf("%-20s | %-5d | %-5d | %-6d | %-11d | %-10s | Auteur: %s, Editeur: %s, Edition: %s\n",
                       titre, salle, rayon, code, nbexemp, type, nom_auteur, nom_editeur, date);
            } else {
                printf("Erreur : Livre mal formate -> %s", ligne);
            }
        } else if (strcmp(type, "magazines") == 0) {
            if (sscanf(ligne, "%49s %d %d %d %d %19s %d",
                       titre, &salle, &rayon, &code, &nbexemp, type, &frequence) == 7) {
                printf("%-20s | %-5d | %-5d | %-6d | %-11d | %-10s | Frequence: %d\n",
                       titre, salle, rayon, code, nbexemp, type, frequence);
            } else {
                printf("Erreur : Magazine mal formaté -> %s", ligne);
            }
        } else {
            printf("Erreur : Type inconnu -> %s\n", ligne);
        }
    }
    fclose(file);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    char rep[3];
    printf("\n\n***Voulez vous revenir au Menu Precedante");
    scanf("%s",&rep);
    if(strcmp(rep,"oui")==0){
        system("cls");
        fflush(stdin);
        documentMenu();
    }
}
void emp_aff() {
    system("cls");
    char rep[3];
    FILE *file = fopen("emprunt.txt", "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier emprunt.txt\n");
        return;
    }
    struct emprunt emp;
    printf("\n--- Liste des Emprunts ---\n");
    printf("ID   | Code Doc | Nom          | Prénom       | Adresse       | Date Prise       |      Date Retour\n");
    printf("--------------------------------------------------------------------------------------------------------\n");
    while (fscanf(file, "%d %d %s %s %s %s %s", &emp.id, &emp.code_doc, emp.nom, emp.prenom, emp.addresse, emp.date_prend, emp.date_donne) == 7) {
        printf("%-5d | %-9d | %-12s | %-12s | %-12s | %-10s | %-10s\n",
               emp.id, emp.code_doc, emp.nom, emp.prenom, emp.addresse, emp.date_prend, emp.date_donne);
    }
    fclose(file);
    printf("---------------------------------------------------------------------------------------------------------\n");
    printf("\n\n***Voulez vous revenir au Menu Precedante");
    scanf("%s",&rep);
    if(strcmp(rep,"oui")==0){
        system("cls");
        fflush(stdin);
        empruntMenu();
    }
    else{
        printf("\n\n*** Revenir au Menu Principale....");
        Sleep(2000);
        menu();
    }
}
int main(){
    initFile(&fille);
	headMessage();
	return 0;
}
