## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.a.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.a.mddl" > "${stdout}"

## merge.2.mt ends here
