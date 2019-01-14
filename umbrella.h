#define NROW 10
#define NCOL 10
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define N_um 100

typedef struct umbrella_t{
  int number;
  int status;         //0-free 1-occuped 2-temp_occupied 
  int start;
  int expire;
  int request;
  int diritto;        // Diritto di accedere all' ombrellone (X cancel)
}umbrella;

void PrintStatus(umbrella[]);
void TxtStatus(umbrella[]);
umbrella* BookInput(char*,umbrella*);