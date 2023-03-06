BEGIN { in_fee = 1 }
/^---$/ { in_fee = 0; getline }
/^\.\.\.$/ { in_fee = 1; getline }
{ if (in_fee && $1 != "...") print $0 }
