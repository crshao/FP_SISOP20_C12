#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int getch (void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr (STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar ();
    tcsetattr (STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

int er (const char *msg) {
    char m[1024];
    strcat (strcat (strcpy (m, "error: "), msg), "\n");
    puts (m);
    return 0;
}

int fileSize (char *filename) {
    struct stat st;
    if (stat (filename, &st) == 0)
        return st.st_size;
    else
        return -1;
}

void terminalSize (int *row, int *col) {
    struct winsize w;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
    
    *row = w.ws_row;
    *col = w.ws_col;
}

void countSize (char *filename, int *row, int *col) {
    int lines=0, maxColumn=0;
    int column=0;
    int ch;
    FILE *file = fopen (filename, "r");
    
    while ((ch = fgetc (file)) != EOF) {
        column++;
        if (ch == '\n') {
            lines++;
            if (column > maxColumn)
                maxColumn = column;
            column = 0;
        }
    }
    
    *row = lines;
    *col = maxColumn;
}

int main (int argc, char **argv) {
 
    if (access (argv[1], F_OK) == -1) {
        char msg[1024];
        sprintf (msg, "%s: file atau directory tidak ditemukan", argv[1]);
        return er (msg);
    }
    
    int row, col;
    terminalSize (&row, &col);
    
    int lines, maxColumn;
    countSize (argv[1], &lines, &maxColumn);
    
    FILE *file = fopen (argv[1], "r");
    int sz = fileSize (argv[1]);
    
    char content[lines][maxColumn];
    int r=0, c=0;
    int temp;
    
    while ((temp = fgetc (file)) != EOF) {
        if (temp == '\n')
            content[r][c] = '\0', r++, c=0;
        else
            content[r][c++] = (char) temp;
    }
    
    int currentRow = 0;
    row--;
    
    int input;
    
    while (1) {
        system ("clear");
        for (int i = 0; i < row; i++)
            puts (content[currentRow+i]);
    
        if (currentRow + row == lines - 1)
            printf ("((end of file))");
        else
            printf (":");
        input = getch ();
        printf ("\n");
        
        switch (input) {
            case 65:
                if (currentRow > 0)
                    currentRow--;
                break;
                
            case 66:
                printf ("%d %d\n", currentRow, lines);
                if (currentRow + row < lines - 1)
                    currentRow++;
                break;
            
            case 'q':
                system ("clear");
                return 0;
        }
    }
    
    return 0;
}
