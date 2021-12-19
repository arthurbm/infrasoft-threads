#include <stdio.h>
#include <stdlib.h>

int row_start_pos = 1;
int column_start_pos = 1;

int row_end_pos = 10;
int column_end_pos = 18;

int** maze;
int rows = 12;
int cols = 20;

enum terrain {
	wall,
	empty,
	goal,
	crumb
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

void add_crumbs()
{
	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			if (maze[i][j] == crumb) {
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
			*current = crumb;
			return 1;
		} 
		if (dfs(row + 1, col)){
			*current = crumb;
			return 1;
		} 
		if (dfs(row, col + 1)){
			*current = crumb;
			return 1;		
		} 
		if (dfs(row - 1, col)){
			*current = crumb;
			return 1;
		}
	}

	return 0;
}

// O caminho é preenchido com números 5 e a posição final vira um 2
int main()
{

  // pthread_t *threads;
  // threads = new pthread_t[N];
  // long *taskids = new long[N];

  // pthread_join(threads[0], NULL);

  get_maze("maze.txt");

  printf("Matriz inicial:\n\n");
	print_maze();

	if (!dfs(row_start_pos, column_start_pos)) {
		printf("Nenhum caminho foi encontrado\n");
	} else {
		add_crumbs();
    printf("Matriz com o caminho:\n\n");
		print_maze();
	}
	return 0;

  return 0;
}