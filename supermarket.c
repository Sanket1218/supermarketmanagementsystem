#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#else
#include <unistd.h>
#endif

#define MAX_ITEMS 100
#define GST_RATE 0.05
#define DISCOUNT_THRESHOLD 1000
#define DISCOUNT_RATE 0.10
#define CATEGORY_COUNT 5

char categories[CATEGORY_COUNT][30] = {
    "Groceries",
    "Mobiles",
    "Electronics",
    "Snacks",
    "Personal Care"
};

struct Item {
    int id;
    char name[50];
    float price;
    int stock;
    char category[30];
};

struct Sale {
    int billID;
    float totalAmount;
    char date[20];
};

struct CartItem {
    int id;
    char name[50];
    float price;
    int quantity;
};

struct Item inventory[MAX_ITEMS];
struct Sale sales[MAX_ITEMS];
struct CartItem cart[MAX_ITEMS];
int itemCount = 0, saleCount = 0, cartCount = 0;

// Function prototypes
void addItem();
void displayItems();
void generateBill();
void searchItem();
void lowStockAlert();
void saveData();
void loadData();
void showSalesReport();
void addToCart();
void spinTheWheel(float *total);

// Utility: Generate unique Bill ID
int generateBillID() {
    return 1000 + saleCount + 1;
}

// Utility: Current date/time
void getDateTime(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d-%02d-%04d %02d:%02d",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
}

// Add new item
void addItem() {
    struct Item newItem;

    printf("\nEnter Item ID: ");
    scanf("%d", &newItem.id);
    getchar(); // clear newline

    printf("Enter Item Name: ");
    fgets(newItem.name, sizeof(newItem.name), stdin);
    newItem.name[strcspn(newItem.name, "\n")] = 0;

    printf("Enter Price: ");
    scanf("%f", &newItem.price);

    printf("Enter Stock Quantity: ");
    scanf("%d", &newItem.stock);

    // Category menu
    printf("\nSelect Category:\n");
    for (int i = 0; i < CATEGORY_COUNT; i++) {
        printf("%d. %s\n", i + 1, categories[i]);
    }

    int catChoice;
    printf("Enter choice (1-%d): ", CATEGORY_COUNT);
    scanf("%d", &catChoice);

    if (catChoice < 1 || catChoice > CATEGORY_COUNT) {
        strcpy(newItem.category, "Groceries");
    } else {
        strcpy(newItem.category, categories[catChoice - 1]);
    }

    inventory[itemCount++] = newItem;
    printf("\n Item added successfully under category '%s'!\n", newItem.category);
    lowStockAlert();
}

// Display items
void displayItems() {
    if (itemCount == 0) {
        printf("\nNo items available!\n");
        return;
    }

    printf("\n---- INVENTORY ----\n");
    printf("ID\tName\t\tCategory\tPrice\tStock\n");
    for (int i = 0; i < itemCount; i++) {
        printf("%d\t%-10s\t%-10s\t%.2f\t%d\n",
               inventory[i].id, inventory[i].name,
               inventory[i].category, inventory[i].price,
               inventory[i].stock);
    }
    lowStockAlert();
}

// Low stock alert
void lowStockAlert() {
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].stock < 5) {
            printf("Low Stock: %s (ID: %d) only %d left!\n",
                   inventory[i].name, inventory[i].id, inventory[i].stock);
        }
    }
}

// Search item
void searchItem() {
    char key[50];
    printf("\nEnter name or category to search: ");
    scanf("%s", key);
    int found = 0;
    printf("\nID\tName\t\tCategory\tPrice\tStock\n");
    for (int i = 0; i < itemCount; i++) {
        if (strstr(inventory[i].name, key) || strstr(inventory[i].category, key)) {
            printf("%d\t%-10s\t%-10s\t%.2f\t%d\n",
                   inventory[i].id, inventory[i].name,
                   inventory[i].category, inventory[i].price,
                   inventory[i].stock);
            found = 1;
        }
    }
    if (!found) printf("No matching items found!\n");
}

// Add to cart
void addToCart() {
    int id, qty;
    char more;

    do {
        displayItems();

        printf("\nEnter Item ID to add to cart: ");
        scanf("%d", &id);

        int found = 0;
        for (int i = 0; i < itemCount; i++) {
            if (inventory[i].id == id) {
                found = 1;
                printf("Enter quantity: ");
                scanf("%d", &qty);

                if (qty > inventory[i].stock) {
                    printf("Only %d available!\n", inventory[i].stock);
                    break;
                }

                cart[cartCount].id = inventory[i].id;
                strcpy(cart[cartCount].name, inventory[i].name);
                cart[cartCount].price = inventory[i].price;
                cart[cartCount].quantity = qty;
                cartCount++;

                printf("\n '%s' (x%d) added to cart!\n", inventory[i].name, qty);
                break;
            }
        }

        if (!found) printf("Item not found!\n");

        printf("\nDo you want to add more items? (y/n): ");
        getchar(); // clear newline
        scanf("%c", &more);

    } while (more == 'y' || more == 'Y');
}


//  Spin-the-Wheel Reward Game
void spinTheWheel(float *total) {
    char choice;
    printf("\n Do you want to play the Spin-the-Wheel game for a discount? (y/n): ");
    getchar();
    scanf("%c", &choice);

    if (choice == 'y' || choice == 'Y') {
        float entryFee = *total * 0.10;
        *total += entryFee;
        printf("\n Entry fee of 10%% (Rs. %.2f) added.\n", entryFee);

        int playAgain = 1;
        while (playAgain) {
            srand(time(NULL));
            int reward = rand() % 7 + 1;

            printf("\n Spinning the wheel...\n");
            for (int i = 0; i < 3; i++) { printf(". "); fflush(stdout); sleep(1); }
            printf("\nWheel stopped at option %d!\n", reward);

            switch (reward) {
                case 1: printf("You won ₹10 discount!\n"); *total -= 10; playAgain = 0; break;
                case 2: printf("You won ₹50 discount!\n"); *total -= 50; playAgain = 0; break;
                case 3: printf("You won 10%% off your total!\n"); *total -= *total * 0.10; playAgain = 0; break;
                case 4: printf("You won 50%% off your total!\n"); *total -= *total * 0.50; playAgain = 0; break;
                case 5: printf("You won 75%% off your total!\n"); *total -= *total * 0.75; playAgain = 0; break;
                case 6: printf("Lucky you! You get another spin!\n"); playAgain = 1; break;
                case 7: printf("Better luck next time!\n"); playAgain = 0; break;
            }
        }
        if (*total < 0) *total = 0;
        printf("\nYour new total after the game: ₹%.2f\n", *total);
    } else {
        printf("\nYou skipped the reward game.\n");
    }
}

void saveReceiptToFile(int billID, char *customerName, char *mobile, char *address, float subtotal, float discount, float gst, float total) {
    char filename[30];
    sprintf(filename, "Bill_%d.txt", billID); // file name based on bill ID
    FILE *fp = fopen(filename, "w");

    if (!fp) {
        printf(" Error saving bill receipt!\n");
        return;
    }

    fprintf(fp, "======== SUPERMARKET BILL ========\n");
    fprintf(fp, "Bill No: %d\nDate: ", billID);
    char datetime[20];
    getDateTime(datetime);
    fprintf(fp, "%s\n\n", datetime);

    fprintf(fp, "Customer: %s\n", customerName);
    fprintf(fp, "Mobile: %s\n", mobile);
    fprintf(fp, "Address: %s\n\n", address);

    fprintf(fp, "Item\tQty\tPrice\tTotal\n");
    for (int i = 0; i < cartCount; i++) {
        float cost = cart[i].price * cart[i].quantity;
        fprintf(fp, "%s\t%d\t%.2f\t%.2f\n", cart[i].name, cart[i].quantity, cart[i].price, cost);
    }

    fprintf(fp, "\nSubtotal: %.2f", subtotal);
    fprintf(fp, "\nDiscount: %.2f", discount);
    fprintf(fp, "\nGST (5%%): %.2f", gst);
    fprintf(fp, "\nTOTAL: %.2f\n", total);
    fprintf(fp, "===================================\n");
    fprintf(fp, "Thank you for shopping with us!\n");
    fclose(fp);

    printf("\n🧾 Bill saved as %s\n", filename);
}

// Generate bill
void generateBill() {
    if (cartCount == 0) {
        printf("\n Your cart is empty! Add items first.\n");
        return;
    }

    char customerName[50], mobile[15], address[100];
    printf("\nEnter Customer Name: ");
    getchar(); // clear newline
    fgets(customerName, sizeof(customerName), stdin);
    customerName[strcspn(customerName, "\n")] = 0;

    printf("Enter Mobile Number: ");
    fgets(mobile, sizeof(mobile), stdin);
    mobile[strcspn(mobile, "\n")] = 0;

    printf("Enter Address: ");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = 0;

    int billID = generateBillID();
    char datetime[20];
    getDateTime(datetime);
    float subtotal = 0;

    printf("\n------ BILL #%d ------\n", billID);
    printf("Customer: %s\nMobile: %s\nAddress: %s\n", customerName, mobile, address);
    printf("Date: %s\n\n", datetime);
    printf("Item\tQty\tPrice\tTotal\n");

    for (int i = 0; i < cartCount; i++) {
        float cost = cart[i].price * cart[i].quantity;
        subtotal += cost;
        printf("%s\t%d\t%.2f\t%.2f\n", cart[i].name, cart[i].quantity, cart[i].price, cost);

        // Reduce stock
        for (int j = 0; j < itemCount; j++) {
            if (inventory[j].id == cart[i].id) {
                inventory[j].stock -= cart[i].quantity;
                break;
            }
        }
    }

    float discount = (subtotal >= DISCOUNT_THRESHOLD) ? subtotal * DISCOUNT_RATE : 0;
    float gst = subtotal * GST_RATE;
    float total = subtotal - discount + gst;

    printf("\nSubtotal: %.2f", subtotal);
    printf("\nDiscount: -%.2f", discount);
    printf("\nGST (5%%): +%.2f", gst);
    printf("\nTotal Bill: %.2f\n", total);

    int points = (int)(total / 100);
    printf("Loyalty Points Earned: %d\n", points);

    // Payment mode
    int mode;
    printf("\nSelect Payment Mode (1.Cash 2.UPI 3.Card): ");
    scanf("%d", &mode);
    getchar(); // clear newline

    if (mode == 2) {
        char upi[50];
        printf("Enter UPI ID: ");
        fgets(upi, sizeof(upi), stdin);
        upi[strcspn(upi, "\n")] = 0;
        printf("Payment Successful via UPI (%s)\n", upi);
    } else if (mode == 3) {
        char card[20], pin[10];
        printf("Enter Card Number: ");
        fgets(card, sizeof(card), stdin);
        card[strcspn(card, "\n")] = 0;
        printf("Enter PIN (4 digits): ");
        fgets(pin, sizeof(pin), stdin);
        pin[strcspn(pin, "\n")] = 0;
        printf("Payment Successful via Card ending with %c%c%c%c\n",
               card[strlen(card)-4], card[strlen(card)-3], card[strlen(card)-2], card[strlen(card)-1]);
    } else {
        printf("Payment Successful via Cash!\n");
    }

    // Gift system
    int giftCount = (int)(total / 1000);
    if (giftCount > 0) {
        printf("\n Congratulations! You qualify for %d gift(s):\n", giftCount);
        for (int i = 1; i <= giftCount; i++) {
            printf("  ➤ Gift #%d: Premium Item Worth ₹%d\n", i, i * 100);
        }
    } else {
        printf("\nNo gift this time — spend ₹1000 or more for a free item next time!\n");
    }

    // Spin-the-Wheel Reward
    spinTheWheel(&total);
//  Save bill receipt to file
saveReceiptToFile(billID, customerName, mobile, address, subtotal, discount, gst, total);

//  Final Bill Summary
printf("\n======== BILL SUMMARY ========\n");
printf("Customer: %s\n", customerName);
printf("Total Before Discount: ₹%.2f\n", subtotal);
printf("Discount Applied: ₹%.2f\n", discount);
printf("GST: ₹%.2f\n", gst);
printf("Final Total: ₹%.2f\n", total);
printf("===============================\n");
printf("Bill saved successfully.\n");

    // Save sale data
    sales[saleCount].billID = billID;
    sales[saleCount].totalAmount = total;
    strcpy(sales[saleCount].date, datetime);
    saleCount++;

    printf("\n Checkout Complete! Final Amount Paid: ₹%.2f\n", total);
    cartCount = 0;
}

// Save and load
void saveData() {
    FILE *f1 = fopen("inventory.txt", "w");
    for (int i = 0; i < itemCount; i++) {
        fprintf(f1, "%d %s %.2f %d %s\n",
                inventory[i].id, inventory[i].name,
                inventory[i].price, inventory[i].stock,
                inventory[i].category);
    }
    fclose(f1);

    FILE *f2 = fopen("sales.txt", "w");
    for (int i = 0; i < saleCount; i++) {
        fprintf(f2, "%d %.2f %s\n",
                sales[i].billID, sales[i].totalAmount, sales[i].date);
    }
    fclose(f2);

    printf("\n Data saved successfully!\n");
}

void loadData() {
    FILE *f1 = fopen("inventory.txt", "r");
    if (f1) {
        while (fscanf(f1, "%d %s %f %d %s",
                      &inventory[itemCount].id,
                      inventory[itemCount].name,
                      &inventory[itemCount].price,
                      &inventory[itemCount].stock,
                      inventory[itemCount].category) != EOF) {
            itemCount++;
        }
        fclose(f1);
    }

    FILE *f2 = fopen("sales.txt", "r");
    if (f2) {
        while (fscanf(f2, "%d %f %[^\n]",
                      &sales[saleCount].billID,
                      &sales[saleCount].totalAmount,
                      sales[saleCount].date) != EOF) {
            saleCount++;
        }
        fclose(f2);
    }
}

void showSalesReport() {
    printf("\n---- SALES REPORT ----\n");
    float totalSales = 0;
    for (int i = 0; i < saleCount; i++) {
        printf("Bill #%d | Amount: %.2f | Date: %s\n",
               sales[i].billID, sales[i].totalAmount, sales[i].date);
        totalSales += sales[i].totalAmount;
    }
    printf("-----------------------------\n");
    printf("Total Sales: %.2f\n", totalSales);
}

void viewCart() {
    if (cartCount == 0) {
        printf("\n🛒 Your cart is empty!\n");
        return;
    }

    printf("\n======= CART ITEMS =======\n");
    printf("Item\tQty\tPrice\tTotal\n");
    float total = 0;
    for (int i = 0; i < cartCount; i++) {
        float cost = cart[i].price * cart[i].quantity;
        total += cost;
        printf("%s\t%d\t%.2f\t%.2f\n", cart[i].name, cart[i].quantity, cart[i].price, cost);
    }
    printf("--------------------------\n");
    printf("Current Cart Total: %.2f\n", total);
}

// Main
int main() {
    loadData();
    int choice;

    while (1) {
        printf("\n==== SUPERMARKET MANAGEMENT ====\n");
        printf("1. Add Item (Admin)\n");
        printf("2. Display Inventory\n");
        printf("3. Search/Filter Item\n");
        printf("4. Add to Cart (Customer)\n");
        printf("5. View Cart\n");
        printf("6. Checkout & Billing\n");
        printf("7. Show Sales Report\n");
        printf("8. Save & Exit\n");
 
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addItem(); break;
            case 2: displayItems(); break;
            case 3: searchItem(); break;
            case 4: addToCart(); break;
            case 5: viewCart(); break;
            case 6: generateBill(); break;
            case 7: showSalesReport(); break;
            case 8:
                saveData();
                printf("\nExiting... Thank you!\n");
                exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}
