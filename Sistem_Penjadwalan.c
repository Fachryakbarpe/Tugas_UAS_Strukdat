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

int main () {
    Node *root = NULL;
    int pilihan;
    long long timeID;
    char title[50], desc[100];
    int durasiMenit;

    printf("===================================");
    printf("   Penjadwalan dengan AVL Tree     ");
    printf("===================================");

    while (1) {
        printf("=============Pilihan==============");
        printf("1. Tambah acara         \n");
        printf("2. Hapus acara          \n");
        printf("3. Cari acara           \n");
        printf("4. Tampilkan semua acara\n");
        printf("0. Keluar               \n");
        printf("===================================");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        getchar(); // Mengecualikan white space

        switch(pilihan) {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 0:
                break;
        }
    }
}