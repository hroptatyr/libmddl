## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.a.mddl ${srcdir}/merge.e.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.a.mddl" > "${stdout}"

## merge.7.mt ends here
