#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int N = 0;

void start_counter();
double get_counter();
double mhz();

int *ind;

/* Initialize the cycle counter */

static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

/* Set *hi and *lo to the high and low order bits of the cycle counter.
 Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo) {
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
        : "=r"(*hi), "=r"(*lo)                /* and move results to */
        : /* No input */                      /* the two outputs */
        : "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
void start_counter() {
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter() {
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double)hi * (1 << 30) * 4 + lo;
    if (result < 0) {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

double mhz(int verbose, int sleeptime) {
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6 * sleeptime);
    if (verbose)
        printf("\n Processor clock rate = %.1f MHz\n", rate);
    return rate;
}

double funcion(double a[N][8], double b[N][8], double c[8]) {
    double e[N], f = 0.f;

    double **d = (double **) malloc(sizeof(double) * N);
    for (int i = 0; i < N; i++) {
        d[i] = (double *) malloc(sizeof(double) * N);
    }

    double dos[] = {2, 2};

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int64_t k = 0; k < 8; k += 2)  {
                // printf("%d %d %d\n", i, j, k);
                __m128d aik, bkj, ck;
                aik = _mm_load_pd(a[i] + k), bkj = _mm_load_pd(b[j] + k), ck = _mm_load_pd(c + k);

                __m128d resta = _mm_sub_pd(bkj, ck);
                __m128d multiplicacion = _mm_mul_pd(_mm_mul_pd(_mm_load_pd(dos), aik), resta);

                // _mm_store_pd(d[i] + j, _mm_add_pd(_mm_load_pd(d[i] + j), multiplicacion));
                // d[i][j] += 2 * a[i][k] * (b[k][j] - c[k]);
                d[i][j] += multiplicacion[0] + multiplicacion[1];
            }
        }
    }

    for (int i = 0; i < N; i++) {
        // printf("%d\n", i);
        e[i] = d[ind[i]][ind[i]] / 2;
        f += e[i];
    }

    return f;
}

void generarAleatorios() {
    int *used = (int *)malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++) used[i] = 0;

    if (ind != NULL) free(NULL);
    ind = (int *)malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++) {
        int start = rand() % N;
        while (used[start]) {
            start = (start + 1) % N;
        }
        ind[i] = start;
        used[start] = 1;
    }
    free(used);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Uso: %s [N]\n", argv[0]);
        return 1;
    } else {
        N = atoi(argv[1]);
    }

    double ck;
    double a[N][8], b[N][8], c[8];

    srand(time(0));

    for (int i = 0; i < N; i++) for (int j = 0; j < 8; j++) a[i][j] = rand();
    for (int i = 0; i < N; i++) for (int j = 0; j < 8; j++) b[i][j] = rand();
    for (int i = 0; i < 8; i++) c[i] = rand();

    generarAleatorios();

    start_counter();
    printf("R=%lf\n", funcion(a, b, c));
    ck = get_counter();

    printf("\n Clocks=%1.10lf \n", ck);

    FILE *f = fopen("out.txt", "w+");
    fprintf(f, "%lf", ck);
    fclose(f);

    /* Esta rutina imprime a frecuencia de reloxo estimada coas rutinas start_counter/get_counter */
    // mhz(1, 1);

    return 0;
}