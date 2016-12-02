# IncBin
A quick command line tool.
It converts data from a file into a C array that can then **be directly included in code**.

**usage:**

     incbin path [-h/--help] | [-o/--output output] | [-s/--size size]

**parameters:**

     -h, --help     - display this
     -o, --output   - output file [default: "data.c"]
     -s, --size     - number of columns per line [default: 16]

**example:**

     ./incbin icon.png -o icon.c -s 8
