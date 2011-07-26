#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
/*
 * WTF!!! Why! Is this not in my version of sdlib? Fucking Ubuntu...
 */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

struct program {
    unsigned int radius; // 2^( 2*radius + 1 ) is neighborhood
    unsigned int neighborhood;
    unsigned int *function; // 2^(2^( 2*radius + 1 )) rules
};

struct automaton {
    struct program prog;
    char *state;
    char *next_state;
    char *swp;
    int l_size; // Latice size
    int steps; // How many steps to iterate
};

/*
 * Initialize an automaton
 */
void init_automaton( struct automaton* autom, unsigned int radius, unsigned int latice_size, unsigned int steps ) {
    int f_size, i;
    for(f_size = 1, i = 0;i< 2*radius+1 ;i++) {
        f_size *= 2;
    }
    printf("f_size: %i\n",f_size);
    autom->prog.neighborhood = 2*radius + 1;
    autom->prog.function = (unsigned int *) calloc(f_size, sizeof(unsigned int) );
    autom->prog.radius = radius;
    autom->l_size = latice_size;
    autom->steps = steps;

    autom->state = (char *)malloc( latice_size );
    autom->next_state = (char *)malloc( latice_size );
}

/*
 * Givin a Wolfram rule, generate the correct transformation function
 */
void generate_program( struct automaton* autom, unsigned int rule ) {
    int state,i;
    for( i=0, state = rule; rule > 0; rule = rule / 2, i++ ){
        autom->prog.function[i] = rule % 2;
        printf("i = %i, rule %i= %i\n",i,rule,rule%2 );
    }
    autom->prog.function[i] = rule % 2;
    printf("i = %i, rule %i= %i\n",i,rule,rule%2 );
}

/*
 * Write current state to file
 */
void save_state( struct automaton* autom, FILE *fp ) {
    int i;
    char new_line = '\n';
    char state;
    for(i = 0; i < autom->l_size; i++ ) {
        state = autom->state[i]+'0';
        fwrite( &state, 1, 1, fp );
    }
    fwrite( &new_line, 1, 1, fp );
}

/*
 * Find number using radius and look up next state
 */
void apply_function( struct automaton* autom ) {
    int i, j, k, cell_state, lucus;
    unsigned int radius = autom->prog.radius;
    // Do state while i < radius (we need to wrap around)
    i = j = k = cell_state = 0;
    while( i < autom->prog.radius ) {
        k = cell_state = 0;
        // Generate the rule that the current cell, i, is in.
        for( j = i - radius; j <= i + (int) radius ; j++ ) {
            cell_state *= 2; // Mult by 2
            if( j < 0 ) // We need to wrap around
                cell_state += autom->state[ autom->l_size - radius + i ];
            else
                cell_state += autom->state[j];
        }
        // Using the cell_state generated, look up the correct next state in the function table
        autom->next_state[i] = autom->prog.function[cell_state];
        i++;
    }
    // Do state until i < latice_size - radius
    while( i < autom->l_size - radius ) {
        k = cell_state = 0;
        // Generate the rule that the current cell, i, is in.
        for( j = i - radius; j <= i +radius; j++ ) {
            cell_state *= 2; // Mult by 2
            cell_state += autom->state[j];
        }
        // Using the cell_state generated, look up the correct next state in the function table
        autom->next_state[i] = autom->prog.function[cell_state];
        i++;
    }
    // Do state while i < latice_size
    while( i < autom->l_size ) {
        k = cell_state = 0;
        // Generate the rule that the current cell, i, is in.
        for( j = i - radius; j <= i +radius; j++ ) {
            cell_state *= 2; // Mult by 2
            if( j >= autom->l_size )
                cell_state += autom->state[ j % autom->l_size ]; //Wrap around
            else
                cell_state += autom->state[ j ]; //Wrap around
        }
        // Using the cell_state generated, look up the correct next state in the function table
        autom->next_state[i] = autom->prog.function[cell_state];
        i++;
    }

}
void get_initial_cond( struct automaton *autom ) {
    //int i;
    /*
    for(i=0;i<autom->l_size ;i++){
        autom->state[i] = 1;
        autom->next_state[i] = 0;
    }
     */
    autom->state[autom->l_size / 2] = 1;
}

/*
 * Use this if we every use a memory scheme
 */
void next_state( struct automaton *autom ) {
    autom->swp = autom->next_state;
    autom->next_state = autom->state;
    autom->state = autom->swp;
}
/*
 * Do one cycle with an automaton
 */
void cycle_do_one( struct automaton *autom, int radius, int latice_size, \
        int steps, int lower_rule, int upper_rule, char *fname )
{
    int rule,j;
    FILE *fp;
    char fname_num[30]; // fname with the rule appended
                        // Can you say buffer overflow?
    char num[10];


    for( rule = lower_rule; rule < upper_rule; rule++ ){
        // Set up file
        itoa(rule,num);
        strcpy(fname_num, fname);
        strcat(fname_num,num);
        fp = fopen( fname_num, "w+" );

        init_automaton( autom, radius, latice_size, steps );
        // Get initial conditions
        get_initial_cond( autom );
        // Get Program
        generate_program( autom, rule );
        // Generate while N > 0
        for( j = 0; j < autom->steps; j++ ){
            //  write to file
            save_state( autom , fp );
            apply_function( autom );
            next_state( autom );
            //  Calculate next state
        }
        fclose( fp );
    }

}

int main( int argc, char **argv ) {
    struct automaton autom;
    int i;
    FILE *fp;
    int radius, latice_size, steps, lower_rule, upper_rule;
    char *fname;
    if( argc != 7 ){
        printf("./run <file> <radius> <latice_size> <steps> <lower_rule> <upper_rule>\n");
        return 2;
    }
    fname = argv[1];
    radius = atoi( argv[2] );
    latice_size = atoi( argv[3] );
    steps = atoi( argv[4] );
    lower_rule = atoi( argv[5] );
    upper_rule = atoi( argv[6] );

    cycle_do_one( &autom, radius, latice_size, steps, lower_rule, upper_rule, (char *)fname );

    return 0;
}


