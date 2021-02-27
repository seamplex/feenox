{
  if (substr($1,1,1) != "#") {
    print ">", $0
  } else {
    print
    print ""
  }
}
