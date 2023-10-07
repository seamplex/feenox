cat .gitignore | sed '/^#.*/ d' | sed '/^\s*$/ d' | grep -v Stanford_Bunny.stl | sed 's/^/rm -rf /' | bash
