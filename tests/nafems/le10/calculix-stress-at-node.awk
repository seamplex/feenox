#!/usr/bin/gawk
{
  # this only works for all-positive coordinates, otherwise we would have to use substr()
  if ($3 == "2.00000E+03" && $4 = "0.00000E+00" && $5 == "3.00000E+02")
  {
    node = $2
  }    
    
  
  if ($1 == -4 && $2 == "STRESS") {
    stresses = 1
  }
    
  if (node != 0 && stresses == 1 && found == 0) {
    if (strtonum(substr($0,4,10)) == node) {
      printf("%e\t", substr($0, 26, 12))
      found = 1
    }  
  }
}
