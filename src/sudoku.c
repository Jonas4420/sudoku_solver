#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <time.h>
#include <unistd.h>

#include "sudoku.h"

static pset_t** grid_alloc(void);

static void grid_free(pset_t**);

static pset_t** grid_copy(pset_t**);

static pset_t** grid_parser(FILE*);

// Generate a grid of size grid_size
static pset_t** grid_generate(bool);

static bool check_input_char(char);

static bool subgrid_map(pset_t**, bool (*func)(pset_t*[]));

static void grid_print(pset_t**);

// Print a grid solved to have readable output format
static void grid_print_solved(pset_t**);

#ifdef DEBUG
static bool subgrid_print(pset_t*[]);
#endif

// Search for a solution
// If the strict mode is set, then it will search for 2 solutions.
// If 2 solutions are found, it returns 2
// Else, it returns the number of solutions found (0 or 1)
// If the strict mode is not set, it searches the first solution.
// If the generate mode is set, the backtracking makes random choices
// Parameter : the grid to solve
// Return : return a number of solutions
static int grid_solver(pset_t**);

// Return : SOLVED if the grid has been solved
//          CONSISTENT if it has not been solved but still consistent
//          INCONSISTENT if not solved and unconsistent
static int grid_heuristics(pset_t**);

// Apply heuristics to the subgrid
// Return : true if the subgrid has not been modified (fixpoint reached)
//          false otherwise
static bool subgrid_heuristics(pset_t*[]);

static bool cross_hatching(pset_t*[]);

static bool lone_number(pset_t*[]);

static bool n_possible(pset_t*[]);

static int grid_choice(pset_t**);

static bool grid_consistency(pset_t**);

static bool subgrid_consistency(pset_t*[]);

// Return : count of occurencies of a given pset
static int subgrid_count(pset_t*[], pset_t);

static bool grid_solved(pset_t**);

// Check if the length of the grid is a correct length
static bool grid_valid_size(int);

// Write the string on the error stream then exit
static void grid_error(const char*);

// Error message for too many or too few lines
static void grid_error_line_number(void);

// Error message for incorrect lines, passing the line number in argument
static void grid_error_line(int);

// Error message for incorrect character
// passing the bad character and the line where it occures
static void grid_error_char(char, int);

static void usage(int);

static void version(void);

static char* soft_name = NULL;
static FILE* output_stream = NULL;
static bool verbose, generate, strict;

// Size of the current grid
static unsigned short grid_size = 0;

static const int heuristics_number = 3;
static bool (*heuristics[])(pset_t*[]) = {
    cross_hatching,
    lone_number,
    n_possible};

int
main(int argc, char* argv[])
{
    int optc;
    FILE* grid_file = NULL;
    struct option long_opts[] = {
        {"output", required_argument, NULL, 'o'},
        {"verbose", no_argument, NULL, 'v'},
        {"version", no_argument, NULL, 'V'},
        {"help", no_argument, NULL, 'h'},
        {"generate", optional_argument, NULL, 'g'},
        {"strict", no_argument, NULL, 's'},
        {NULL, 0, NULL, 0}};

    soft_name = argv[0];
    output_stream= stdout;
    verbose = false;
    generate = false;
    strict = false;

    // Scan the options
    while((optc = getopt_long(argc, argv, "o:vVhg::s", long_opts, NULL)) != -1)
    {
        switch(optc)
        {
            case 'o': // Set the output stream for printing the result
                output_stream = fopen(optarg, "w");

                if(!output_stream)
                {
                    perror("Output stream");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v': // Set the verbose mode on
                verbose = true;
                break;
            case 'V': // Show the version of the software and exit
                version();
                break;
            case 'h': // Show the help for the software and exit
                usage(EXIT_SUCCESS);
                break;
            case 'g': // Generate a grid of a size given (default : 9)
                if(optarg == NULL)
                    grid_size = 9;
                else
                {
                    grid_size = atoi(optarg);
                    if(!grid_valid_size(grid_size))
                        usage(EXIT_FAILURE);
                }

                generate = true;
                break;
            case 's':
                strict = true;
                break;
            default: // If the option is invaild, show an error and exit
                usage(EXIT_FAILURE);
        }
    }

    // Initialize the seed for the random number generator
    srand(time(NULL) + getpid());

    if(generate)
    {
        // Generate a random grid
        pset_t** grid = grid_generate(strict);

        grid_print_solved(grid);

        grid_free(grid);
    }
    else
    {
        // This option is not a valid option in this mode
        if(strict)
            usage(EXIT_FAILURE);

        // Make sure that the software is called with at least one file
        if(optind == argc)
            usage(EXIT_FAILURE);

        grid_file = fopen(argv[optind], "r");
        if(!grid_file)
        {
            perror(argv[optind]);
            exit(EXIT_FAILURE);
        }

        pset_t** grid = grid_parser(grid_file);

        if(grid_solver(grid) >= 1)
        {
            printf("The grid has been solved!\n");

            grid_print_solved(grid);
        }
        else
        {
            printf("The grid hasn't been solved!\n");
            printf("The grid isn't consistent!\n");

            grid_print(grid);
        }

        grid_free(grid);
        fclose(grid_file);
    }

    if(output_stream != stdout)
        fclose(output_stream);

    return EXIT_SUCCESS;
}

static pset_t**
grid_alloc(void)
{
    pset_t** result;

    result = calloc(grid_size, sizeof(pset_t*));
    if(result == NULL)
        grid_error("out of memory !");

    for(int i = 0 ; i < grid_size ; ++i)
    {
        result[i] = calloc(grid_size, sizeof(pset_t));
        if(result[i] == NULL)
            grid_error("out of memory !");
    }

    return result;
}

static void
grid_free(pset_t** grid)
{
    for(int i = 0 ; i < grid_size ; ++i)
        free(grid[i]);

    free(grid);
}

static pset_t**
grid_copy(pset_t** grid)
{
    pset_t** result;

    result = grid_alloc();

    for(int i = 0 ; i < grid_size ; ++i)
        for(int j = 0 ; j < grid_size ; ++j)
            result[i][j] = grid[i][j];

    return result;
}

static pset_t**
grid_parser(FILE* file)
{
    pset_t** result = NULL;
    char current_char;
    char first_line[MAX_COLORS];
    int line, column;

    // States of the parser
    bool state_first_line = true;
    bool state_empty_line = true;
    bool state_commentary = false;
    bool state_end_of_file = false;

    line = column = 0;

    // Scanning the grid
    while(!state_end_of_file)
    {
        current_char = fgetc(file);
        switch(current_char)
        {
            case ' ': // Filter out blank characters
            case '\t':
                break;

            case EOF: 
                state_end_of_file = true;
                // Process also the '\n'-case code because the last line can be
                // ended with a EOF character without a '\n' character before.
                // In case of the EOF following the '\n', the parser would be
                // in an empty_line state so it would not do anything abnormal
            case '\n':
                if(!state_empty_line)
                {
                    // Special process for the first line
                    if(state_first_line)
                    {
                        if(!grid_valid_size(column))
                            grid_error_line(line + 1);

                        grid_size = column;
                        result = grid_alloc();

                        for(int i = 0 ; i < column ; ++i)
                        {
                            // Check if it is valid content as it was not
                            // checked on the first copy
                            if(check_input_char(first_line[i]))
                                if(first_line[i] == '_')
                                    result[line][i] = pset_full(grid_size);
                                else
                                    result[line][i] = char2pset(first_line[i]);
                            else
                                grid_error_char(first_line[i], line + 1);
                        }

                        state_first_line = false;
                    }

                    if(column != grid_size)
                        grid_error_line(line + 1);

                    column = 0;
                    ++line;
                }

                state_empty_line = true;
                state_commentary = false;

                break;

            case '#': // Starting the commentary state
                state_commentary = true;
                break;

            default:
                // Handle the character only if it is not in a commentary
                if(!state_commentary)
                {
                    state_empty_line = false;

                    if(state_first_line)
                        // On the first line, the content is copied
                        // without checking the validity of the character
                        // as can't tell the size of the grid yet.
                        // Verifications are done when copying into the grid
                        first_line[column] = current_char;
                    else
                    {
                        if(column >= grid_size)
                            grid_error_line(line + 1);

                        if(line >= grid_size)
                            grid_error_line_number();

                        if(check_input_char(current_char))
                            if(current_char == '_')
                                result[line][column] = pset_full(grid_size);
                            else
                                result[line][column] = char2pset(current_char);
                        else
                            grid_error_char(current_char, line + 1);
                    }

                    ++column;
                }

                break;
        }
    }

    if(result == NULL)
        grid_error("no grid found in the file");

    if(line < grid_size)
        grid_error_line_number();

    return result;
}

static pset_t**
grid_generate(bool strict)
{
    pset_t** result = grid_alloc();
    int remove_limit, percent;
    bool still_choices = true;
    // Bidimentional array to check if a cell can be removed
    bool checked_cells[grid_size][grid_size];
    int removed_cells = 0;

    // Fill up the grid with full pset
    for(int i = 0 ; i < grid_size ; ++i)
        for(int j = 0 ; j < grid_size ; ++j)
            result[i][j] = pset_full(grid_size);

    for(int i = 0 ; i < grid_size ; ++i)
        for(int j = 0 ; j < grid_size ; ++j)
            checked_cells[i][j] = false;

    // Slove the grid, as the backtracking use random choices if
    // it is in generate mode, it generates a randomly grid but
    // consistent and solved
    grid_solver(result);

    // Compute the percentage of cells to remove
    if(grid_size == 1)
        percent = 100;
    else
    {
        if(grid_size <= 16)
            percent = 40 + (rand() % 20);
        else if(grid_size <= 49)
            percent = 30 + (rand() % 10);
        else
            percent = 20 + (rand() % 5);
    }

    remove_limit = (grid_size * grid_size) * percent / 100;

    while((removed_cells < remove_limit) && still_choices)
    {
        // Choose a random coordinate
        int x = rand() % grid_size;
        int y = rand() % grid_size;

        // If the random cell is already checked,
        // move to the next cell, so it cannot have
        // infinite loop in the case of bad random choices
        while(checked_cells[x][y])
        {
            x = (x + 1) % grid_size;
            if(x == 0)
                y = (y + 1) % grid_size;
        }

        if(strict)
        {
            // Try to solve a copy with an unique solution
            pset_t** result_tmp = grid_copy(result);
            result_tmp[x][y] = pset_full(grid_size);

            if(grid_solver(result_tmp) == 1)
            {
                // If it has an unique solution, then apply the choice
                result[x][y] = pset_full(grid_size);
                ++removed_cells;
            }

            grid_free(result_tmp);
        }
        else
        {
            result[x][y] = pset_full(grid_size);
            ++removed_cells;
        }

        // The cell has been checked whatever is the result
        // As if it had one solution it is set to full
        // and if not, it is not necessary to re-check this cell
        checked_cells[x][y] = true;

        for(int i = 0 ; i < grid_size ; ++i)
            for(int j = 0 ; j < grid_size ; ++j)
                still_choices = still_choices || !(checked_cells[i][j]);
    }

    return result;
}

static bool
check_input_char(char c)
{
    bool result = false;

    if(c == '_')
        result = true;
    else
        for(int i = 0 ; i < grid_size ; ++i)
        {
            if(color_table[i] == c)
            {
                result = true;
                break;
            }
        }

    return result;
}

static bool
subgrid_map(pset_t** grid, bool (*func)(pset_t* subgrid[]))
{
    bool result = true;
    bool func_result;
    pset_t** subgrid;

    subgrid = calloc(grid_size, sizeof(pset_t*));
    if(subgrid == NULL)
        grid_error("out of memory !");

    // Scanning all rows
    for(int i = 0 ; i < grid_size ; ++i)
    {
        for(int j = 0 ; j < grid_size ; ++j)
            subgrid[j] = &(grid[i][j]);

        // The result of the function is computed apart as
        // the and operator does not compute all the expression
        // if one member is false
        func_result = (*func)(subgrid);
        result = result && func_result;
    }

    // Scanning all columns
    for(int j = 0 ; j < grid_size ; ++j)
    { 
        for(int i = 0 ; i < grid_size ; ++i)
            subgrid[i] = &(grid[i][j]);

        func_result = (*func)(subgrid);
        result = result && func_result;
    }

    // Scanning all blocks
    int block_size = (int)sqrt(grid_size);
    int block_i, block_j;
    int cell_i, cell_j;
    for(int i = 0 ; i < block_size; ++i)
    {
        for(int j = 0 ; j < block_size ; ++j)
        {
            // Coordinates of the top left cell of the block
            block_i = i * block_size;
            block_j = j * block_size;

            for(int k = 0 ; k < grid_size ; ++k)
            {
                cell_i = block_i + (k / block_size);
                cell_j = block_j + (k % block_size);
                subgrid[k] = &(grid[cell_i][cell_j]);
            }

            func_result = (*func)(subgrid);
            result = result && func_result;
        }
    }

    free(subgrid);

    return result;
}

static void
grid_print(pset_t** grid)
{
    char string[MAX_COLORS + 1];

    for(int i = 0 ; i < grid_size ; ++i)
    {
        for(int j = 0 ; j < grid_size ; ++j)
        {
            pset2str(string, grid[i][j]);
            fprintf(output_stream, "%*s", grid_size, string);

            if(j < (grid_size - 1))
                fprintf(output_stream, " ");
        }

        fprintf(output_stream, "\n");
    }
}

static void
grid_print_solved(pset_t** grid)
{
    if(generate && (grid_size == 1))
        fprintf(output_stream, "_\n");
    else
    {
        char string[MAX_COLORS + 1];

        for(int i = 0 ; i < grid_size ; ++i)
        {
            for(int j = 0 ; j < grid_size ; ++j)
            {

                if(pset_is_singleton(grid[i][j]))
                {
                    pset2str(string, grid[i][j]);
                    fprintf(output_stream, "%s", string);
                }
                else
                    fprintf(output_stream, "_");

                if(j < (grid_size - 1))
                    fprintf(output_stream, " ");
            }

            fprintf(output_stream, "\n");
        }
    }

}

#ifdef DEBUG
static bool
subgrid_print(pset_t* subgrid[])
{
    char string[MAX_COLORS];

    printf("subgrid:  ");
    for(int i = 0 ; i < grid_size ; ++i)
    {
        pset2str(string, *subgrid[i]);
        printf("(%d) = '%s'", i, string);

        if(i < (grid_size - 1))
            printf(", ");
    }
    printf("\n");

    return true;
}
#endif

static int 
grid_solver(pset_t** grid)
{
    // Current number of solutions
    int result = 0;

    if(!grid_consistency(grid))
        return 0;

    int heuristics_result = grid_heuristics(grid);

    // If heuristics resolve the grid, the grid has a unique solution
    if(heuristics_result == SOLVED)
        return 1;

    if(heuristics_result == UNCONSISTENT)
        return 0;

    // Choose a cell in the grid to apply the backtracking
    int coordinates = grid_choice(grid);
    int x, y;

    // Compute coordinates
    x = coordinates / grid_size;
    y = coordinates % grid_size;

    int pset_card = pset_cardinality(grid[x][y]);

    pset_t** grid_tmp;
    pset_t** grid_solution;
    pset_t pset_left, pset_choosen;

    grid_solution = NULL;
    // Test the value of the cell for each color of the set
    pset_choosen = grid[x][y];
    // Random number of the color that will be choosen for the cell
    for(int i = 0 ; i < pset_card ; ++i)
    {
        grid_tmp = grid_copy(grid);
        // The generate mode relies on the fact that the choices
        // are made randomly
        // Else, a determinist choice is done
        if(generate)
        {
            // Choose a random color in the choosen pset
            int random_color = (rand() % pset_cardinality(pset_choosen)) + 1;
            pset_left = pset_n_leftmost(pset_choosen, random_color);
        }
        else
        {
            pset_left = pset_leftmost(pset_choosen);
        }

        grid_tmp[x][y] = pset_left;

        if(verbose)
        {
            char str_pset[MAX_COLORS + 1];
            char str_left[MAX_COLORS + 1];

            pset2str(str_pset, grid[x][y]);
            pset2str(str_left, pset_left);

            fprintf(output_stream,
                    "Next choice at grid[%d][%d]"
                    " = '%s' and choice is '%s'.\n",
                    x,
                    y,
                    str_pset,
                    str_left);

            grid_print(grid_tmp);
        }

        int number_of_solutions;
        // Check if the new grid has at least one solution
        if((number_of_solutions = grid_solver(grid_tmp)) >= 1)
        {
            result += number_of_solutions;

            // If strict mode is set and still not 2 solutions, then continue
            if(strict && (result <= 1))
            {
                // Store a solution if it has never be done 
                if(grid_solution == NULL)
                    grid_solution = grid_tmp;
                else
                    grid_free(grid_tmp);
            }
            else
            {
                if(grid_solution != NULL)
                    grid_free(grid_solution);

                // Copy the solution found to the grid
                for(int j = 0 ; j < grid_size ; ++j)
                {
                    pset_t* tmp = grid[j];
                    grid[j] = grid_tmp[j];
                    grid_tmp[j] = tmp;
                }

                grid_free(grid_tmp);
                return result;
            }
        }
        else
        {
            if(verbose)
                fprintf(output_stream, "Bad choice.\n");
            grid_free(grid_tmp);
        }

        // remove the choice for the color of the cell
        pset_choosen = pset_substract(pset_choosen, pset_left);
    }

    // We are in this case if no solution were found
    // Or if the strict mode is set and there is only one solution
    if(grid_solution != NULL)
    {
        for(int i = 0 ; i < grid_size ; ++i)
        {
            pset_t* tmp = grid[i];
            grid[i] = grid_solution[i];
            grid_solution[i] = tmp;
        }
        grid_free(grid_solution);
    }

    return result;
}

static int
grid_heuristics(pset_t** grid)
{
    bool fixpoint = false;

    // Fixpoint is reached ?
    while(!fixpoint)
    {
        // Apply heuristics to each subgrid
        fixpoint = subgrid_map(grid, subgrid_heuristics);

        if(verbose)
        {
            grid_print(grid);
            fprintf(output_stream, "\n");
        }
    }

    return grid_consistency(grid) ?
        (grid_solved(grid) ? SOLVED : CONSISTENT): UNCONSISTENT;
}

static bool
subgrid_heuristics(pset_t* subgrid[])
{
    bool result = true;

    bool heuristic_result;
    for(int i = 0 ; i < heuristics_number ; ++i)
    {
        heuristic_result = heuristics[i](subgrid);
        result = result & heuristic_result;
    }
    
    return result;
}

static bool
cross_hatching(pset_t* subgrid[])
{
    bool result = true;
    pset_t pset_tmp;

    // Cross-hatching heuristic
    pset_t pset_singletons = pset_empty();
    for(int i = 0 ; i < grid_size ; ++i)
        if(pset_is_singleton(*subgrid[i]))
            pset_singletons = pset_or(pset_singletons, *subgrid[i]);

    for(int i = 0 ; i < grid_size ; ++i)
        if(!pset_is_singleton(*subgrid[i]))
        {
            pset_tmp = pset_substract(*subgrid[i], pset_singletons);
            if(!pset_equals(pset_tmp, *subgrid[i]))
            {
                *subgrid[i] = pset_tmp;
                result = false;
            }
        }

    return result;
}

static bool
lone_number(pset_t* subgrid[])
{
    bool result = true;
    pset_t pset_tmp;

    // Lone-number heuristic
    // Vector containing every color that appears only once in the subgrid
    pset_t pset_lone = pset_empty();
    // Vector containing every color that appears twice or more in the subgrid
    pset_t pset_more = pset_empty();

    for(int i = 0 ; i < grid_size ; ++i)
    {
        pset_t pset_lone_new, pset_more_new;

        pset_lone_new = pset_xor(*subgrid[i], pset_lone);
        pset_lone_new = pset_and(pset_lone_new, pset_negate(pset_more));

        pset_more_new = pset_and(*subgrid[i], pset_lone);
        pset_more_new = pset_or(pset_more_new, pset_more);

        pset_lone = pset_lone_new;
        pset_more = pset_more_new;
    }

    for(int i = 0 ; i < grid_size ; ++i)
    {
        if(!pset_is_singleton(*subgrid[i]))
        {
            pset_tmp = pset_and(*subgrid[i], pset_lone);
            if(!pset_equals(pset_tmp, pset_empty())
                    && !pset_equals(pset_tmp, *subgrid[i]))
            {
                *subgrid[i] = pset_tmp;
                result = false;
            }
        }
    }

    return result;
}

static bool
n_possible(pset_t* subgrid[])
{
    bool result = true;
    pset_t pset_tmp;

    // N-possible heuristic
    // When a n possible values are the only possible values on n cells
    // of a subgrid, then those cells are the only place that can hold those
    // values. So those n values can be removed from all other cells.
    // (ref: http://sudokuassistant.co.uk/solving/solving-sudoku.htm)
    for(int i = 0 ; i < grid_size ; ++i)
        if(subgrid_count(subgrid, *subgrid[i])
                == pset_cardinality(*subgrid[i]))
            for(int j = 0 ; j < grid_size ; ++j)
                // Working only on unsolved cells and cells that are not
                // equal to the current pset
                if((!pset_is_singleton(*subgrid[j]))
                        && (!pset_equals(*subgrid[i], *subgrid[j])))
                {
                    pset_tmp = pset_substract(*subgrid[j], *subgrid[i]);

                    if(!pset_equals(pset_tmp, *subgrid[j]))
                    {
                        *subgrid[j] = pset_tmp;
                        result = false;
                    }
                }

    return result;
}

static int 
grid_choice(pset_t** grid)
{
    int result;
    int x, y;
    int current_cardinality, min_cardinality;

    x = y = -1;
    min_cardinality = MAX_COLORS + 1;

    for(int i = 0 ; i < grid_size ; ++i)
        for(int j = 0 ; j < grid_size ; ++j)
            if(pset_cardinality(grid[i][j]) > 1)
            {
                current_cardinality = pset_cardinality(grid[i][j]);
                if(current_cardinality < min_cardinality)
                {
                    x = i;
                    y = j;
                    min_cardinality = current_cardinality;
                }
            }

    if((x != -1) && (y != -1))
        result = (x * grid_size) + y;
    else
        result = -1;

    return result;
}

static bool
grid_consistency(pset_t** grid)
{
    return subgrid_map(grid, subgrid_consistency);
}

static bool
subgrid_consistency(pset_t* subgrid[])
{
    bool result = true;

    // Check that each color appears at least once
    pset_t pset_colors = pset_empty();
    for(int i = 0 ; i < grid_size ; ++i)
        pset_colors = pset_or(*subgrid[i], pset_colors);
    result = result && pset_equals(pset_colors, pset_full(grid_size));

    // Check that there are not two singletons of the same color
    pset_t pset_singletons = pset_empty();
    for(int i = 0 ; (i < grid_size) && result ; ++i)
    {
        if(pset_is_singleton(*subgrid[i]))
        {
            if(pset_is_included(*subgrid[i], pset_singletons))
                result = false;
            else
                pset_singletons = pset_or(*subgrid[i], pset_singletons);
        }
    }

    // Check that there is no empty cell
    for(int i = 0 ; (i < grid_size) && result ; ++i)
        result = result && !pset_equals(*subgrid[i], pset_empty());

    return result;
}

static int
subgrid_count(pset_t* subgrid[], pset_t pset)
{
    int result = 0;

    for(int i = 0 ; i < grid_size ; ++i)
        if(pset_equals(*subgrid[i], pset))
            ++result;

    return result;
}

static bool
grid_solved(pset_t** grid)
{
    bool result = true;

    for(int i = 0 ; (i < grid_size) && result ; ++i)
        for(int j = 0 ; (j < grid_size) && result; ++j)
            result = result && pset_is_singleton(grid[i][j]);

    return result;
}


static bool
grid_valid_size(int size)
{
    bool result = false;
    // Valid sizes for the grids
    unsigned short valid_size_number = 8;
    unsigned short valid_sizes[] = {1, 4, 9, 16, 25, 36, 49, 64};

    for(int i = 0 ; (i < valid_size_number) && !result ; ++i)
        if(size == valid_sizes[i])
            result = true;

    return result;
}

static void
grid_error(const char* error_message)
{
    fprintf(stderr, "%s: error: ", soft_name);
    fprintf(stderr, "%s\n", error_message);

    exit(EXIT_FAILURE);
}

static void
grid_error_line_number(void)
{
    grid_error("too many/few lines in the grid");
}

static void
grid_error_line(int line)
{
    char error_message[256];

    snprintf(error_message, 256,
            "line %d is malformed (wrong number of cells)", line);
    grid_error(error_message);
}

static void
grid_error_char(char character, int line)
{
    char error_message[256];

    snprintf(error_message, 256,
            "wrong character '%c' at line %d", character, line);
    grid_error(error_message);
}

static void
usage(int status)
{
    switch(status)
    {
        case EXIT_SUCCESS: // Print the usage of the software
            printf("Usage: %s [OPTION] FILE...\n", soft_name);
            printf("Solve Sudoku puzzle's of variable sizes (1-4).\n\n"
                    "\t-o, --output=FILE\twrite result to FILE\n"
                    "\t-v, --verbose\t\tverbose output\n"
                    "\t-V, --version\t\tdisplay version and exit\n"
                    "\t-h, --help\t\tdisplay this help\n"
                    "\t-g [size], --generate=[size]\t\t"
                    "generate a grid of length size (default : 9)\n"
                    "\t-s, --strict\t\tenforce the generation of a grid"
                    "with only one solution\n");
            break;
        default: // Explain how to get help
            fprintf(stderr,
                    "Try '%s --help' for more information.\n", soft_name);
    }

    exit(status);
}

static void
version(void)
{
    printf("%s %d.%d.%d\n", 
            soft_name,
            PROG_VERSION,
            PROG_SUBVERSION,
            PROG_REVISION);
    printf("This software is a sudoku solver.\n");

    exit(EXIT_SUCCESS);
}

