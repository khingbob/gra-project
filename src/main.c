#include <stdio.h>  // for printf
#include <stdlib.h> // for exit
#include <stdint.h> // for uint32_t
#include <errno.h>  // for errno
#include <string.h> // for strcmp, strrchr
#include <limits.h> // for UINT32_MAX
#include <getopt.h> // for getopt
#include <math.h>   // for log2
// Request is the format of one input to the simulation
struct Request
{
    uint32_t addr;
    uint32_t data;
    int we;
};

// Result is the output of the simulation
// it should be defined either in the simulation.cpp or here but now it is in both
struct Result
{
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitiveGateCount;
};

extern struct Result run_simulation(
    int cycles,
    unsigned tlbSize,
    unsigned tlbsLatency,
    unsigned blocksize,
    unsigned v2bBlockOffset,
    unsigned memoryLatency,
    size_t numRequests,
    struct Request requests[],
    const char *tracefile);

// usage_message can be changed to a more suitable message
const char *usage_message =
    "Usage: %s [options] -c --cycles   Die Anzahl der Zyklen, die simuliert werden sollen.\n"
    "   or: %s [options] --blocksize   Simuliert die Größe eines Speicherblocks in Byte.\n"
    "   or: %s [options] --v2b-block-offset   Gibt an, wie virtuelle Adressen zu physischen Adressen übersetzt werden.\n"
    "   or: %s [options] --tlb-size   Die Größe des TLBs in Einträgen.\n"
    "   or: %s [options] --tlb-latency   Die Latenzzeit des TLBs in Zyklen. Latenzzeit ist unabhängig von Treffer oder Miss.\n"
    "   or: %s [options] --memory-latency   Die Latenzzeit des Hauptspeichers in Zyklen. Lese und schreiboperationen haben die gleiche Latenz.\n"
    "   or: %s [options] --tf   Ausgabedatei für ein Tracefile mit allen Signalen.\n"
    "   or: %s [options]        Positional Argument: Die Eingabedatei, die die zu verarbeitenden Daten enthält.\n"
    "   or: %s -h --help        Zeigt diese Hilfe an.\n";

void print_usage(const char *program_name)
{
    fprintf(stderr, "\n%s", usage_message);
}

void print_help(const char *program_name)
{
    // print_usage(program_name);
    fprintf(stderr, "Help message : you have to at least provide a csv file that contains the requests you want to use in the simulation\n");
    fprintf(stderr, "\n%s", usage_message); // should be help_message
}
// should create a help_message but for now i will use the usage_message for help_message
// should decide if we even need both usage_message and help_message or just one of them
// for now i got inspired from the Nutzereingaben-Tutorial

int convert_unsigned(char *str, unsigned *value, char *value_name)
{
    errno = 0; // to ensure that any error detected is from the current conversion attempt
    char *endptr;
    unsigned long temp = strtoul(str, &endptr, 10);

    if (endptr == str || *endptr != '\0')
    {
        fprintf(stderr, "Invalid number: %s contains non numeric values or is empty.\n", value_name);
        return EXIT_FAILURE;
    }
    else if (errno == ERANGE)
    { // == ERANGE ?
        fprintf(stderr, "Invalid number: %s over- or underflows unsigned long.\n", value_name);
        return EXIT_FAILURE;
    }
    else if (temp > UINT_MAX)
    {
        fprintf(stderr, "Invalid number: %s exceeds the maximum of unsigned int\n", value_name);
        return EXIT_FAILURE;
    }
    // non of our values can be negative
    if (str[0] == '-')
    {
        fprintf(stderr, "Invalid number: %s is negative.\n", value_name);
        return EXIT_FAILURE;
    }

    *value = (unsigned)temp; // make sure that this casting does not cause any problems
    return EXIT_SUCCESS;
}

int convert_int(char *str, int *value)
{
    errno = 0;
    char *endptr;
    int temp = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0')
    {
        fprintf(stderr, "Invalid number: cycles contains non numeric values or is empty.\n");
        // for now only for cycles but can be adjusted if needed
        return EXIT_FAILURE;
    }
    else if (errno == ERANGE)
    { // ?
        fprintf(stderr, "Invalid number: cycles over- or underflows long.\n");
        return EXIT_FAILURE;
    }

    if (str[0] == '-')
    {
        fprintf(stderr, "Invalid number: cycles is negative.\n");
        return EXIT_FAILURE;
    }

    *value = (int)temp; // make sure that this casting does not cause any problems
    return EXIT_SUCCESS;
}
int isPowerOfTwo(unsigned int n)
{
    if (n == 0) // find out why it causes an error when n == 2
        return EXIT_FAILURE;

    while (n != 1)
    {
        if (n % 2 != 0)
            return EXIT_FAILURE;
        n /= 2;
    }
    return EXIT_SUCCESS;
}

int parse_uint32(const char *str, uint32_t *value, char *write_or_read)
{
    // Ensure errno is clear before conversion
    errno = 0;
    char *endptr;

    // Check for binary
    if (str[0] == '0' && (str[1] == 'b' || str[1] == 'B'))
    {
        // Skip '0b' and set the base to 2
        *value = strtoul(str + 2, &endptr, 2);
    }
    else
    {
        // base 0 to detect 0x and decimal
        *value = strtoul(str, &endptr, 0);
    }

    if (errno == ERANGE)
    {
        fprintf(stderr, "Number out of range: %s\n", str);
        return EXIT_FAILURE;
    }
    if (*write_or_read == 'R' && endptr == str)
    {
        return EXIT_SUCCESS;
    }
    else if (endptr == str)
    {
        fprintf(stderr, "Invalid number: %s is empty or contains a non numerical value.\n", str);
        return EXIT_FAILURE;
    }

    //*value = (uint32_t)temp; // temp is an unsigned long and this causes a problem by casting it to uint32_t find a diffrent way
    return EXIT_SUCCESS;
}

int convert_tracefile_name(char *filename, const char **value, char *value_name)
{
    if (filename == NULL)
    {
        return EXIT_SUCCESS;
    }

    const char *dot = strrchr(filename, '.'); // returns a pointer to the last occurrence of the character in the C string str.
    if (dot != NULL)
    {
        fprintf(stderr, "Invalid file name: %s should not have an extension.\n", value_name);
        return EXIT_FAILURE;
    }

    *value = filename;
    return EXIT_SUCCESS;
}
// maybe convert_tracefile_name  and get_input_file_name can be combined

int set_input_file_name(char *filename, const char **value)
{
    const char *validExtension = ".csv";

    const char *dot = strrchr(filename, '.');

    if (dot == NULL)
    {
        fprintf(stderr, "Invalid file name: Input_file does not have an extension.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(dot, validExtension) == 0)
    {
        *value = filename;
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Invalid file name: Input_file has an invalid extension.\n");
    return EXIT_FAILURE;
}

size_t get_numRequests(const char *filename)
{
    char full_path[PATH_MAX] = "examples/"; // Ensure there's enough space in the buffer
    strncat(full_path, filename, sizeof(full_path) - strlen(full_path) - 1);
    FILE *file = fopen(full_path, "r");
    if (file == NULL)
    {
        perror("Failed to open the file");
        return EXIT_FAILURE;
    }

    size_t numRequests_temp = 0;
    char line[100]; // to store each line read from the file 32 + 32 + 32 + 2 = 98

    while (fgets(line, sizeof(line), file))
    {
        numRequests_temp++;
    }

    if (feof(file) == 0)
    { // check if the end of the file has been reached
        perror("Failed to reach the end of the file");
        fclose(file);
        return EXIT_FAILURE;
    }

    fclose(file);
    return numRequests_temp;
}

int parse_input_file(const char *filename, struct Request requests[])
{
    char full_path[PATH_MAX] = "examples/";

    strncat(full_path, filename, sizeof(full_path) - strlen(full_path) - 1);

    FILE *file = fopen(full_path, "r");
    if (file == NULL)
    {
        perror("Failed to open the file");
        return EXIT_FAILURE;
    }

    char line[100];
    char *current_ptr; // to store the current pointer of the line
    int k = 0;         // request counter
    while ((current_ptr = fgets(line, sizeof(line), file)) != NULL)
    {

        int comma_counter = 0;
        char *start_ptr;
        char address_temp[32];

        // to check if it is a read or write or invalid
        char write_or_read = *current_ptr;
        if (write_or_read == 'W')
        {
            requests[k].we = 1;
        }
        else if (write_or_read == 'R')
        {
            requests[k].we = 0;
        }
        else
        {
            fprintf(stderr, "Invalid input file: %s\n", filename);
            fclose(file);
            return EXIT_FAILURE;
        }

        for (int i = 0; current_ptr[i] != '\0'; i++)
        {
            if (current_ptr[i] == ',' && comma_counter == 0)
            {
                start_ptr = current_ptr + i + 1; // pointer to the start of the address
                comma_counter++;
            }
            else if (current_ptr[i] == ',' && comma_counter == 1)
            {
                strncpy(address_temp, start_ptr, i - 2); // copy the address to the address_temp
                if (parse_uint32(address_temp, &requests[k].addr, "W") != 0)
                {
                    fclose(file);
                    return EXIT_FAILURE;
                }
                start_ptr = current_ptr + i + 1; // pointer to the start of the data
                comma_counter++;
            }
            else if (current_ptr[i] == '\n' && comma_counter == 2)
            {
                if (parse_uint32(start_ptr, &requests[k].data, &write_or_read) != 0)
                {
                    fclose(file);
                    return EXIT_FAILURE;
                }

                // check if the data is 0 for a read request
                if (write_or_read == 'R' && requests[k].data != 0)
                {
                    fprintf(stderr, "Invalid input file: %s\n", filename);
                    fclose(file);
                    return EXIT_FAILURE;
                }
                break;
            }
        }

        // reset for each line
        memset(address_temp, '\0', sizeof(address_temp));
        start_ptr = NULL;
        k++;
    }

    if (feof(file) == 0)
    {
        perror("Failed to reach the end of the file");
        fclose(file);
        return EXIT_FAILURE;
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    // The implementation of the Rahmenprogramm

    const char *program_name = argv[0];
    if (argc < 2)
    {
        fprintf(stderr, "No input values given.\n");
        print_help(program_name);
        return EXIT_FAILURE;
    }

    // here the default values

    int cycles = 1000000000;
    unsigned tlbSize = 8;
    unsigned tlbsLatency = 10;
    unsigned blocksize = 16;
    unsigned v2bBlockOffset = 5;
    unsigned memoryLatency = 30;
    size_t numRequests = 0;
    const char *tracefile = NULL; // by default no tracefile
    const char *input_file_name;

    int opt;
    int option_index = 0;

    struct option long_options[] = {
        {"cycles", required_argument, 0, 'c'},
        {"blocksize", required_argument, 0, 0},
        {"v2b-block-offset", required_argument, 0, 0},
        {"tlb-size", required_argument, 0, 0},
        {"tlb-latency", required_argument, 0, 0},
        {"memory-latency", required_argument, 0, 0},
        {"tf", required_argument, 0, 0},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0} // to indicate the end of the array
    };

    while ((opt = getopt_long(argc, argv, "c:h", long_options, &option_index)) != -1)
    {

        switch (opt)
        {
        case 0:
            if (strcmp(long_options[option_index].name, "blocksize") == 0)
            {
                if (convert_unsigned(optarg, &blocksize, "blocksize") != 0)
                {
                    return EXIT_FAILURE;
                }
                else if (isPowerOfTwo(blocksize) != 0)
                {
                    fprintf(stderr, "Invalid blocksize: %u is not a power of two.\n", blocksize);
                    return EXIT_FAILURE;
                }
                else if (blocksize >= UINT_MAX)
                {
                    fprintf(stderr, "Invalid blocksize: blocksize has to be less than %u.\n", UINT_MAX);
                    return EXIT_FAILURE;
                }
                break;
            }
            else if (strcmp(long_options[option_index].name, "v2b-block-offset") == 0)
            {
                if (convert_unsigned(optarg, &v2bBlockOffset, "v2b-block-offset") != 0)
                {
                    return EXIT_FAILURE;
                }
                break;
            }
            else if (strcmp(long_options[option_index].name, "tlb-size") == 0)
            {
                if (convert_unsigned(optarg, &tlbSize, "tlb-size") != 0)
                {
                    return EXIT_FAILURE;
                }
                else if (tlbSize <= 1) // tlb length should be more than 1
                {
                    fprintf(stderr, "Invalid tlb-size:  %u Tlb has to have length of at least two.\n", tlbSize);
                    return EXIT_FAILURE;
                }
                else if (tlbSize >= UINT_MAX)
                {
                    fprintf(stderr, "Invalid tlb-size: Tlb has to have length of less than %u.\n", UINT_MAX);
                    return EXIT_FAILURE;
                }
                break;
            }
            else if (strcmp(long_options[option_index].name, "tlb-latency") == 0)
            {
                if (convert_unsigned(optarg, &tlbsLatency, "tlb-latency") != 0)
                {
                    return EXIT_FAILURE;
                }
                break;
            }
            else if (strcmp(long_options[option_index].name, "memory-latency") == 0)
            {
                if (convert_unsigned(optarg, &memoryLatency, "memory-latency") != 0)
                {
                    return EXIT_FAILURE;
                }
                break;
            }
            else if (strcmp(long_options[option_index].name, "tf") == 0)
            {
                if (convert_tracefile_name(optarg, &tracefile, "tracefile") == 1)
                {
                    return EXIT_FAILURE;
                }
                break;
            }
            break;
        case 'c':
            if (convert_int(optarg, &cycles) != 0)
            {
                return EXIT_FAILURE;
            }
            break;
        case 'h':
            if (optind == argc)
            {
                fprintf(stdout, "Help message\n");
                print_help(program_name);
                return EXIT_SUCCESS;
            }
        default:
            fprintf(stderr, "Invalid option: %s\n", argv[optind]);
            print_usage(program_name);
            return EXIT_FAILURE;
        }
    }

    if (cycles < tlbsLatency)
    {
        fprintf(stderr, "Invalid option: number of cycles %d can not be less than the tlb Latency \n", cycles);
        return EXIT_FAILURE;
    }

    if (log2(blocksize) + log2(tlbSize) >= 31)
    {
        fprintf(stderr, "Invalid options: blocksize and tlb-size are too big for a 32 Bit architecture.\n");
        return EXIT_FAILURE;
    }
    if (optind == argc)
    {
        printf("Missing input file\n");
        print_usage(program_name);
        return EXIT_FAILURE;
    }

    if (set_input_file_name(argv[optind], &input_file_name) != 0)
    {
        return EXIT_FAILURE;
    }

    numRequests = get_numRequests(input_file_name);

    if (numRequests < 1)
    {
        fprintf(stderr, "Invalid input file: %s is empty.\n", input_file_name);
        return EXIT_FAILURE;
    }

    struct Request requests[numRequests];
    if (parse_input_file(input_file_name, requests) != 0)
    {
        return EXIT_FAILURE;
    }

    // here the treatment of the input file and checking if it is valid
    // after the checking is complete for everything
    struct Result result = run_simulation(
        cycles,
        tlbSize,
        tlbsLatency,
        blocksize,
        v2bBlockOffset,
        memoryLatency,
        numRequests,
        requests,
        tracefile);

    fprintf(stdout, "\n#Simulation Result:\nCycles: %zu\nMisses: %zu\nHits: %zu\nPrimitive Gate Count: %zu\n\n", result.cycles, result.misses, result.hits, result.primitiveGateCount);

    return EXIT_SUCCESS;
}