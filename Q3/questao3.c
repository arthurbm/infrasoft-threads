#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define num_threads 1

#define row_start_pos 1
#define column_start_pos 1

#define row_end_pos 10
#define column_end_pos 18

#define rows 12
#define cols 20

int** maze;

enum terrain {
	wall,
	empty,
	goal,
	path
};

// Alocar memória para a matriz maze
void alloc_maze()
{
	maze = malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; ++i){
		maze[i] = malloc(cols * sizeof(int*));
	}
}

// Guardar os valores recebidos no arquivo em uma matriz
void get_maze(char* file_name)
{
	char matrix_value;

	FILE* maze_file = fopen(file_name, "r");

	alloc_maze();
	
	int i,j;
  // Inicializar a matriz
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {

			if ((matrix_value = getc(maze_file)) == '\n') {
				matrix_value = getc(maze_file);
			}

      if (matrix_value == '0') {
				maze[i][j] = wall;
			} else {
				maze[i][j] = empty;
			}
      // Define goal
      maze[row_end_pos][column_end_pos] = goal;
      
		}
	}

	fclose(maze_file);
}

void print_maze()
{
	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			printf("%d", maze[i][j]);
		}
		printf("\n");
	}	
	printf("\n");
}

void add_path()
{
	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			if (maze[i][j] == path) {
				maze[i][j] = 5;
			}
		}
	}
}

int dfs(int row, int col)
{
	int* current = &maze[row][col];

	if (*current == goal) {
		return 1;
	}

	if (*current == empty) {
		*current = wall;

		if (dfs(row, col - 1)){
			*current = path;
			return 1;
		}
		if (dfs(row + 1, col)){
			*current = path;
			return 1;
		} 
		if (dfs(row, col + 1)){
			*current = path;
			return 1;		
		} 
		if (dfs(row - 1, col)){
			*current = path;
			return 1;
		}
	}

	return 0;
}

void *func_thread(void *arg) {

  if (!dfs(row_start_pos, column_start_pos)) {
		printf("Nenhum caminho foi encontrado\n");
	} else {
		add_path();
    printf("Matriz com o caminho:\n\n");
		print_maze();
	}
}

// O caminho é preenchido com números 5 e a posição final vira um 2
int main()
{
  pthread_t threads[num_threads];
  int *threadIDs[num_threads];

  get_maze("labirinto.txt");

  printf("Matriz inicial:\n\n");
	print_maze();
  
  
  for (int i = 0; i < num_threads; i++)
  {
    threadIDs[i] = (int*) malloc(sizeof(int));
    *threadIDs[i] = i;
    if (pthread_create(&threads[i], NULL, &func_thread, (void*)threadIDs[i]) != 0)
      printf("Falha na criação da thread");

    pthread_join(threads[i], NULL);
  }

  return 0;
}