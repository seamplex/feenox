cat .gitignore | grep -v sparselizard | grep -v output | sed '/^#.*/ d' | sed '/^\s*$/ d' | sed 's/^/rm -rf /' | bash
