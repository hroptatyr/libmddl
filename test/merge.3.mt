## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.a.mddl ${srcdir}/merge.b.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.c.mddl" > "${stdout}"

## merge.3.mt ends here
