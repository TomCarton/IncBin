//
//  main.c
//  incbin
//
//  Created by Thomas Carton.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


// lastPathComponentName: return the last component of a given path
static char *lastPathComponentName(const char *path)
{
    char *lname = strrchr(path, '/');
    lname = lname == NULL ? (char *)path : lname + 1;

    char *name = malloc(strlen(lname) + 1);
    strcpy(name, lname);

    char *rname = strrchr(name, '.');
    if (rname)
    {
        *rname = '\0';
    }

    return name;
}

// readline: read a line of size x data in file, return the size read
static unsigned int readline(FILE *file, unsigned char **line, unsigned int size)
{
    if (!file || !line || size < 1 || (!*line && !(*line = (unsigned char *)malloc(size))))
        return 0;

    unsigned int i;
    for (i = 0; i < size; ++i)
    {
        int c = fgetc(file);

        if (ferror(file))
            return 0;
        if (c == EOF)
            break;

        *(*line + i) = c;
    }

    return i;
}

int main(int argc, const char *argv[])
{
    int ret = 0;

    char infile[PATH_MAX];
    char outfile[FILENAME_MAX] = "data.c";
    unsigned int size = 16;

    unsigned int byte = 1;
    char *bsize[] = { "???", "char", "short", "???", "int" };

    unsigned int start = 0;
    unsigned int count = 0;

    int endianness = 0;

    if (argc < 2)
    {
        goto usage;
    }

    // read parameters
    for (unsigned int i = 0; i < argc; ++i)
    {
        // size
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0)
        {
            if (argc > i + 1)
            {
                sscanf(argv[i + 1], "%d", &size);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // byte
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--byte") == 0)
        {
            if (argc > i + 1)
            {
                sscanf(argv[i + 1], "%d", &byte);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // start offset
        else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--offset") == 0)
        {
            if (argc > i + 1)
            {
                sscanf(argv[i + 1], "%d", &start);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // count
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0)
        {
            if (argc > i + 1)
            {
                sscanf(argv[i + 1], "%d", &count);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // endianness
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--endianness") == 0)
        {
            if (argc > i + 1)
            {
                sscanf(argv[i + 1], "%d", &endianness);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // output
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
        {
            if (argc > i + 1)
            {
                strcpy(outfile, argv[i + 1]);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // help
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            goto usage;
        }
        // input
        else
        {
            strcpy(infile, argv[i]);
        }
    }

    char *name = lastPathComponentName(infile);

    // files
    FILE *in = NULL;
    FILE *out = NULL;

    // input
    if (!(in = fopen(infile, "r")))
    {
        fprintf(stderr, "failed to open `%s' for reading\n", infile);
        goto error;
    }

    // output
    if (!(out = fopen(outfile, "w")))
    {
        fprintf(stderr, "failed to open `%s' for output\n", outfile);
        goto error;
    }

	// get input file total size
	fseek(in, 0L, SEEK_END);
	long int inputFileSize = ftell(in);

    fseek(in, start, SEEK_SET);
    inputFileSize -= start;

    if (count && count < inputFileSize)
    {
        inputFileSize = count;
    }


    // size
    if (byte != 1 && byte != 2 && byte != 4)
    {
        fprintf(stderr, "incorrect byte size (%d, expected 1, 2 or 4)\n", byte);
        goto error;
    }


    fprintf(out, "// Generated file\n// %s\n\n", name);
    fprintf(out, "#ifndef %s_h_\n", name);
    fprintf(out, "#define %s_h_\n\n", name);

    fprintf(out, "unsigned %s %s[%ld] = \n{\n", bsize[byte], name, inputFileSize / byte);

    unsigned char *line = NULL;
    unsigned int linesize = size * byte;
    unsigned int read = 0;
    while ((read = readline(in, &line, linesize)) && inputFileSize > 0)
    {
        fprintf(out, "    ");
        for (unsigned int i = 0; i < read; ++i)
        {
            switch (8 * endianness + byte)
            {
                case 1:
                    fprintf(out, "0x%02X, ", line[i]);
                    break;
                case 2:
                    fprintf(out, "0x%02X%02X, ", line[i + 1], line[i]);
                    i += 1;
                    break;
                case 4:
                    fprintf(out, "0x%02X%02X%02X%02X, ", line[i + 3], line[i + 2], line[i + 1], line[i]);
                    i += 3;
                    break;

                case 9:
                    fprintf(out, "0x%02X, ", line[i]);
                    break;
                case 10:
                    fprintf(out, "0x%02X%02X, ", line[i], line[i + 1]);
                    i += 1;
                    break;
                case 12:
                    fprintf(out, "0x%02X%02X%02X%02X, ", line[i], line[i + 1], line[i + 2], line[i + 3]);
                    i += 3;
                    break;

                default:
                    break;
            }
        }
        fprintf(out, "\n");

        free(line);
        line = NULL;

        inputFileSize -= read;
    }


    fprintf(out, "};\n\n");
    fprintf(out, "#endif // %s_h_\n\n", name);

    free(name);
    fclose(out);
    fclose(in);

    if (ret == 0)
    {
        printf("generated '%s'\n", outfile);
    }
    else
    {
        remove(outfile);
    }

    return ret;

usage:
    fprintf(stderr, "\n");

    fprintf(stderr, "   usage:\n");
    fprintf(stderr, "     %s path [-h/--help] | [-o/--output output] | [-s/--size size] | [-b/--byte byte] | [-t/--offset offset] | [-c/--count count] | [-e/--endianness endianness]\n", lastPathComponentName(argv[0]));
    fprintf(stderr, "   \n");
    fprintf(stderr, "   parameters:\n");
    fprintf(stderr, "     -h, --help        - display this\n");
    fprintf(stderr, "     -o, --output      - output file [default: \"data.c\"]\n");
    fprintf(stderr, "     -s, --size        - number of columns per line [default: 16]\n");
    fprintf(stderr, "     -b, --byte        - number of bytes per element [default: 1]\n");
    fprintf(stderr, "     -t, --offset      - start offset [default: 0]\n");
    fprintf(stderr, "     -c, --count       - number of bytes to parse [default: 0]\n");
    fprintf(stderr, "     -e, --endianness  - specify if endianness has to be switched (0/1) [default: 0]\n");
    fprintf(stderr, "   \n");
    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s icon.png -o icon.c -s 8\n", argv[0]);

    fprintf(stderr, "\n");

    return 0;

error:
    if (in) fclose(in);
    if (out) fclose(out);

    free(name);
    return 1;
}

