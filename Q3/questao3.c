#include <stdio.h>
#include <stdlib.h>

int row_start_pos = 1;
int column_start_pos = 1;

int row_end_pos = 10;
int column_end_pos = 18;

char** maze;
int** visited;
int rows = 12;
int cols = 20;

enum terrain {
	empty,
	wall,
	goal,
	crumb
};

// Alocar memória para a matriz visited
void alloc_visited()
{
	visited = malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; ++i){
		visited[i] = malloc(cols * sizeof(int*));
	}
}

// Alocar memória para a matriz maze
void alloc_maze()
{
	maze = malloc(rows * sizeof(char*));
	for (int i = 0; i < rows; ++i){
		maze[i] = malloc(cols * sizeof(char*));
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

			maze[i][j] = matrix_value;
		}
	}

	fclose(maze_file);
}

// Inicializar a matriz auxiliar visited
void init_visited()
{
	alloc_visited();


	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			if (maze[i][j] == '0') {
				visited[i][j] = wall;
			} else {
				visited[i][j] = empty;
			}
		}
	}

  // Define goal
  visited[row_end_pos][column_end_pos] = goal;
}	

void print_maze()
{
	int i, j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			printf("%c", maze[i][j]);
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
			if (visited[i][j] == crumb) {
				maze[i][j] = '.';
			}
		}
	}
}

int dfs(int row, int col)
{
	int* current = &visited[row][col];

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

int main()
{

  // pthread_t *threads;
  // threads = new pthread_t[N];
  // long *taskids = new long[N];

  // pthread_join(threads[0], NULL);

  get_maze("maze.txt");
	init_visited();

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