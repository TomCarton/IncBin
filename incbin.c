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


// lastPathComponent: return the last component of a given path
static char *lastPathComponent(const char *path)
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

    char *name = lastPathComponent(infile);

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


    fprintf(out, "// Generated file\n// %s\n\n", name);
    fprintf(out, "#ifndef %s_h_\n", name);
    fprintf(out, "#define %s_h_\n\n", name);

    fprintf(out, "unsigned char %s[] = \n{\n", name);

    unsigned int len = 0;
    unsigned char *line = NULL;
    while ((len = readline(in, &line, size)))
    {
        fprintf(out, "    ");
        for (unsigned int i = 0; i < len; ++i)
        {
            fprintf(out, "0x%02X, ", line[i]);
        }
        fprintf(out, "\n");

        free(line);
        line = NULL;
    }

    fclose(in);

    fprintf(out, "};\n\n");
    fprintf(out, "#endif // %s_h_\n\n", name);

    fclose(out);

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
    fprintf(stderr, " %s path [-h/--help] | [-o/--output output]\n\n", argv[0]);

    fprintf(stderr, "   usage:\n");
    fprintf(stderr, "     -h, --help     - display this\n");
    fprintf(stderr, "     -o, --output   - output file [default is \"data.c\"]\n");

    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s icon.png -o icon.c\n", argv[0]);

    fprintf(stderr, "\n");

    return 0;

error:
    if (in) fclose(in);
    if (out) fclose(out);

    return 1;
}

