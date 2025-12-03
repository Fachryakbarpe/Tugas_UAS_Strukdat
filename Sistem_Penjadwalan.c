#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// ------------------- Fungsi Pendukung ------------------ //

int max(int a, int b) {
    return (a > b) ? a : b;
}

// Jika ada node, return N->height
// Jika node kosong, return NULL
// Fungsi ini menentukan tinggi suatu tree, yang menentukan NULL case
int getHeight(Node *N) {
    if (N == NULL)
        return 0;
    return N-> height;
}

// Fungsi ini menentukan rotasi suatu tree
// berat cabang kiri - cabang kanan, tree tidak dapat berat sebelah
int getBalance(Node *N){
    if (N == NULL)
        return 0;
    return N->left->height - N->right->height;
}

// Membuat Node baru
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
 
Node* rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right; // T2 adalah pointer temporer yang digunakan 
                         // Untuk memindahkan Node

    // Lakukan Rotasi
    x->right = y;
    y->left = T2;

    // Perbarui Tinggi
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    // Kembalikan root baru
    return x;
}

// Rotasi Kiri (Left Rotation)
Node* leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left; 

    // Lakukan Rotasi
    y->left = x;
    x->right = T2;

    // Perbarui Tinggi
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    // Kembalikan root baru
    return y;
}

// ------------------- Fungsi Utama AVL ------------------ //

// Fungsi untuk mencari node dengan nilai minimum
// Dibutuhkan untuk menghapus node
Node* minValueNode(Node* node) {
    Node* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Fungsi untuk menyisipkan node baru ke dalam AVL Tree 
Node* insert(Node* node, long long timeID, char* title, char* desc, int duration) { 
    if (node == NULL) { 
        return createNode(timeID, title, desc, duration); 
    } 
 
    if (timeID < node->data.timeID) { 
        node->left = insert(node->left, timeID, title, desc, duration); 
    } else if (timeID > node->data.timeID) { 
        node->right = insert(node->right, timeID, title, desc, duration); 
    } else { 
        return node;
    } 
 
    node->height = 1 + max(getHeight(node->left), getHeight(node->right)); 
    int balance = getBalance(node); 
 
    // Control Flow tidak seimbang
 
    // Kasus Kiri-Kiri (LL)
    if (balance > 1 && timeID < node->left->data.timeID) 
        return rightRotate(node); 
 
    // Kasus Kanan-Kanan (RR)
    if (balance < -1 && timeID > node->right->data.timeID) 
        return leftRotate(node); 
 
    // Kasus Kiri-Kanan (LR)
    if (balance > 1 && timeID > node->left->data.timeID) { 
        node->left = leftRotate(node->left); 
        return rightRotate(node); 
    } 
 
    // Kasus Kanan-Kiri (RL)
    if (balance < -1 && timeID < node->right->data.timeID) { 
        node->right = rightRotate(node->right); 
        return leftRotate(node); 
    } 
    return node;
}

// Fungsi untuk menghapus node dari AVL tree
Node* deleteNode(Node *node, long long timeID) {
    if (!node)
        return node;

    if (timeID < node->data.timeID)
        node->left = deleteNode(node->left, timeID);
    else if (timeID > node->data.timeID)
        node->right = deleteNode(node->right, timeID);
    else {
        // Node with one child or no child
        if (!node->left || !node->right) {
            Node *temp = node->left ? node->left : node->right;
            
            if (!temp) {
                temp = node;
                node = NULL;
            } else {
                *node = *temp;
            }
            free(temp);
        } else {
            // Node with two children
            Node *temp = minValueNode(node->right);
            node->data = temp->data;
            node->right = deleteNode(node->right, temp->data.timeID);
        }
    }

    if (!node)
        return node;

    // Update height
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // Get balance factor
    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && getBalance(node->left) >= 0)
        return rightRotate(node);

    // Left Right Case
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && getBalance(node->right) <= 0)
        return leftRotate(node);

    // Right Left Case
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Fungsi untuk mencari node dalam AVL Tree
Node* search(Node* root, long long timeID) { 
    if (root == NULL || root->data.timeID == timeID) { 
        return root; 
    } 
 
    if (timeID < root->data.timeID) { 
        return search(root->left, timeID); 
    } 

    return search(root->right, timeID); 
} 
 
// Fungsi untuk menampilkan data Event
void printEvent(Event e) {
    printf("  -> TimeID: %lld\n", e.timeID);
    printf("  -> Title: %s\n", e.title);
    printf("  -> Description: %s\n", e.description);
    printf("  -> Duration: %d minutes\n", e.durationMinutes);
}

// Fungsi untuk inorder traversal 
void inorderTraversal(Node* root) { 
    if (root != NULL) { 
        inorderTraversal(root->left); 
        printf("--- Event ---\n");
        printEvent(root->data);
        printf("  -> Height: %d\n", root->height);
        inorderTraversal(root->right); 
    } 
}

// Fungsi untuk membersihkan memory
void freeTree(Node* N) {
    if ( N != NULL) {
        freeTree(N->left);
        freeTree(N->right);
        free(N);
    }
}

int main () {
    Node *root = NULL;
    int pilihan;
    long long timeID;
    char title[50], desc[100];
    int durasiMenit;

    // Data awal
    Event dataAwal[] = { 
    {202512020700, "Basdat", "Temporer", 150},
    {202512020930, "BelaNegara", "Temporer", 150},
    {202512021200, "Istirahat", "Temporer", 60},
    {202512021500, "Kerkel", "Temporer", 120},
    {202512021700, "Istirahat", "Temporer", 120},
    {202512021900, "Belajar", "Temporer", 150},
    {202512030930, "Stakom", "Temporer", 150},
    {202512031300, "Pemlan", "Temporer", 150},
    {202512031530, "Kerkel", "Temporer", 120},
    {202512031900, "Belajar", "Temporer", 150},
    };
    
    int jumlahAwal = sizeof(dataAwal)/sizeof(dataAwal[0]);

    for (int i = 0; i < jumlahAwal; i++) {
        root = insert   (root, dataAwal[i].timeID,
                        dataAwal[i].title,
                        dataAwal[i].description,
                        dataAwal[i].durationMinutes);
    };
    
    printf("===================================\n");
    printf("   Penjadwalan dengan AVL Tree     \n");
    printf("===================================\n");

    while (1) {
        printf("=============Pilihan==============\n");
        printf("1. Tambah acara         \n");
        printf("2. Hapus acara          \n");
        printf("3. Cari acara           \n");
        printf("4. Tampilkan semua acara\n");
        printf("0. Keluar               \n");
        printf("===================================\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        getchar(); // Mengecualikan white space

        switch(pilihan) {
            case 1:
                printf("\n====== Membuat acara baru ======\n");
                printf("Masukkan id dengan format YYYYMMDDJJMM, misal 202512040650\n");
                printf("untuk 4 Desember 2025, 06:50. TimeID: ");
                scanf("%lld", &timeID);
                getchar();

                printf("Masukkan judul acara: ");
                fgets(title, 50, stdin);
                title[strcspn(title, "\n")] = 0;
                
                printf("Masukkan deskripsi: ");
                fgets(desc, 100, stdin);
                desc[strcspn(desc, "\n")] = 0;
                
                printf("Masukkan durasi (menit): ");
                scanf("%d", &durasiMenit);
                
                root = insert(root, timeID, title, desc, durasiMenit);
                printf("\n Acara berhasil ditambahkan!\n");

                break;
            case 2:
                printf("\n====== Menghapus acara ======\n");
                printf("Masukkan TimeID acara yang akan dihapus: ");
                scanf("%lld", &timeID);
                
                if (search(root, timeID)) {
                    root = deleteNode(root, timeID);
                    printf("\nAcara berhasil dihapus!\n");
                } else {
                    printf("\nAcara tidak ditemukan!\n");
                }
                break;
            case 3:
                printf("\n====== Cari acara ======\n");
                printf("Masukkan TimeID: ");
                scanf("%lld", &timeID);
                
                Node *pencarian = search(root, timeID);
                if (pencarian) {
                    printf("\nAcara Ditemukan:\n");
                    printf("\n      Acara ditemukan     \n");
                    printEvent(pencarian->data);
                    printf("  -> Height: %d\n", pencarian->height);
                } else {
                    printf("\nAcara tidak ditemukan!\n");
                }
                break;
            case 4:
                printf("\n========================================\n");
                printf("       DAFTAR SEMUA Acara (Terurut)\n");
                printf("========================================\n");
                
                if (root == NULL) {
                    printf("\nBelum ada acara yang terjadwal.\n");
                } else {
                    inorderTraversal(root);
                }
                printf("========================================\n");
                break;
            case 0:
                printf("\nMembersihkan memory dan keluar...\n");
                freeTree(root);
                printf("Terima kasih telah menggunakan sistem penjadwalan!\n\n");
                return 0;
                break;
        }
    }
    return 0;
}