#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global Config: 1 = AVL Mode (Auto Balance), 0 = BST Mode 
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

void printTableHeader() {
    printf("+----------+----------------------+------------------------------+----------+\n");
    printf("| TimeID   | Title                | Description                  | Duration |\n");
    printf("+----------+----------------------+------------------------------+----------+\n");
}

void printTableFooter() {
    printf("+----------+----------------------+------------------------------+----------+\n");
}

void printDescriptionWrapped(const char *desc) {
    int len = strlen(desc);
    int descWidth = 28;
    
    if (len <= descWidth) {
        // Short description - print as is
        printf("%-*s", descWidth, desc);
    } else {
        // Long description - truncate and add "..."
        printf("%-*.*s...", descWidth - 3, descWidth - 3, desc);
    }
}

// ------------------- AVL Rotation ------------------ //

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
    // 1. Normal Insert
    if (node == NULL) return createNode(timeID, title, desc, duration); 
    
    if (timeID < node->data.timeID)
        node->left = insert(node->left, timeID, title, desc, duration); 
    else if (timeID > node->data.timeID)
        node->right = insert(node->right, timeID, title, desc, duration); 
    else 
        return node; // Duplicate ID

    // 2. Update Height
    node->height = 1 + max(getHeight(node->left), getHeight(node->right)); 

    // --- Separataion Logic (Mode BST Stop) ---
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

    // 1. Node Search
    if (timeID < node->data.timeID) 
        node->left = deleteNode(node->left, timeID);
    else if (timeID > node->data.timeID) 
        node->right = deleteNode(node->right, timeID);
    else {
        // Node Found
        // Case 1: Node with a child or no child
        if ((node->left == NULL) || (node->right == NULL)) {
            Node *temp = node->left ? node->left : node->right;
            
            // No child case
            if (temp == NULL) {
                temp = node;
                node = NULL;
            } else { 
                // One child case
                *node = *temp; // Copy isi anak ke parent
            }
            free(temp);
        } else {
            // Case 2: Node with two children
            Node *temp = minValueNode(node->right);
            node->data = temp->data;
            node->right = deleteNode(node->right, temp->data.timeID);
        }
    }

    if (node == NULL) return node;

    // 2. Update Height
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // --- Separataion Logic (Mode BST Stop) ---
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

// ------------------- Dinamic Database CSV -------------------

// Prototype saveInorderToCSV
void saveInorderToCSV(FILE* file, Node* node);

void saveTreeToCSV(const char* filename, Node* root) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Gagal buat %s\n", filename);
        return;
    }
    
    fprintf(file, "timeID,title,description,duration\n");
    saveInorderToCSV(file, root);
    fclose(file);
}

void saveInorderToCSV(FILE* file, Node* node) {
    if (!node) return;
    saveInorderToCSV(file, node->left);
    fprintf(file, "%lld,\"%s\",\"%s\",%d\n", 
            node->data.timeID, node->data.title, 
            node->data.description, node->data.durationMinutes);
    saveInorderToCSV(file, node->right);
}

Node* loadCSVToTree(Node* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No %s found, start fresh\n", filename);
        return root;
    }
    
    char line[300];
    fgets(line, sizeof(line), file); // skip header
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        char* token = strtok(line, ",");
        long long timeID = token ? atoll(token) : 0;
        
        // Title
        token = strtok(NULL, ","); 
        char title[50] = "";
        if (token) {
            strncpy(title, token + 1, 48);          // lewati " depan
            title[49] = '\0';
            size_t len = strlen(title);
            if (len > 0 && title[len - 1] == '"')   // buang " belakang
                title[len - 1] = '\0';
        }

        // Description
        token = strtok(NULL, ","); 
        char desc[100] = "";
        if (token) {
            strncpy(desc, token + 1, 98);
            desc[99] = '\0';
            size_t len = strlen(desc);
            if (len > 0 && desc[len - 1] == '"')
                desc[len - 1] = '\0';
        }

        
        token = strtok(NULL, ","); 
        int duration = token ? atoi(token) : 0;
        
        root = insert(root, timeID, title, desc, duration);
    }
    fclose(file);
    printf("Loaded CSV to %s tree\n", isAVL ? "AVL" : "BST");
    return root;
}

// Auto-save wrapper
Node* insertWithCSV(Node* node, long long timeID, char* title, char* desc, int duration) {
    Node* result = insert(node, timeID, title, desc, duration);
    saveTreeToCSV("ScheduleData.csv", result);
    return result;
}

// ------------------- VISUALIZATION & OUTPUT ------------------ //

void printTreeVisual(Node *root, int space) {
    int COUNT = 10; 
    if (root == NULL) return;
    space += COUNT;
    
    // Right child (Print above)
    printTreeVisual(root->right, space);
    
    // Current node
    printf("\n");
    for (int i = COUNT; i < space; i++) printf(" ");
    printf("[%lld] H:%d\n", root->data.timeID % 10000, root->height);
    
    // Left child (Print below)
    printTreeVisual(root->left, space);
} 

void printEvent(Event e) {
    printf("| %-*lld | %-*s | ", 8, e.timeID, 20, e.title);
    printDescriptionWrapped(e.description);
    printf(" | %-8d |\n", e.durationMinutes);
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
    int choice, ChoiceMode;
    long long timeID;
    char title[50], desc[100];
    int duration;

    // Data read from here
    root = loadCSVToTree(root, "ScheduleData.csv");
    
    srand(time(0)); 

    printf("===================================\n");
    printf("     Event Scheduling System      \n");
    printf("===================================\n");
    printf("Choose Data Structure Method:\n");
    printf("1. BST (No Balancing)\n");
    printf("2. AVL Tree (Auto Balancing)\n");
    printf("choice: ");
    scanf("%d", &ChoiceMode);
    getchar();

    if (ChoiceMode == 1) { isAVL = 0; printf("\n[INFO] Mode: BST\n"); } 
    else { isAVL = 1; printf("\n[INFO] Mode: AVL TREE\n"); }

    while (1) {
        printf("\n============= MENU (%s) ==============\n", isAVL ? "AVL" : "BST");
        printf("1. Add Event\n");
        printf("2. Delete Event\n");
        printf("3. Find Event\n");
        printf("4. List Lookup \n");
        printf("5. Visualization\n");
        printf("0. Exit\n");
        printf("choice: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1:
                printf("TimeID: "); scanf("%lld", &timeID); getchar();
                printf("Title: "); fgets(title, 50, stdin); title[strcspn(title, "\n")] = 0;
                printf("Desc: "); fgets(desc, 100, stdin); desc[strcspn(desc, "\n")] = 0;
                printf("Duration: "); scanf("%d", &duration); getchar();
                root = insertWithCSV(root, timeID, title, desc, duration);
                printf("Event created!");
                break;
            case 2:
                printf("Delete TimeID: "); scanf("%lld", &timeID); getchar();
                if (search(root, timeID)) {
                    root = deleteNode(root, timeID);
                    saveTreeToCSV("ScheduleData.csv", root);
                    printf("Deletetion success!\n");
                } else printf("Not Found\n");
                break;
            case 3:
                printf("Find: "); scanf("%lld", &timeID); getchar();
                Node* found = search(root, timeID);
                if (found) {
                    printf("\n--- Event Details ---\n");
                    printf("TimeID: %lld\n", found->data.timeID);
                    printf("Title: %s\n", found->data.title);
                    printf("Description: %s\n", found->data.description);
                    printf("Duration: %d minutes\n", found->data.durationMinutes);
                } else {
                    printf("No Data\n");
                }
                break;
                
            case 4:
                if (!root) printf("Empty\n");
                else {
                    printf("\n--- List (Sorted) ---\n");
                    printTableHeader();
                    inorderTraversal(root);
                    printTableFooter();
                }
                break;
            case 5:
                if (!root) printf("Empty \n");
                else printTreeVisual(root, 0);
                break;
            case 0:
                saveTreeToCSV("ScheduleData.csv", root);
                freeTree(root);
                printf("Good bye!\n");
                return 0;
        }
    }
    return 0;
}