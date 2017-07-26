# IncBin
A quick command line tool.
It converts data from a file into a C array that can then **be directly included in code**.

**usage:**

     incbin path [-h/--help] | [-o/--output output] | [-s/--size size] | [-b/--byte byte] | [-t/--offset offset] | [-c/--count count] | [-e/--endianness endianness]

**parameters:**

     -h, --help        - display this
     -o, --output      - output file [default: "data.c"]
     -s, --size        - number of columns per line [default: 16]
     -b, --byte        - number of bytes per element [default: 1]
     -t, --offset      - start offset [default: 0]
     -c, --count       - number of bytes to parse [default: 0]
     -e, --endianness  - specify if endianness has to be switched (0/1) [default: 0]

**example:**

     ./incbin icon.png -o icon.c -s 8
