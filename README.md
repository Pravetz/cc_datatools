# cc_datatools
Pair of small utilities to efficiently extract strings from files
# EMSS
Program to extract strings from a message according to a list of keywords, given by the user.

Possible parameters:

    -i <path> = give input file path
    -o = give output file path
    -f <filtered words> -f = filter out strings containing given keywords
    -k <keywords> -k = only extract strings containing given keywords
# CSVCE
Program to extract column from a .csv file by it's index.

Possible parameters:

	-i = input file path
	-o = output file path
	-c = desired state.column(1-based).
	-noendl = discard(or replace, optionally) every occurence of '\n' character in strings.
