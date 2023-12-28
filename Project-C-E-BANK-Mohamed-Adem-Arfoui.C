#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct histo {
    int content;
    struct histo *next;
} histo;

typedef struct {
    histo *head, *tail;
} operation;

operation *queue_new(void) {
    operation *q = malloc(sizeof(operation));
    if (!q) return NULL;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

int histo_add(operation *q, int op) {
    histo *e = malloc(sizeof(histo));
    if (!e) return -1;
    e->content = op;
    e->next = NULL;
    if (q->tail) q->tail->next = e;
    else q->head = e;
    q->tail = e;
    return 0;
}

typedef struct compte {
    char type_compte[30];
    operation *hist;
    float solde;
} compte;

typedef struct {
    char id[7];
    char nom[50];
    char prenom[50];
    char code[8];
    compte *cpmt;
} client;

typedef struct tableclient {
    client *cl;
    struct tableclient *suiv;
} tableclient;

tableclient *chercher_client(tableclient *tcl, char *idcl) {
    while (tcl != NULL) {
        if (strcmp(tcl->cl->id, idcl) == 0) {
            return tcl;
        }
        tcl = tcl->suiv;
    }
    return NULL;
}

void ajouter_client(tableclient **tcl) {
    tableclient *newNode = malloc(sizeof(tableclient));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed for tableclient.\n");
        exit(EXIT_FAILURE);
    }

    newNode->cl = malloc(sizeof(client));
    if (newNode->cl == NULL) {
        printf("Error: Memory allocation failed for client.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    printf("Enter your ID: ");
    scanf("%6s", newNode->cl->id);
    printf("Enter your name: ");
    scanf("%49s", newNode->cl->nom);
    printf("Enter your first name: ");
    scanf("%49s", newNode->cl->prenom);

    newNode->cl->cpmt = malloc(sizeof(compte));
    if (newNode->cl->cpmt == NULL) {
        printf("Error: Memory allocation failed for compte.\n");
        free(newNode->cl);
        free(newNode);
        exit(EXIT_FAILURE);
    }
    printf("Enter your account type: ");
    scanf("%29s", newNode->cl->cpmt->type_compte );
    newNode->cl->cpmt->hist = queue_new();
    printf("Enter your initial deposit: ");
    float montant;
    scanf("%f", &montant);
    newNode->cl->cpmt->solde = montant;
    histo_add(newNode->cl->cpmt->hist, montant);
    newNode->suiv = *tcl;
    *tcl = newNode;
}

void historique_compte(char *idclient, tableclient *tcl) {
    tableclient *p = chercher_client(tcl, idclient);
    if (p == NULL) {
        printf("Client not found.\n");
        return;
    }

    histo *current = p->cl->cpmt->hist->head;
    while (current != NULL) {
        printf("%d\n", current->content);
        current = current->next;
    }
}

void supprimerCompte(char *idclient, tableclient **tcl, char *codesecret) {
    if (*tcl == NULL) {
        printf("No clients to delete.\n");
        return;
    }

    tableclient *current = *tcl;
    tableclient *prev = NULL;

    while (current != NULL && strcmp(current->cl->id, idclient) != 0) {
        prev = current;
        current = current->suiv;
    }

    if (current == NULL) {
        printf("Client not found.\n");
        return;
    }

    

    if (prev == NULL) {
        *tcl = current->suiv;
    } else {
        prev->suiv = current->suiv;
    }

    free(current->cl->cpmt->hist); // Assuming a function to free the history list
    free(current->cl->cpmt);
    free(current->cl);
    free(current);
}

void afficherDetailsCompte(char *idc, tableclient *tbcl) {
    tableclient *p = chercher_client(tbcl, idc);
    if (p == NULL) {
        printf("Client not found.\n");
        return;
    }

    printf("ID: %s\n", p->cl->id);
    printf("Name: %s %s\n", p->cl->nom, p->cl->prenom);
    printf("Account Type: %s\n", p->cl->cpmt->type_compte);
    printf("Balance: %.2f\n", p->cl->cpmt->solde);
}

void afficherListeClients(tableclient *cl) {
    if (cl == NULL) {
        printf("No clients to display.\n");
        return;
    }

    while (cl != NULL) {
        printf("ID: %s, Name: %s %s\n", cl->cl->id, cl->cl->nom, cl->cl->prenom);
        cl = cl->suiv;
    }
}
// Function to deposit amount to a client's account
void versement_Compte(char *idclient, float versement, tableclient *tcl) {
    tableclient *p = chercher_client(tcl, idclient);
    if (p == NULL) {
        printf("Client not found.\n");
        return;
    }
    p->cl->cpmt->solde += versement;
    histo_add(p->cl->cpmt->hist, versement);
}

// Function to withdraw amount from a client's account
void retrait_Compte(char *idclient, float retrait, tableclient *tcl) {
    tableclient *p = chercher_client(tcl, idclient);
    if (p == NULL) {
        printf("Client not found.\n");
        return;
    }
    if (p->cl->cpmt->solde < retrait) {
        printf("Insufficient balance.\n");
        return;
    }
    p->cl->cpmt->solde -= retrait;
    histo_add(p->cl->cpmt->hist, -retrait);
}

void Menu() {
    printf("\n\t\t __ Bienvenue Chez E-BANK ___  \n");
    printf("\t\t========WELCOME========\n\n");
    printf("\n  ========================\n\t>< MENU ><\n  ========================\n\n");
    printf("[1]. Creer un nouveau compte.\n");
    printf("[2]. Versement.\n");
    printf("[3]. Retrait.\n");
    printf("[4]. Supprimer un compte existant.\n");
    printf("[5]. Afficher les details d un compte existant.\n");
    printf("[6]. Afficher la liste des clients.\n");
    printf("[7]. Afficher Mon historique;\n");
    printf("[0]. Quitter\n\n");
    printf("  ========================\n\n");
    printf("Choisissez une option : ");
}

int main() {
    tableclient *table_client = NULL;
    int choix;
    char id_client[7];
    float montant;
    char code[8];

    do {
        Menu();
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajouter_client(&table_client);
                break;
            case 2:
                printf("Enter Client ID: ");
                scanf("%6s", id_client);
                printf("Enter Deposit Amount: ");
                scanf("%f", &montant);
                versement_Compte(id_client, montant, table_client);
                break;
            case 3:
                printf("Enter Client ID: ");
                scanf("%6s", id_client);
                printf("Enter Withdrawal Amount: ");
                scanf("%f", &montant);
                retrait_Compte(id_client, montant, table_client);
                break;
            case 4:
                printf("Enter Client ID: ");
                scanf("%6s", id_client);
                printf("Enter Secret Code: ");
                scanf("%7s", code);
                supprimerCompte(id_client, &table_client, code);
                break;
            case 5:
                printf("Enter Client ID: ");
                scanf("%6s", id_client);
                afficherDetailsCompte(id_client, table_client);
                break;
            case 6:
                afficherListeClients(table_client);
                break;
            case 7:
                printf("Enter Client ID: ");
                scanf("%6s", id_client);
                historique_compte(id_client, table_client);
                break;
            case 0:
                printf("Au revoir!\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix != 0);

    // Free memory before exiting
    // ...

    return 0;
}
