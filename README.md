# concat

Commandline program to concatenate file lines. Supports a separator string, removing empty lines, removing leading
whitespaces, grouping and can be used in pipes.


## Background

When one googles how to join lines in e.g. Bash, then you might find [solutions like shown here](https://www.baeldung.com/linux/join-multiple-lines).
One of these solutions is a Posix commandline tool called `paste`, which already has a misleading name and uses in the
short description also the word "merge", which again is misleading in my opinion. So it was time to create something
that makes more sense.


## Building

    make


## Usage

    concat [-vhlwne] [-s <sep>] [-g <num>] [-S <sep>] [file]

You can't pass a file when text is piped into `concat`.


## Options

    -s <sep>  optional separator string
    -g <num>  add group separator string after <num> of lines
    -S <sep>  optional group separator string (default is newline)
    -e        interpret some few escape sequences (\\,\t,\n)
    -l        remove empty lines
    -w        remove leading whitespaces
    -n        do not output the trailing newline
    -v        print version info
    -h        print help text


## Examples

### Simple Concatenation

    $ cat file
    a
    b
    c
    $ concat file
    abc
    $ echo -e "a\nb\nc" | concat
    abc

### Concatenation With Separator With `-s`

    $ cat file
    a
    b
    c
    $ concat -s ',' file
    a,b,c
    $ cat file | tr '\n' ','
    a,b,c,
    $ concat -e -s '\t' file
    a    b    c
    $ concat -s ' - ' file
    a - b - c

Notice how `tr` would replace also the last newline with a comma, which sometimes is not what you want, e.g. when in CSV
files the last comma would create a new empty column.

### Concatenation And Removing Empty Lines With `-l`

    $ cat file
    a
    
    b
    
    c
    $ concat -s ';' file
    a;;b;;c
    $ concat -s ';' -l file
    a;b;c

### Concatenation And Removing Leading Whitespaces With `-w`

    $ cat file
    a
      b
        c
    $ concat -s ':' file
    a:  b:    c
    $ concat -s ':' -w file
    a:b:c

### Concatenate A Given Number Of Lines With `-g` and `-S`

    $ cat file
    a
    b
    c
    a
    b
    c
    $ concat file
    abcabc
    $ concat -g 3 file
    abc
    abc
    $ concat -e -g 3 -S '\n---\n' file
    abc
    ---
    abc

