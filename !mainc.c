#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define R 6371
#define PI 3.14

typedef struct stanica {
    int linija;
    double lat;
    double lon;
    char naziv[256];
    int zona;
} Stanica;

typedef struct node {
    Stanica s;
    struct node* prev;
    struct node* next;
} Node;

Node* createNode(Stanica stanica) {
    Node* newNode = (Node*)calloc(1, sizeof(Node));
    if (newNode == NULL) {
        printf("MEM_GRESKA");
        return NULL;
    }
    newNode->s = stanica;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void addNode(Node** head, Stanica stanica) {
    Node* newNode = createNode(stanica);
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* curr = *head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = newNode;
        newNode->prev = curr;
    }
}

int read(Node** head, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("DAT_GRESKA");
        return 1;
    }

    Stanica stn;
    char format[] = "%d!%[^!]!%lf!%lf!%d";
    while (fscanf(file, format, &stn.linija, stn.naziv, &stn.lat, &stn.lon, &stn.zona) == 5) {
        addNode(head, stn);
    }
    fclose(file);

    return 0;
}

double distance(double lat1, double lon1, double lat2, double lon2) {
    double radconst = PI / 180;
    lat1 *= radconst;
    lon1 *= radconst;
    lat2 *= radconst;
    lon2 *= radconst;

    double t1 = pow(sin((lat1 - lat2) / 2), 2);
    double t2 = pow(sin((lon1 - lon2) / 2), 2);

    return 2 * R * asin(sqrt(t1 + t2 * cos(lat1) * cos(lat2)));
}

void findStops(Node* stops, Node** resList, double plat, double plon, double radius) {
    Node* curr = stops;
    while (curr) {
        if (distance(plat, plon, curr->s.lat, curr->s.lon) <= radius) {
            addNode(resList, curr->s);
        }
        curr = curr->next;
    }
}

void writeResults(const char* filename, Node* resList) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("DAT_GRESKA");
        return;
    }

    Node* curr = resList;
    while (curr) {

        fprintf(file, "%d!%s!%lf!%lf!1\n", curr->s.linija, curr->s.naziv, curr->s.lat, 6, curr->s.lon, 6);
        curr = curr->next;
    }
    fclose(file);
}

void ukloniDuplikate(Node* head){
    Node* a = head;
    Node *b;

    for (; a; a = a->next){
        for (b = a -> next; b; b = b->next){
            if ((strcmp(a->s.naziv, b->s.naziv) == 0) && a->s.linija == b->s.linija){
                //ukloni b
                Node *prev = b->prev;
                Node *next = b->next;
                prev->next = b->next;
                if (next != NULL) {
                    next->prev = prev;
                }
                free(b);
                b = next;
            }
        }
    }
}

int cmp(Node* node1, Node* node2, double plat, double plon) {
    double dist1 = distance(node1->s.lat, node1->s.lon, plat, plon);
    double dist2 = distance(node2->s.lat, node2->s.lon, plat, plon);

    if (dist1 < dist2) {
        return -1;
    } else if (dist1 > dist2) {
        return 1;
    } else {
        return 0;
    }
}

void sortList(Node* head, double plat, double plon) {
    Node* a;
    Node* b;
    Stanica temp;

    for (a = head; a != NULL; a = a->next) {
        for (b = a->next; b != NULL; b = b->next) {
            if (cmp(a, b, plat, plon) > 0) {
                temp = a->s;
                a->s = b->s;
                b->s = temp;
            }
        }
    }
}

void freeList(Node* head) {
    Node* curr = head;
    while (curr) {
        Node* temp = curr;
        curr = curr->next;
        free(temp);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 7) {
        printf("ARG_GRESKA");
        return 0;
    }

    const char* output = argv[1];
    const char* smer = argv[2];
    double plat = atof(argv[3]);
    double plon = atof(argv[4]);
    double radius = atof(argv[5]) / 1000;

    Node* stops = NULL;
    Node* resList = NULL;

    for (int i = 6; i < argc; i++) {
        char filename[256];
        sprintf(filename, "%s_%s.txt", argv[i], smer);
        if (read(&stops, filename) == 1){
            return 0;
        }
    }

    findStops(stops, &resList, plat, plon, radius);
    ukloniDuplikate(resList);
    sortList(resList, plat, plon);
    writeResults(output, resList);

    freeList(stops);
    freeList(resList);

    return 0;
}
