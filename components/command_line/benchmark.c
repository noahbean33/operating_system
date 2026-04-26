#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>

/* speed in MB/s */
typedef struct {
    float secs, speed_mbs;
} BenchmarkResult;

typedef struct {
    FILE *src, *dst;
    char *dst_filename;
    int num_gb;
    BenchmarkResult result;
    void *pbuf;
    int chunksize;
} Benchmark;

long get_micros()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    long micros = now.tv_sec * 1000L * 1000L + now.tv_usec; 
    return micros;
}

/* size in bytes, micros in micros */
void set_benchmark_result(Benchmark *pbenchmark, long size, long micros)
{
    float speed_mbs = ((double) size) / (double) micros;
    float seconds = ((float) micros) / (float) 1000000L;
    pbenchmark->result = (BenchmarkResult) {seconds, speed_mbs};
}

void print_result(BenchmarkResult result)
{
    printf("Took %.2f seconds. Write speed: %.2fMB/s\n", result.secs, result.speed_mbs);
}

void process_benchmark(Benchmark *pbenchmark)
{
    long size = pbenchmark->num_gb * 1000L * 1000L * 1000L; // X GB of data
    long reps = size / pbenchmark->chunksize;

    long micros = get_micros();
    for(long i=0; i<reps; i++)
    {
        fread(pbenchmark->pbuf, pbenchmark->chunksize, 1, pbenchmark->src);
        fwrite(pbenchmark->pbuf, pbenchmark->chunksize, 1, pbenchmark->dst);
    }
    long delta_time_micros = get_micros() - micros;

    set_benchmark_result(pbenchmark, size, delta_time_micros);
}

/* Returns a string containing the test / target file path. 300 chars max.
Return value MUST be freed */
char *create_dst_file_string(char *path)
{
    char *pdst = malloc(300);
    strcpy(pdst, path);
    strcat(pdst, "/test.bin");
    return pdst;
}

/* Returns a valid benchmark. Otherwise NULL. */
Benchmark* create_benchmark(char *source_path, char *dst_dir, int num_gb)
{
    Benchmark *pbench = malloc(sizeof(Benchmark));
    pbench->chunksize = 128;
    void *pbuf = malloc(pbench->chunksize);
    
    FILE *psrc = fopen(source_path, "r");

    char *dst_file = create_dst_file_string(dst_dir);
    pbench->dst_filename = dst_file;
    FILE *pdst = fopen(dst_file, "w");

    if(!psrc || !pdst || !pbuf)
    {
        return NULL;
    }

    pbench->src = psrc;
    pbench->dst = pdst;
    pbench->num_gb = num_gb;
    pbench->pbuf = pbuf;

    return pbench;
}

void destroy_benchmark(Benchmark *pbench)
{
    if(pbench->src != NULL)
    {
        fclose(pbench->src);
    }
    if(pbench->dst != NULL)
    {
        fclose(pbench->dst);
    }
    if(pbench->dst_filename != NULL)
    {
        remove(pbench->dst_filename);
        free(pbench->dst_filename);
    }
    if(pbench->pbuf != NULL)
    {
        free(pbench->pbuf);
    }

    free(pbench);
}

void benchmark(int num_gb, char *path)
{
    assert(num_gb != 0 && "Number of gigabytes must not be zero");
    assert(path && "Path must not be NULL");
     
    Benchmark *pbenchmark_zero = create_benchmark("/dev/zero", path, num_gb);
    Benchmark *pbenchmark_rand = create_benchmark("/dev/random", path, num_gb);
    assert((pbenchmark_zero || pbenchmark_rand) && "Benchmark creation failed");

    printf("Testing zeros (write):\n");
    process_benchmark(pbenchmark_zero);
    print_result(pbenchmark_zero->result);

    printf("Testing random numbers (write):\n");
    process_benchmark(pbenchmark_rand);
    print_result(pbenchmark_rand->result);

    destroy_benchmark(pbenchmark_zero);
    destroy_benchmark(pbenchmark_rand);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s <num-of-gigabytes> <path-to-disk-directory>\n", argv[0]);
        return 0;
    }
    int num_gb = atoi(argv[1]);
    char *diskpath = argv[2];
    benchmark(num_gb, diskpath);

    return 0;
}
