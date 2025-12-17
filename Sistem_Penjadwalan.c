#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global Config: 1 = AVL Mode (Auto Balance), 0 = BST Mode (Bisa Miring)
int isAVL = 1; 

typedef struct {
    long long timeID; 
    char title[50];
    char description[100];
    int durationMinutes;
} Event;

typedef struct Node {
    Event data;
    struct Node *left;
    struct Node *right;
    int height;
} Node;

// ------------------- Helper Functions ------------------ //

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getHeight(Node *N) {
    if (N == NULL) return 0;
    return N->height;
}

int getBalance(Node *N) {
    if (N == NULL) return 0;
    int leftH = (N->left) ? N->left->height : 0;
    int rightH = (N->right) ? N->right->height : 0;
    return leftH - rightH;
}

Node* createNode(long long timeID, char* title, char* desc, int duration) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data.timeID = timeID;
    strcpy(newNode->data.title, title);
    strcpy(newNode->data.description, desc);
    newNode->data.durationMinutes = duration;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

// ------------------- Rotasi AVL ------------------ //

Node* rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    
    return x;
}

Node* leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    
    return y;
}

// ------------------- Core Functions ------------------ //

Node* minValueNode(Node* node) {
    Node* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

Node* insert(Node* node, long long timeID, char* title, char* desc, int duration) { 
    // 1. Insert Normal
    if (node == NULL) return createNode(timeID, title, desc, duration); 
    
    if (timeID < node->data.timeID)
        node->left = insert(node->left, timeID, title, desc, duration); 
    else if (timeID > node->data.timeID)
        node->right = insert(node->right, timeID, title, desc, duration); 
    else 
        return node; // Duplicate ID

    // 2. Update Height
    node->height = 1 + max(getHeight(node->left), getHeight(node->right)); 

    // --- LOGIKA PEMISAH (Mode BST Stop Disini) ---
    if (isAVL == 0) return node; 

    // 3. Balancing AVL
    int balance = getBalance(node); 

    // LL Case
    if (balance > 1 && timeID < node->left->data.timeID) 
        return rightRotate(node); 
    // RR Case
    if (balance < -1 && timeID > node->right->data.timeID) 
        return leftRotate(node); 
    // LR Case
    if (balance > 1 && timeID > node->left->data.timeID) { 
        node->left = leftRotate(node->left); 
        return rightRotate(node); 
    } 
    // RL Case
    if (balance < -1 && timeID < node->right->data.timeID) { 
        node->right = rightRotate(node->right); 
        return leftRotate(node); 
    } 
    return node;
}

Node* deleteNode(Node *node, long long timeID) {
    if (!node) return node;

    // 1. Pencarian Node
    if (timeID < node->data.timeID) 
        node->left = deleteNode(node->left, timeID);
    else if (timeID > node->data.timeID) 
        node->right = deleteNode(node->right, timeID);
    else {
        // Node Ditemukan
        // Kasus 1: Node dengan satu anak atau tanpa anak
        if ((node->left == NULL) || (node->right == NULL)) {
            Node *temp = node->left ? node->left : node->right;
            
            // Kasus Tanpa Anak
            if (temp == NULL) {
                temp = node;
                node = NULL;
            } else { 
                // Kasus Satu Anak
                *node = *temp; // Copy isi anak ke parent
            }
            free(temp);
        } else {
            // Kasus 2: Node dengan dua anak
            Node *temp = minValueNode(node->right);
            node->data = temp->data;
            node->right = deleteNode(node->right, temp->data.timeID);
        }
    }

    if (node == NULL) return node;

    // 2. Update Height
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // --- LOGIKA PEMISAH (Mode BST Stop Disini) ---
    if (isAVL == 0) return node;

    // 3. Balancing AVL
    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0) return rightRotate(node);
    if (balance > 1 && getBalance(node->left) < 0) { 
        node->left = leftRotate(node->left); 
        return rightRotate(node); 
    }
    if (balance < -1 && getBalance(node->right) <= 0) return leftRotate(node);
    if (balance < -1 && getBalance(node->right) > 0) { 
        node->right = rightRotate(node->right); 
        return leftRotate(node); 
    }
    return node;
}

Node* search(Node* root, long long timeID) { 
    if (root == NULL || root->data.timeID == timeID) return root; 
    if (timeID < root->data.timeID) return search(root->left, timeID); 
    return search(root->right, timeID); 
} 

// ------------------- VISUALISASI & OUTPUT ------------------ //

void printTreeVisual(Node *root, int space) {
    int COUNT = 10; 
    if (root == NULL) return;
    space += COUNT;
    
    // Anak Kanan (Cetak di Atas)
    printTreeVisual(root->right, space);
    
    // Node Saat Ini
    printf("\n");
    for (int i = COUNT; i < space; i++) printf(" ");
    printf("[%lld] H:%d\n", root->data.timeID % 10000, root->height);
    
    // Anak Kiri (Cetak di Bawah)
    printTreeVisual(root->left, space);
} 

void printEvent(Event e) {
    printf("   -> TimeID: %lld | %s (%d min)\n", e.timeID, e.title, e.durationMinutes);
}

void inorderTraversal(Node* root) { 
    if (root != NULL) { 
        inorderTraversal(root->left); 
        printEvent(root->data);
        inorderTraversal(root->right); 
    } 
}

void freeTree(Node* N) {
    if ( N != NULL) {
        freeTree(N->left);
        freeTree(N->right);
        free(N);
    }
}

// ------------------- MAIN ------------------ //
int main () {
    Node *root = NULL;
    int pilihan, modePilihan;
    long long timeID;
    char title[50], desc[100];
    int durasiMenit;
    
    srand(time(0)); 

    printf("===================================\n");
    printf("     SISTEM PENJADWALAN ACARA      \n");
    printf("===================================\n");
    printf("Pilih Metode Struktur Data:\n");
    printf("1. BST Biasa (Tanpa Balancing)\n");
    printf("2. AVL Tree (Auto Balancing)\n");
    printf("Pilihan: ");
    scanf("%d", &modePilihan);
    getchar();

    if (modePilihan == 1) { isAVL = 0; printf("\n[INFO] Mode: BST BIASA\n"); } 
    else { isAVL = 1; printf("\n[INFO] Mode: AVL TREE\n"); 

    // --- GENERATE DATA DINAMIS ---
    printf("\nSedang meng-generate 10 Data Acara Dinamis (Acak)...\n");
    const char *kegiatan[] = {"Rapat", "Kuliah", "Makan", "Tidur", "Gym", "Coding", "Nonton", "Main", "Belanja", "Jogging"};
    
    for (int i = 0; i < 10; i++) {
        long long randomTime = 202512010000 + ((rand() % 30) * 100) + (rand() % 2400);
        char randomTitle[50];
        sprintf(randomTitle, "%s #%d", kegiatan[rand() % 10], rand() % 100);
        int randomDurasi = 30 + (rand() % 151);
        root = insert(root, randomTime, randomTitle, "Generated by System", randomDurasi);
    }
    printf("[OK] 10 Data berhasil digenerate.\n");

    while (1) {
        printf("\n============= MENU (%s) ==============\n", isAVL ? "AVL" : "BST");
        printf("1. Tambah Acara\n");
        printf("2. Hapus Acara\n");
        printf("3. Cari Acara\n");
        printf("4. Lihat Daftar \n");
        printf("5. Visualisasi\n");
        printf("0. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        getchar();

        switch(pilihan) {
            case 1:
                printf("TimeID: "); scanf("%lld", &timeID); getchar();
                printf("Judul: "); fgets(title, 50, stdin); title[strcspn(title, "\n")] = 0;
                printf("Deskripsi: "); fgets(desc, 100, stdin); desc[strcspn(desc, "\n")] = 0;
                printf("Durasi: "); scanf("%d", &durasiMenit);
                root = insert(root, timeID, title, desc, durasiMenit);
                printf("Sukses!\n");
                break;
            case 2:
                printf("TimeID hapus: "); scanf("%lld", &timeID);
                if(search(root, timeID)) {
                    root = deleteNode(root, timeID);
                    printf("Terhapus.\n");
                } else printf("Tidak ditemukan.\n");
                break;
            case 3:
                printf("Cari TimeID: "); scanf("%lld", &timeID);
                Node* res = search(root, timeID);
                if(res) printf("Ketemu: %s (H:%d)\n", res->data.title, res->height);
                else printf("Tidak ketemu.\n");
                break;
            case 4:
                if(!root) printf("Kosong.\n");
                else {
                    printf("\n--- Daftar Jadwal ---\n");
                    inorderTraversal(root);
                }
                break;
            case 5:
                if(!root) printf("Tree Kosong.\n");
                else {
                    printf("\n=== VISUALISASI ===\n");
                    printTreeVisual(root, 0);
                }
                break;
            case 0:
                freeTree(root);
                return 0;
        }
    }
    return 0;
}