#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100
#define URL_LEN 256
#define TIME_LEN 64
#define FILE_NAME "history.txt"

typedef struct {
    char url[URL_LEN];
    char ts[TIME_LEN];
} Entry;


Entry stack[MAX];
int top = -1;


Entry fstack[MAX];
int ftop = -1;


void now_str(char *buf, size_t n) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buf, n, "%Y-%m-%d %H:%M:%S", &tm);
}


void saveHistory() {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) return;
    for (int i = 0; i <= top; ++i) {
        fprintf(fp, "%s\t%s\n", stack[i].ts, stack[i].url);
    }
    fclose(fp);
}


void loadHistory() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return;
    char line[TIME_LEN + URL_LEN + 8];
    while (fgets(line, sizeof(line), fp)) {
        char *tab = strchr(line, '\t');
        if (!tab) continue;
        *tab = '\0';
        char *ts = line;
        char *url = tab + 1;
        url[strcspn(url, "\r\n")] = 0;
        if (top < MAX - 1) {
            ++top;
            strncpy(stack[top].ts, ts, TIME_LEN-1); stack[top].ts[TIME_LEN-1] = '\0';
            strncpy(stack[top].url, url, URL_LEN-1); stack[top].url[URL_LEN-1] = '\0';
        } else {
      
            for (int i = 0; i < top; ++i) stack[i] = stack[i+1];
            strncpy(stack[top].ts, ts, TIME_LEN-1); stack[top].ts[TIME_LEN-1] = '\0';
            strncpy(stack[top].url, url, URL_LEN-1); stack[top].url[URL_LEN-1] = '\0';
        }
    }
    fclose(fp);
}


void visitURL(const char *url) {
    Entry e;
    now_str(e.ts, sizeof(e.ts));
    strncpy(e.url, url, URL_LEN-1); e.url[URL_LEN-1] = '\0';

 
    ftop = -1;

    if (top < MAX - 1) {
        stack[++top] = e;
    } else {
        
        for (int i = 0; i < top; ++i) stack[i] = stack[i+1];
        stack[top] = e; 
    }
    saveHistory();
    printf("Visited: %s  (%s)\n", e.url, e.ts);
}


void goBack() {
    if (top < 0) {
        printf("No history.\n");
        return;
    }
    
    if (ftop < MAX - 1) {
        fstack[++ftop] = stack[top];
    } else {
        
        for (int i = 0; i < ftop; ++i) fstack[i] = fstack[i+1];
        fstack[ftop] = stack[top];
    }
    --top; 
    saveHistory();
    if (top >= 0) printf("Went back. Current page: %s\n", stack[top].url);
    else printf("Went back. No current page.\n");
}


void goForward() {
    if (ftop < 0) {
        printf("No forward page.\n");
        return;
    }
    Entry e = fstack[ftop--];
    if (top < MAX - 1) {
        stack[++top] = e;
    } else {
       
        for (int i = 0; i < top; ++i) stack[i] = stack[i+1];
        stack[top] = e;
    }
    saveHistory();
    printf("Went forward. Current page: %s\n", stack[top].url);
}
void showHistory() {
    if (top < 0) {
        printf("No history.\n");
        return;
    }
    printf("Browser History (most recent first):\n");
    for (int i = top; i >= 0; --i) {
        printf("  %d) %s  -  %s\n", i+1, stack[i].ts, stack[i].url);
    }
    printf("Current page: %s\n", stack[top].url);
}


void clearHistory() {
    top = -1;
    ftop = -1;
    
    FILE *fp = fopen(FILE_NAME, "w");
    if (fp) fclose(fp);
    printf("History cleared.\n");
}


void showMenu() {
    printf("\n-- Menu --\n");
    printf("1) Visit URL\n");
    printf("2) Back\n");
    printf("3) Forward\n");
    printf("4) Show History\n");
    printf("5) Clear History\n");
    printf("0) Exit\n");
    printf("Choose: ");
}

int main() {
    loadHistory();
    printf("Browser history\n");

    int choice;
    char input[URL_LEN];

    while (1) {
        showMenu();
        if (scanf("%d", &choice) != 1) break;
        getchar(); 

        if (choice == 0) break;

        if (choice == 1) {
            printf("Enter URL: ");
            if (!fgets(input, sizeof(input), stdin)) continue;
            input[strcspn(input, "\r\n")] = 0;
            if (strlen(input) == 0) { printf("Empty URL.\n"); continue; }
            visitURL(input);
        }
        else if (choice == 2) {
            goBack();
        }
        else if (choice == 3) {
            goForward();
        }
        else if (choice == 4) {
            showHistory();
        }
        else if (choice == 5) {
            printf("Are you sure you want to clear history? (y/N): ");
            char c = getchar();
            while (getchar() != '\n'); 
            if (c == 'y' || c == 'Y') clearHistory();
            else printf("Cancelled.\n");
        }
        else {
            printf("Invalid option.\n");
        }
    }

    printf("Exiting.\n");
    return 0;
}