/^---$/ { in_yaml = 1; getline }
/^\.\.\.$/ { in_yaml = 0; getline }
{ if (in_yaml) print $0 }
