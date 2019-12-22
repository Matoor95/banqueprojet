#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#define MAXNOM 20
#define MAX_SIZE 80
typedef struct {     //structure pour le date
    int jour;
    int mois;
    int annee;
}DATE;

typedef struct {    // structure information client
    int numero_cmpt;
    char nom[MAXNOM];
    char der_operation;
    double anc_solde;
    double nouv_solde;
    DATE date;
}CLIENT;

void getDate(DATE d) { //fonction pour obtenir la date machine
    
    time_t timestamp = time( NULL );// t contient maintenant la date et l'heure courante
    struct tm * pTime = localtime( & timestamp );
    char buffer[ MAX_SIZE ];
    strftime( buffer, MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime );
    printf( "Date and french time : %s\n", buffer );

}

void ouvrir(FILE **f,char nomfich[]){  // Fonction pour ouvrir le fichier
    *f=fopen(nomfich,"r+"); // ouvrir on mode lecture et ecriture
    if(*f==NULL){
        *f=fopen(nomfich,"w+"); //sinon ouvrir on mode écriture et lecture (creer le fichier automatiquement
        if(*f==NULL){
            perror("Erreur d'ouverture du fichier!"); //afficher un message d'erreur
            exit(EXIT_FAILURE);
        }
    }
}

void fermer(FILE *fich){  //fonction pour fermer le fichier
    if(fich!=NULL){
        fclose(fich);
    }
}

int chercher_compte(FILE *fich,int cpt,char nom[]){  //fonction to seek the desired account
    CLIENT client;
    int trouve=0; char ret;
    rewind(fich);//pointeur au debut
    while(!trouve){
        ret=fread(&client,sizeof(CLIENT),1,fich);
        if(ret==0)break;
        if((client.numero_cmpt==cpt)||(strcmp(client.nom,nom))){
            fseek(fich,-11*sizeof(CLIENT),SEEK_CUR);
            return 1;
        }
    }
    return 0;
    
}

int ajout(FILE *fich){  //fonction to add an account
    char ret;
    CLIENT client;
    printf("Ajout d'un client\n");
    printf("\tNumero de compte:");
    scanf("%d",&client.numero_cmpt);
    if(chercher_compte(fich,client.numero_cmpt,client.nom)){
        fprintf(stderr,("Compte existant\n"));
        return 0;
    }
    printf("\tNom:");
    scanf("%s",client.nom);
    fflush(stdin);
    client.der_operation='V';
    client.anc_solde=0.0;
    printf("\tSolde initial:");
    scanf("%lf",&client.nouv_solde);
    //int t=getDate(client.date);
    
    time_t timestamp = time( NULL );// t contient maintenant la date et l'heure courante
    struct tm * pTime = localtime( & timestamp );
    //char buffer[ MAX_SIZE ];
    int day = pTime->tm_mday;
    int mois = pTime->tm_mon + 1;
    int an = pTime->tm_year + 1900;
    client.date.jour=day;
    client.date.mois=mois;
    client.date.annee=an;
    fseek(fich,0,SEEK_END);//moving pointer to end
    ret=fwrite(&client,sizeof(CLIENT),1,fich);
    printf("Succes !");
    return ret;
}

void affiche(FILE *fich){   //fonction to show the account desired
    CLIENT cli;
    char nom[MAXNOM];
    int cpt,ret;
    printf("Consultation par numero du compte ou nom  \n");
    printf("Numero du compte ou nom:");
    rewind(stdin);/* vide le tampon avant une lecture */
    scanf("%d",&cpt);
    ret=chercher_compte(fich,cpt,nom);
    if(ret==0){
        printf("Compte inexistant...\n");
    }
    else{
        fread(&cli,sizeof(CLIENT),1,fich);
        printf("compte:\t%d\nNom:\t%s\nDerniere operation: %c\nAncien solde:%.2f\nNouveau solde:%.2f\nDate:\t%d/%d/%d\n",cli.numero_cmpt,cli.nom,cli.der_operation,cli.anc_solde,cli.nouv_solde,cli.date.jour,cli.date.mois,cli.date.annee);
    }}

void lister(FILE *fich){   //fontion to list all bank accounts already added
    CLIENT cli;
    rewind(fich);
    printf("listage du contenu du fichier.\n");
    printf("Num\t\tNom\t\tOpe\t\tAnc\t\tNouv\t\tDate\n");
    while(fread(&cli,sizeof(CLIENT),1,fich)==1){
        printf("%d\t\t%s\t\t%c\t\t%.2f\t\t%.2f\t\t%d/%d/%d\n",cli.numero_cmpt,cli.nom,cli.der_operation,cli.anc_solde,cli.nouv_solde,cli.date.jour,cli.date.mois,cli.date.annee);
    }
}

void operation(FILE *fich){   //fonction for modifiying an account informations
    CLIENT cli;
    char choix;
    double somme;
    printf("Numero du compte:");
    scanf("%d",&cli.numero_cmpt);
    if(!chercher_compte(fich,cli.numero_cmpt,cli.nom)){
        printf("Compte inexistant...\n");
        return;
    }
    fread(&cli,sizeof(CLIENT),1,fich); // read from file
    printf("Compte %d\nNom %s\n Ancien solde %.2f\nNouveau solde %.2f\nDate %d/%d/%d\n",cli.numero_cmpt,cli.nom,cli.anc_solde,cli.nouv_solde,cli.date.jour,cli.date.mois,cli.date.annee);
    printf("Que voulez-vous faire?\n");
    do{
        printf("Versement:V\n Retrait:R\n Votre choix:");
        scanf("%c",&choix);
    }while (choix!='V'&& choix!='R');
    cli.anc_solde=cli.nouv_solde;
    printf("Somme:");
    scanf("%lf",&somme);
    if(somme<0.0){
        somme=-somme;
    }
    if(choix=='V'){
        cli.der_operation='V';
        cli.anc_solde+=somme;
    }
    else{
        cli.der_operation='R';
        cli.nouv_solde-=somme;
    }
    getDate(cli.date);
    fseek(fich,-11*sizeof(CLIENT),SEEK_CUR);
    fwrite(&cli,sizeof(CLIENT),1,fich);
    printf("Succes !");
}

void menu(FILE *fic){  //main fonction
    char choix;
    do{
        printf("\n\n-Ajouter d'un nouveau client ..............:A\n");
        printf("-Consultation d'un compte client ..........:C\n");
        printf("-Lister tous les comptes des clients ......:L\n");
        printf("-Operation sur un compte client ...........:O\n");
        printf("-Quitter ..................................:Q\n");
        printf(".........................votre choix : ");
        rewind(stdin);/* vide le tampon avant une lecture */
        scanf("%c",&choix);
        switch(choix){
            case'a':
            case'A':
                ajout(fic);
                break;
            case'c':
            case'C':
                affiche(fic);
                break;
            case'l':
            case'L':
                lister(fic);
                break;
            case'o':
            case 'O':
                operation(fic);
                break;
            case'q':
            case'Q':
                printf("Bye!");
                break;
                
            default:
                printf("Erreur de saisie,recommencez....\n");
                
        }
    }
   while(choix!='q'&&choix!='Q');
}

//la fonction main
int main(){
    FILE *fich = NULL;
    printf("\t\t* MouhaSeck *");
    ouvrir(&fich,"BANKACCOUNT.txt");
    menu(fich);
    fermer(fich);
    return 0;
}
