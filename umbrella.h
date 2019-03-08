#define NROW 14
#define NCOL 12
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define N_um 132
#define DIM 512

typedef struct umbrella_t
{
  int number;
  int status; //0-free 1-occuped 2-temp_occupied
  int start;
  int expire;
  int request;
  int diritto; // Diritto di accedere all' ombrellone (X cancel)
} umbrella;
typedef struct threadinput_t
{
  int sockfd;
  struct umbrella_t *beach;
} threadinput;

void PrintStatus(void *);
void TxtStatus(umbrella[]);
umbrella *BookInput(char *, umbrella *);
void readBeachStatus(void *);