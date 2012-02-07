## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.e.mddl ${srcdir}/merge.a.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.a.mddl" > "${stdout}"

## merge.6.mt ends here
